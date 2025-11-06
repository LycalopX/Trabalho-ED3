#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para ftruncate

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"

#include "../f4/f4.h"

typedef struct
{
    int idPessoa;

    // ID = 0, Idade = 1, Nome = 2, Usuario = 3
    int indiceDaRegra;

    // Caso o registro precise ser movido para o final, rrnAntigo > 0
    int rrnAntigo;
    int idPessoaAntigo;

} TarefaDeAtualizacao;

void inicializa_tarefa(TarefaDeAtualizacao *tarefas, int idPessoa, int *indexTarefa)
{

    tarefas[*indexTarefa].indiceDaRegra = 0;

    tarefas[*indexTarefa].idPessoa = idPessoa;
    tarefas[*indexTarefa].rrnAntigo = 0;
    tarefas[*indexTarefa].idPessoaAntigo = 0;
}

int comparar_tarefas_por_id(const void *a, const void *b)
{
    TarefaDeAtualizacao *tarefaA = (TarefaDeAtualizacao *)a;
    TarefaDeAtualizacao *tarefaB = (TarefaDeAtualizacao *)b;

    return (tarefaA->idPessoa - tarefaB->idPessoa);
}

int comparar_tarefas_por_byteoffset(const void *a, const void *b)
{
    TarefaDeAtualizacao *tarefaA = (TarefaDeAtualizacao *)a;
    TarefaDeAtualizacao *tarefaB = (TarefaDeAtualizacao *)b;

    if (tarefaA->rrnAntigo < tarefaB->rrnAntigo)
        return -1;
    else if (tarefaA->rrnAntigo > tarefaB->rrnAntigo)
        return 1;
    else
        return 0;
}

// Libera a memória de um array de ResultadoBuscaPessoa
static void liberar_resultados_busca(ResultadoBuscaPessoa *resultados, int nBuscas)
{
    if (resultados == NULL)
        return;

    for (int i = 0; i < nBuscas; i++)
    {
        if (resultados[i].registrosBusca != NULL)
        {
            for (int j = 0; j < resultados[i].nRegistros; j++)
            {
                if (resultados[i].registrosBusca[j] != NULL)
                {
                    destroi_registro_pessoa(resultados[i].registrosBusca[j]->registro);
                    free(resultados[i].registrosBusca[j]);
                }
            }
            free(resultados[i].registrosBusca);
        }
        if (resultados[i].busca.campo != NULL)
            free(resultados[i].busca.campo);
        if (resultados[i].busca.valor != NULL)
            free(resultados[i].busca.valor);
        if (resultados[i].update.campo != NULL)
            free(resultados[i].update.campo);
        if (resultados[i].update.valor != NULL)
            free(resultados[i].update.valor);
    }
    free(resultados);
}

static int atualizar_id(ResultadoBuscaPessoa *resultadosEmBuscas, TarefaDeAtualizacao *tarefas, Parametro *u,
                        RegistroBuscaPessoa **registrosBusca, int nTarefas, int nRegistros)
{

    int j = 0;
    for (j = 0; j < nRegistros; j++)
    {
        int indexTarefa = j + nTarefas;
        inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

        // Garantir que o novo valor não é igual ao atual (se for, não atualizar)
        if (atoi(u->valor) != registrosBusca[j]->registro->idPessoa)
        {
            // Para atualizar no índice posteriormente
            tarefas[indexTarefa].idPessoaAntigo = registrosBusca[j]->registro->idPessoa;

            registrosBusca[j]->registro->idPessoa = atoi(u->valor);
        }
        else
        {
            // Marcar como não atualizar
            registrosBusca[j]->ByteOffset = -1;
            // nIgnorados++;
        }
    }

    return j;
}

static int atualizar_idade(ResultadoBuscaPessoa *resultadosEmBuscas, TarefaDeAtualizacao *tarefas, Parametro *u,
                           RegistroBuscaPessoa **registrosBusca, int nTarefas, int nRegistros)
{

    int j = 0;
    for (j = 0; j < nRegistros; j++)
    {
        int indexTarefa = j + nTarefas;
        inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

        if (atoi(u->valor) != registrosBusca[j]->registro->idadePessoa)
        {
            // Se idade é nula
            if (strcmp(u->valor, "") == 0)
            {
                registrosBusca[j]->registro->idadePessoa = -1;
            }
            else
            {
                registrosBusca[j]->registro->idadePessoa = atoi(u->valor);
            }
        }
        else
        {
            // Marcar como não atualizar
            registrosBusca[j]->ByteOffset = -1;
            // nIgnorados++;
        }
    }

    return j;
}

static int atualizar_nome(ResultadoBuscaPessoa *resultadosEmBuscas, TarefaDeAtualizacao *tarefas, Parametro *u,
                          RegistroBuscaPessoa **registrosBusca, int nTarefas, int nRegistros, long long *proxByteOffset)
{
    int j = 0;
    for (j = 0; j < nRegistros; j++)
    {
        int indexTarefa = j + nTarefas;
        inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

        char *current_name = registrosBusca[j]->registro->nomePessoa;
        char *new_name = u->valor;

        int update_needed = 0;
        if (current_name == NULL)
        {
            if (strlen(new_name) > 0)
            {
                update_needed = 1;
            }
        }
        else
        {
            if (strcmp(new_name, current_name) != 0)
            {
                update_needed = 1;
            }
        }

        if (update_needed)
        {
            int tamanhoNomePessoa = strlen(new_name);
            int old_tamanhoNomePessoa = registrosBusca[j]->registro->tamanhoNomePessoa;

            if (current_name != NULL)
            {
                free(registrosBusca[j]->registro->nomePessoa);
            }
            registrosBusca[j]->registro->nomePessoa = strdup(new_name);
            registrosBusca[j]->registro->tamanhoNomePessoa = tamanhoNomePessoa;

            if (tamanhoNomePessoa > old_tamanhoNomePessoa)
            {

                // Novo nome maior que o atual, precisamos apagar o registro anterior e inserir no final
                tarefas[indexTarefa].rrnAntigo = registrosBusca[j]->ByteOffset;
                registrosBusca[j]->ByteOffset = *proxByteOffset;
                *proxByteOffset += sizeof(char) + sizeof(int) + registrosBusca[j]->registro->tamanhoRegistro;
                continue;
            }
        }
        else
        {
            // Marcar como não atualizar
            registrosBusca[j]->ByteOffset = -1;
            // nIgnorados++;
        }
    }

    return j;
}

static int atualizar_usuario(ResultadoBuscaPessoa *resultadosEmBuscas, TarefaDeAtualizacao *tarefas, Parametro *u,
                             RegistroBuscaPessoa **registrosBusca, int nTarefas, int nRegistros, long long *proxByteOffset)
{
    int j = 0;
    for (j = 0; j < nRegistros; j++)
    {
        int indexTarefa = j + nTarefas;
        inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

        char *current_username = registrosBusca[j]->registro->nomeUsuario;
        char *new_username = u->valor;

        int update_needed = 0;
        if (current_username == NULL)
        {
            if (strlen(new_username) > 0)
                update_needed = 1;
        }
        else
        {
            if (strcmp(new_username, current_username) != 0)
                update_needed = 1;
        }

        if (update_needed)
        {
            int tamanhoNomeUsuario = strlen(new_username);
            int old_tamanhoNomeUsuario = registrosBusca[j]->registro->tamanhoNomeUsuario;

            free(registrosBusca[j]->registro->nomeUsuario);

            registrosBusca[j]->registro->nomeUsuario = strdup(new_username);
            registrosBusca[j]->registro->tamanhoNomeUsuario = tamanhoNomeUsuario;

            if (tamanhoNomeUsuario > old_tamanhoNomeUsuario)
            {
                // Novo nome maior que o atual, precisamos apagar o registro anterior e inserir no final
                tarefas[indexTarefa].rrnAntigo = registrosBusca[j]->ByteOffset;
                registrosBusca[j]->ByteOffset = *proxByteOffset;
                *proxByteOffset += sizeof(char) + sizeof(int) + registrosBusca[j]->registro->tamanhoRegistro;
                continue;
            }
        }
        else
        {
            // Marcar como não atualizar
            registrosBusca[j]->ByteOffset = -1;
            // nIgnorados++;
        }
    }

    return j;
}

// Processa as atualizações para cada busca, preenchendo o array de tarefas
static void processar_atualizacoes_de_busca(int buscas, ResultadoBuscaPessoa *resultadosEmBuscas, TarefaDeAtualizacao *tarefas, long long *proxByteOffset)
{

    int nTarefas = 0;

    for (int i = 0; i < buscas; i++)
    {

        if (resultadosEmBuscas[i].nRegistros == 0 || resultadosEmBuscas[i].update.valor == NULL)
        {
            continue;
        }

        RegistroBuscaPessoa **registrosBusca = resultadosEmBuscas[i].registrosBusca;
        Parametro *u = &resultadosEmBuscas[i].update;

        // Fazer o for dentro da condição, para reduzir o número de comparações por iteração.
        if (strcmp(u->campo, CAMPO_ID) == 0)
        {
            int j = atualizar_id(resultadosEmBuscas, tarefas, u, registrosBusca, nTarefas, resultadosEmBuscas[i].nRegistros);
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_IDADE) == 0)
        {
            int j = atualizar_idade(resultadosEmBuscas, tarefas, u, registrosBusca, nTarefas, resultadosEmBuscas[i].nRegistros);
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_NOME) == 0)
        {
            int j = atualizar_nome(resultadosEmBuscas, tarefas, u, registrosBusca, nTarefas, resultadosEmBuscas[i].nRegistros, proxByteOffset);
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_USUARIO) == 0)
        {
            int j = atualizar_usuario(resultadosEmBuscas, tarefas, u, registrosBusca, nTarefas, resultadosEmBuscas[i].nRegistros, proxByteOffset);
            nTarefas += j;
        }
    }
}

static void unificarResultados(RegistroBuscaPessoa **resultados, TarefaDeAtualizacao *tarefas, int *nRegsEncontrados)
{
    // Implementar a unificação dos resultados, garantindo que cada registro único seja mantido
    // e que o ByteOffset seja o maior entre os registros duplicados.
    // Atualizar nRegsEncontrados conforme necessário.

    int write_idx = 0;
    for (int read_idx = 1; read_idx < (*nRegsEncontrados); read_idx++)
    {
        if (resultados[read_idx]->ByteOffset == -1)
        {
            // Ignora e libera registro marcado como não-atualizável
            destroi_registro_pessoa(resultados[read_idx]->registro);
            free(resultados[read_idx]);
            continue;
        }

        if (resultados[read_idx]->registro->idPessoa != resultados[write_idx]->registro->idPessoa)
        {
            // Encontrou um novo registro único, avança o ponteiro de escrita
            write_idx++;
            resultados[write_idx] = resultados[read_idx];
            tarefas[write_idx] = tarefas[read_idx];
        }
        else
        {
            // Encontrou uma duplicata, funde a atualização no registro em write_idx
            switch (tarefas[read_idx].indiceDaRegra)
            {
            case 0: // ID
                resultados[write_idx]->registro->idPessoa = resultados[read_idx]->registro->idPessoa;
                break;
            case 1: // Idade
                resultados[write_idx]->registro->idadePessoa = resultados[read_idx]->registro->idadePessoa;
                break;
            case 2: // Nome
                free(resultados[write_idx]->registro->nomePessoa);
                resultados[write_idx]->registro->nomePessoa = strdup(resultados[read_idx]->registro->nomePessoa);
                resultados[write_idx]->registro->tamanhoNomePessoa = resultados[read_idx]->registro->tamanhoNomePessoa;
                break;
            case 3: // Usuario
                free(resultados[write_idx]->registro->nomeUsuario);
                resultados[write_idx]->registro->nomeUsuario = strdup(resultados[read_idx]->registro->nomeUsuario);
                resultados[write_idx]->registro->tamanhoNomeUsuario = resultados[read_idx]->registro->tamanhoNomeUsuario;
                break;
            }

            // Digamos que o byteoffset seja -1 para apenas uma das mudanças de um registro, ou que uma das alterações registros tenha o movido para o final
            // O byteoffset entre as duas modificações sempre deve ser o maior
            resultados[write_idx]->ByteOffset = (resultados[write_idx]->ByteOffset > resultados[read_idx]->ByteOffset) ? resultados[write_idx]->ByteOffset : resultados[read_idx]->ByteOffset;

            // Libera a memória do registro duplicado que foi fundido
            destroi_registro_pessoa(resultados[read_idx]->registro);
            free(resultados[read_idx]);
        }
    }
    (*nRegsEncontrados) = write_idx + 1;
}

static int aplicar_atualizacoes_de_busca(FILE *fp, RegistroIndice **indice_em_memoria, TarefaDeAtualizacao *tarefas,
                                          int nRegsEncontrados, CabecalhoPessoa cp, RegistroBuscaPessoa **resultados)
{

    fseek(fp, 17, SEEK_SET);
    long long cursor_position = 17;

    // Itera sobre os registros únicos e consolidados para aplicar as atualizações.
    for (int i = 0; i < nRegsEncontrados; i++)
    {

        RegistroPessoa *reg_atualizado = resultados[i]->registro;

        // offsets
        long long original_offset = tarefas[i].rrnAntigo; // antigo
        long long byteOffset = resultados[i]->ByteOffset; // atual

        // tamanhos de registro
        int tamanho_novo_dados = sizeof(int) * 4 + reg_atualizado->tamanhoNomePessoa + reg_atualizado->tamanhoNomeUsuario; // atual
        int tamanho_antigo_dados = reg_atualizado->tamanhoRegistro;                                                        // antigo

        // tamanho total escrito
        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + tamanho_novo_dados;
        long long diffByteOffset = byteOffset - cursor_position;

        if (tamanho_novo_dados <= tamanho_antigo_dados)
        {
            // ATUALIZAÇÃO IN-PLACE
            fseek(fp, diffByteOffset, SEEK_CUR);
            reg_atualizado->tamanhoRegistro = tamanho_antigo_dados; // Mantém o tamanho do espaço original
            escreve_registro_pessoa(fp, reg_atualizado);

            // Preenche o lixo com '$'
            int diff = tamanho_antigo_dados - tamanho_novo_dados;
            if (diff > 0)
            {
                char *lixo = malloc(diff);
                memset(lixo, '$', diff);
                fwrite(lixo, 1, diff, fp);
                free(lixo);
            }
        }
        else
        {
            // ATUALIZAÇÃO OUT-OF-PLACE (REMOVER E INSERIR)

            // Marca o registro antigo como removido
            fseek(fp, original_offset, SEEK_SET);
            char removido_flag = '1';
            fwrite(&removido_flag, sizeof(char), 1, fp);

            cp.quantidadeRemovidos++;
            cp.quantidadePessoas--; // Decremento temporário

            // Insere o novo registro no final do arquivo
            fseek(fp, byteOffset - (original_offset - 1), SEEK_CUR);
            reg_atualizado->tamanhoRegistro = tamanho_novo_dados;

            escreve_registro_pessoa(fp, reg_atualizado);
            cp.quantidadePessoas++; // Incrementa de volta
        }

        // Se o ID mudou, precisamos atualizar o índice.
        if (tarefas[i].indiceDaRegra == 0 || tarefas[i].rrnAntigo > 0)
        {
            // pegar maior entre id busca e id atual
            int id_busca;

            if (tarefas[i].idPessoaAntigo == 0)
            {
                id_busca = reg_atualizado->idPessoa;
            }
            else
            {
                id_busca = tarefas[i].idPessoaAntigo;
            }

            // Lógica para atualizar o índice em memória (remover id_original, adicionar id_novo)
            RegistroIndice **p_encontrado_ptr = bsearch(&id_busca,
                                                        indice_em_memoria,
                                                        cp.quantidadePessoas,
                                                        sizeof(RegistroIndice *),
                                                        comparar_bsearch_indice);

            if (p_encontrado_ptr == NULL)
            {
                printf(FALHA_AO_PROCESSAR_ARQUIVO);
                free(indice_em_memoria);
                return -1;
            }

            if (tarefas[i].indiceDaRegra == 0)
            {
                // Muda o ID
                (*p_encontrado_ptr)->idPessoa = reg_atualizado->idPessoa;
            }
            else
            {
                // Muda Byteoffset
                (*p_encontrado_ptr)->byteOffset = byteOffset;
            }
        }

        byteOffset += diffByteOffset + tamanho_real_escrito;
    }
    return 0;
}

static void reescrever_arquivo_indice (FILE *fpIndice, RegistroIndice **indice_em_memoria, CabecalhoPessoa cp) {
    
    CabecalhoIndice index_header;
    le_cabecalho_indice(fpIndice, &index_header);

    toggle_cabecalho_indice(fpIndice, &index_header);

    fseek(fpIndice, 12, SEEK_SET);
    for (int i = 0; i < cp.quantidadePessoas; i++)
    {
        if (indice_em_memoria[i] != NULL && indice_em_memoria[i]->byteOffset > 0)
        {
            fwrite(&indice_em_memoria[i]->idPessoa, sizeof(int), 1, fpIndice);
            fwrite(&indice_em_memoria[i]->byteOffset, sizeof(long long), 1, fpIndice);
        }
        destroi_registro_indice(indice_em_memoria[i]);
    }
    free(indice_em_memoria);

    long finalPos = ftell(fpIndice);
    ftruncate(fileno(fpIndice), finalPos);

    toggle_cabecalho_indice(fpIndice, &index_header);
}

int funcionalidade7(FILE *fp, FILE *fpIndice, int buscas)
{
    /*
    Permita  a  atualização  dos  registros  do  arquivo  de  dados  pessoa
    */

    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 1);

    TarefaDeAtualizacao *tarefas = malloc(sizeof(TarefaDeAtualizacao) * nRegsEncontrados);
    if (tarefas == NULL)
    {
        printf(FALHA_AO_ALOCAR);
        // Free resultadosEmBuscas if tarefas allocation fails
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);

    CabecalhoPessoa cp;
    le_cabecalho_pessoa(fp, &cp); // Lê o cabeçalho para posicionar o ponteiro corretamente
    toggle_cabecalho_pessoa(fp, &cp);

    long long proxByteOffset = cp.proxByteOffset;

    // Ignorar casos onde a busca não retorna registros
    // Usar resultadosEmBuscas para fazer as atualizações

    /*
    Campos a serem atualizados com valores nulos devem ser identificados, na entrada da
    funcionalidade,  com  NULO.  Os  valores  dos  campos  do  tipo  string  devem  ser
    especificados entre aspas duplas ("). Para a manipulação de strings com aspas duplas,
    pode-se  usar  a  função  scan_quote_string  disponibilizada  na  página  do  projeto  da
    disciplina. Não será solicitada a atualização dos campos idPessoa e nomeUsuario de
    com  valores  nulos  ou  repetidos.
    */
    // Mesma especificação do exercício 6 para leitura dos campos a serem atualizados

    processar_atualizacoes_de_busca(buscas, resultadosEmBuscas, tarefas, &proxByteOffset);

    // Juntar tudo em uma única array para usar em outras funções
    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));

    if (resultados == NULL && nRegsEncontrados > 0)
    {
        printf(FALHA_AO_ALOCAR);
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        free(tarefas);
        return -1;
    }

    // Transfere os resultados e libera a memória
    liberar_resultados_busca(resultadosEmBuscas, buscas);

    if (nRegsEncontrados == 0)
    {
        free(tarefas);
        free(resultados);
        return 0;
    }

    // Não esquecer de unificar resultados (se tiver um registro em mais de uma busca)
    // Para unificação de dois resultados diferentes, sempre usar o byteOffset de maior valor entre os dois

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_id);
    qsort(tarefas, nRegsEncontrados, sizeof(TarefaDeAtualizacao), comparar_tarefas_por_id);

    unificarResultados(resultados, tarefas, &nRegsEncontrados);

    /*
    Quando o tamanho do registro atualizado for maior do que o tamanho do
    registro atual, o registro atual deve ser logicamente removido e o registro atualizado
    deve ser inserido como um novo registro

    Caso contrário, o  lixo  que  porventura  permanecer  no  registro  atualizado  deve  ser
    identificado pelo caractere ‘$’
    */
    // Fazer remoção/inserção, ou só inserção conforme o caso
    // Atualizar registro

    // Carrega o índice completo em memória para realizar as atualizações.
    RegistroIndice **indice_em_memoria = carregar_indice_inteiro(fpIndice, cp.quantidadePessoas);

    if (indice_em_memoria == NULL && cp.quantidadePessoas > 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(tarefas);
        free(resultados);
        return -1;
    }

    // Para podermos avançar o cursor, em vez de ter que dar set_seek toda hora
    qsort(tarefas, nRegsEncontrados, sizeof(TarefaDeAtualizacao), comparar_tarefas_por_byteoffset);
    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_offset);

    if (aplicar_atualizacoes_de_busca(fp, indice_em_memoria, tarefas, nRegsEncontrados, cp, resultados) < 0) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(tarefas);
        free(resultados);
        return -1;
    };

    // Lógica para reescrever o arquivo de índice a partir do índice em memória, similar à funcionalidade 6.
    qsort(indice_em_memoria, cp.quantidadePessoas, sizeof(RegistroIndice *), comparar_indices_id);

    reescrever_arquivo_indice(fpIndice, indice_em_memoria, cp);

    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    toggle_cabecalho_pessoa(fp, &cp);
    fflush(fp);
    fflush(fpIndice);

    // Libera a memória alocada para as tarefas e resultados
    if (tarefas != NULL)
    {
        free(tarefas);
    }

    if (resultados != NULL)
    {
        for (int i = 0; i < nRegsEncontrados; i++)
        {
            if (resultados[i] != NULL)
            {
                destroi_registro_pessoa(resultados[i]->registro);
                free(resultados[i]);
            }
        }
        free(resultados);
    }

    return 0;
}