#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f7.h"
#include "../f4/f4.h"

typedef struct
{
    // ID
    int idPessoa;
    // ID = 0, Idade = 1, Nome = 2, Usuario = 3
    int indiceDaRegra;
    // Caso o registro precise ser movido para o final, rrnAntigo > 0
    int rrnAntigo;
} TarefaDeAtualizacao;

void comparar_tarefas_por_id(const void *a, const void *b)
{
    TarefaDeAtualizacao *tarefaA = (TarefaDeAtualizacao *)a;
    TarefaDeAtualizacao *tarefaB = (TarefaDeAtualizacao *)b;

    return (tarefaA->idPessoa - tarefaB->idPessoa);
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
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    CabecalhoPessoa cp;
    le_cabecalho_pessoa(fp, &cp); // Lê o cabeçalho para posicionar o ponteiro corretamente

    cp.status = '0';
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

    int nIgnorados = 0;

    for (int i = 0; i < buscas; i++)
    {

        if (resultadosEmBuscas[i].nRegistros == 0)
        {
            printf("Registro inexistente.\n");
        }

        RegistroBuscaPessoa **registrosBusca = resultadosEmBuscas[i].registrosBusca;
        Parametro *u = &resultadosEmBuscas[i].update;

        // Para unificarmos depois (aplicação de qsort)
        RegistroBuscaPessoa **regs = malloc(sizeof(RegistroBuscaPessoa *) * nRegsEncontrados);

        if (!regs)
        {
            printf(FALHA_AO_ALOCAR);
            continue; // Skip this search
        }

        if (u->valor != NULL)
        {
            continue; // Skip if there's no update value
        }

        tarefas[i].idPessoa = registrosBusca[0]->registro->idPessoa; // ID do primeiro registro (todos são iguais)
        tarefas[i].rrnAntigo = 0;

        // Fazer o for dentro da condição, para reduzir o número de comparações por iteração.
        if (strcmp(u->campo, CAMPO_ID) == 0)
        {
            tarefas[i].indiceDaRegra = 0;
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                // Garantir que o novo valor não é igual ao atual (se for, não atualizar)
                if (strcmp(u->valor, registrosBusca[j]->registro->idPessoa) != 0)
                {
                    registrosBusca[j]->registro->idPessoa = atoi(u->valor);
                    regs[j] = registrosBusca[j];
                }
                else
                {
                    // Marcar como não atualizar
                    registrosBusca[j]->ByteOffset = -1;
                    nIgnorados++;
                }
            }
        }
        else if (strcmp(u->campo, CAMPO_IDADE) == 0)
        {
            tarefas[i].indiceDaRegra = 1;
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                if (strcmp(u->valor, registrosBusca[j]->registro->idadePessoa) != 0)
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
                    regs[j] = registrosBusca[j];
                }
                else
                {
                    // Marcar como não atualizar
                    registrosBusca[j]->ByteOffset = -1;
                    nIgnorados++;
                }
            }
        }
        else if (strcmp(u->campo, CAMPO_NOME) == 0)
        {
            tarefas[i].indiceDaRegra = 2;
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                if (strcmp(u->valor, registrosBusca[j]->registro->nomePessoa) != 0)
                {
                    int tamanhoNomePessoa = strlen(u->valor);
                    if (tamanhoNomePessoa > registrosBusca[j]->registro->tamanhoNomePessoa)
                    {
                        // Novo nome maior que o atual, precisamos apagar o registro anterior e inserir no final
                        tarefas[i].rrnAntigo = registrosBusca[j]->ByteOffset;
                        registrosBusca[j]->ByteOffset = proxByteOffset;
                        proxByteOffset += sizeof(char) + sizeof(int) + registrosBusca[j]->registro->tamanhoRegistro;
                        continue;
                    }

                    free(registrosBusca[j]->registro->nomePessoa);
                    registrosBusca[j]->registro->nomePessoa = strdup(u->valor);
                    registrosBusca[j]->registro->tamanhoNomePessoa = tamanhoNomePessoa;
                    regs[j] = registrosBusca[j];
                }
                else
                {
                    // Marcar como não atualizar
                    registrosBusca[j]->ByteOffset = -1;
                    nIgnorados++;
                }
            }
        }
        else if (strcmp(u->campo, CAMPO_USUARIO) == 0)
        {
            tarefas[i].indiceDaRegra = 3;
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                if (strcmp(u->valor, registrosBusca[j]->registro->nomeUsuario) != 0)
                {
                    int tamanhoNomePessoa = strlen(u->valor);
                    if (tamanhoNomePessoa > registrosBusca[j]->registro->tamanhoNomePessoa)
                    {
                        // Novo nome maior que o atual, precisamos apagar o registro anterior e inserir no final
                        tarefas[i].rrnAntigo = registrosBusca[j]->ByteOffset;
                        registrosBusca[j]->ByteOffset = proxByteOffset;
                        proxByteOffset += sizeof(char) + sizeof(int) + registrosBusca[j]->registro->tamanhoRegistro;
                        continue;
                    }

                    free(registrosBusca[j]->registro->nomeUsuario);
                    registrosBusca[j]->registro->nomeUsuario = strdup(u->valor);
                    registrosBusca[j]->registro->tamanhoNomeUsuario = tamanhoNomePessoa;
                    regs[j] = registrosBusca[j];
                }
                else
                {
                    // Marcar como não atualizar
                    registrosBusca[j]->ByteOffset = -1;
                    nIgnorados++;
                }
            }
        }
    }

    // Juntar tudo em uma única array para usar em outras funções
    int index = 0;
    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));

    // Transfere os resultados e libera a memória
    if (resultadosEmBuscas != NULL)
    {
        for (int i = 0; i < buscas; i++)
        {
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                resultados[index] = resultadosEmBuscas[i].registrosBusca[j];
                index++;
            }
            free(resultadosEmBuscas[i].registrosBusca);
        }
        free(resultadosEmBuscas); // Libera o array de ponteiros
    }

    // Não esquecer de unificar resultados (se tiver um registro em mais de uma busca)
    // Para unificação de dois resultados diferentes, sempre usar o byteOffset de maior valor entre os dois

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_id);
    qsort(tarefas, nRegsEncontrados, sizeof(TarefaDeAtualizacao), comparar_tarefas_por_id);

    if (nRegsEncontrados > 0)
    {
        int write_idx = 0;
        for (int read_idx = 1; read_idx < nRegsEncontrados; read_idx++)
        {
            if (resultados[read_idx]->ByteOffset == -1)
            {
                // Ignora e libera registro marcado como não-atualizável
                destroi_registro(resultados[read_idx]->registro);
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
                destroi_registro(resultados[read_idx]->registro);
                free(resultados[read_idx]);
            }
        }
        nRegsEncontrados = write_idx + 1;
    }

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
    if (indice_em_memoria == NULL && cp.quantidadePessoas > 0) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        // TODO: Liberar memória de `resultados` e `tarefas`
        return -1;
    }

    // Itera sobre os registros únicos e consolidados para aplicar as atualizações.
    for (int i = 0; i < nRegsEncontrados; i++) {
        long long original_offset = resultados[i]->ByteOffset;
        RegistroPessoa *reg_atualizado = resultados[i]->registro;

        // Para obter o tamanho e ID originais, precisamos ler o registro do disco.
        fseek(fp, original_offset, SEEK_SET);
        char c;
        int tamanho_antigo;
        int id_original;
        fread(&c, sizeof(char), 1, fp); // Pula o campo 'removido'
        fread(&tamanho_antigo, sizeof(int), 1, fp);
        fread(&id_original, sizeof(int), 1, fp);

        int novo_tamanho = sizeof(int) * 3 + reg_atualizado->tamanhoNomePessoa + reg_atualizado->tamanhoNomeUsuario;

        if (novo_tamanho <= tamanho_antigo) {
            // ATUALIZAÇÃO IN-PLACE
            fseek(fp, original_offset, SEEK_SET);
            reg_atualizado->tamanhoRegistro = tamanho_antigo; // Mantém o tamanho do espaço original
            escreve_registro_pessoa(fp, reg_atualizado);

            // Preenche o lixo com '$'
            int diff = tamanho_antigo - novo_tamanho;
            if (diff > 0) {
                char *lixo = malloc(diff);
                memset(lixo, '$', diff);
                fwrite(lixo, 1, diff, fp);
                free(lixo);
            }

            // Se o ID mudou, precisamos atualizar o índice.
            if (reg_atualizado->idPessoa != id_original) {
                // TODO: Lógica para atualizar o índice em memória (remover id_original, adicionar id_novo)
            }
        } else {
            // ATUALIZAÇÃO OUT-OF-PLACE (REMOVER E INSERIR)
            // Marca o registro antigo como removido
            fseek(fp, original_offset, SEEK_SET);
            char removido_flag = '1';
            fwrite(&removido_flag, sizeof(char), 1, fp);
            cp.quantidadeRemovidos++;
            cp.quantidadePessoas--; // Decremento temporário

            // Insere o novo registro no final do arquivo
            fseek(fp, cp.proxByteOffset, SEEK_SET);
            long long novo_offset = cp.proxByteOffset;
            reg_atualizado->tamanhoRegistro = novo_tamanho;
            escreve_registro_pessoa(fp, reg_atualizado);
            cp.proxByteOffset = ftell(fp);
            cp.quantidadePessoas++; // Incrementa de volta

            // TODO: Lógica para atualizar o índice em memória (atualizar byteOffset para id_original ou remover/adicionar se ID mudou)
        }
    }

    // TODO: Lógica para reescrever o arquivo de índice a partir do índice em memória, similar à funcionalidade 6.

    // Atualiza o cabeçalho do arquivo de dados
    fseek(fp, 0, SEEK_SET);
    cp.status = '1';
    escreve_cabecalho_pessoa(fp, &cp);

    // TODO: Liberar toda a memória alocada (resultados, tarefas, indice_em_memoria).

    return 0;
}