#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para ftruncate

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"

#include "../f4/f4.h"
#include "tarefas.h"

// Atualizar campos como ID, idade
static int atualizar_campo_inteiro(RegistroBuscaPessoa *registroBusca, TarefaDeAtualizacao *tarefa, Parametro *u, int *campo_registro)
{
    if (atoi(u->valor) != *campo_registro)
    {
        tarefa->idPessoaAntigo = registroBusca->registro->idPessoa;
        *campo_registro = atoi(u->valor);
    }
    else
    {
        registroBusca->ByteOffset = -1;
    }
    return 0;
}

// Atualizar campos como nome, usuario
static int atualizar_campo_string(RegistroBuscaPessoa *registroBusca, TarefaDeAtualizacao *tarefa, Parametro *u, char **campo_registro, int *tamanho_campo, long long *proxByteOffset)
{
    char *current_string = *campo_registro;
    char *new_string = u->valor;

    int update_needed = 0;
    if (current_string == NULL)
    {
        if (strlen(new_string) > 0)
        {
            update_needed = 1;
        }
    }
    else
    {
        if (strcmp(new_string, current_string) != 0)
        {
            update_needed = 1;
        }
    }

    if (update_needed)
    {
        int new_size = strlen(new_string);
        int old_size = *tamanho_campo;

        if (current_string != NULL)
        {
            free(*campo_registro);
        }
        *campo_registro = strdup(new_string);
        *tamanho_campo = new_size;

        if (new_size > old_size)
        {
            tarefa->rrnAntigo = registroBusca->ByteOffset;
            registroBusca->ByteOffset = *proxByteOffset;
            *proxByteOffset += sizeof(char) + sizeof(int) + registroBusca->registro->tamanhoRegistro;
        }
    }
    else
    {
        registroBusca->ByteOffset = -1;
    }

    return 0;
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

        int j = 0;
        int nRegistros = resultadosEmBuscas[i].nRegistros;

        // Fazer o for dentro da condição, para reduzir o número de comparações por iteração.
        if (strcmp(u->campo, CAMPO_ID) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

                atualizar_campo_inteiro(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->idPessoa);
            }
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_IDADE) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

                atualizar_campo_inteiro(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->idadePessoa);
            }
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_NOME) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

                atualizar_campo_string(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->nomePessoa, &registrosBusca[j]->registro->tamanhoNomePessoa, proxByteOffset);
            }
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_USUARIO) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, &indexTarefa);

                atualizar_campo_string(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->nomeUsuario, &registrosBusca[j]->registro->tamanhoNomeUsuario, proxByteOffset);
            }
            nTarefas += j;
        }
    }
}

static int unificarResultados(RegistroBuscaPessoa **resultados, TarefaDeAtualizacao *tarefas, int *nRegsEncontrados)
{
    // Implementar a unificação dos resultados, garantindo que cada registro único seja mantido
    // e que o ByteOffset seja o maior entre os registros duplicados.
    // Atualizar nRegsEncontrados conforme necessário.

    int write_idx = 0;
    int nRemovidosInseridos = 0;
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

            // Para sabermos o tamanho do vetor antes de alocar na funcionalidade aplicar_atualizacoes_de_busca
            int tamanho_novo_dados = sizeof(int) * 4 + resultados[read_idx]->registro->tamanhoNomePessoa + resultados[read_idx]->registro->tamanhoNomeUsuario; // atual
            int tamanho_antigo_dados = resultados[read_idx]->registro->tamanhoRegistro;                                                                        // antigo

            if (tamanho_antigo_dados < tamanho_novo_dados)
            {
                nRemovidosInseridos++;
            }
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
    return nRemovidosInseridos;
}

static int aplicar_atualizacoes_de_busca(FILE *fp, RegistroIndice **indice_em_memoria, TarefaDeAtualizacao *tarefas,
                                         int nRegsEncontrados, int nRemovidosInseridos, CabecalhoPessoa cp, RegistroBuscaPessoa **resultados)
{

    fseek(fp, 17, SEEK_SET);
    long long cursor_position = 17;

    // Para remover e inserir depois
    RegistroBuscaPessoa **removidos_inseridos = malloc(nRemovidosInseridos * sizeof(RegistroBuscaPessoa *));
    int removidos_inseridos_idx = 0;

    // Itera sobre os registros únicos e consolidados para aplicar as atualizações.
    for (int i = 0; i < nRegsEncontrados; i++)
    {
        RegistroPessoa *reg_atualizado = resultados[i]->registro;

        // offsets
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
            removidos_inseridos[removidos_inseridos_idx] = malloc(sizeof(RegistroBuscaPessoa));
            removidos_inseridos[removidos_inseridos_idx]->registro = reg_atualizado;
            removidos_inseridos[removidos_inseridos_idx]->ByteOffset = byteOffset;
            removidos_inseridos_idx++;
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

    remover_pessoas_e_indices(removidos_inseridos, indice_em_memoria, cp, nRemovidosInseridos, fp, 1);
    inserir_pessoas(fp, removidos_inseridos, nRemovidosInseridos);
    return 0;
}

void copiar_resultados_busca_para_array(ResultadoBuscaPessoa *resultadosEmBuscas, int buscas, RegistroBuscaPessoa **resultados, int nRegsEncontrados)
{
    int current_idx = 0;
    for (int i = 0; i < buscas; i++)
    {
        for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
        {
            resultados[current_idx++] = resultadosEmBuscas[i].registrosBusca[j];
        }
        // Libera apenas a estrutura ResultadoBuscaPessoa, não os registros internos
        free(resultadosEmBuscas[i].registrosBusca);
    }
    free(resultadosEmBuscas);
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

    if ((resultados == NULL && nRegsEncontrados > 0))
    {
        printf(FALHA_AO_ALOCAR);
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        free(tarefas);
        return -1;
    }
    if (nRegsEncontrados == 0) {
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        free(tarefas);
        return 0;
    }

    // Copia resultados para uma única array e libera-os
    copiar_resultados_busca_para_array(resultadosEmBuscas, buscas, resultados, nRegsEncontrados);

    // Não esquecer de unificar resultados (se tiver um registro em mais de uma busca)
    // Para unificação de dois resultados diferentes, sempre usar o byteOffset de maior valor entre os dois

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_id);
    qsort(tarefas, nRegsEncontrados, sizeof(TarefaDeAtualizacao), comparar_tarefas_por_id);

    int nRemovidosInseridos = unificarResultados(resultados, tarefas, &nRegsEncontrados);

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

    if (aplicar_atualizacoes_de_busca(fp, indice_em_memoria, tarefas, nRegsEncontrados, nRemovidosInseridos, cp, resultados) < 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(tarefas);
        free(resultados);
        return -1;
    };

    // Lógica para reescrever o arquivo de índice a partir do índice em memória, similar à funcionalidade 6.
    qsort(indice_em_memoria, cp.quantidadePessoas, sizeof(RegistroIndice *), comparar_indices_id);

    reescrever_arquivo_indice(fpIndice, indice_em_memoria, cp.quantidadePessoas);

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