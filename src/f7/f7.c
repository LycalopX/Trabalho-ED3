#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"
#include "../f4/f4.h"
#include "atualizacao.h"

// Aplica uma atualização a um campo de tipo inteiro de um registro.
static int atualizar_campo_inteiro(Atualizacao *atualizacao, Parametro *u, int campo_registro, int *campo_registro_atualizacao)
{
    if (atoi(u->valor) != campo_registro)
    {
        *campo_registro_atualizacao = atoi(u->valor);
    }
    else
    {

        atualizacao->ByteOffset = -1;
    }
    return 0;
}

// Aplica uma atualização a um campo de tipo string de um registro.
static int atualizar_campo_string(Atualizacao *atualizacao, Parametro *u, char *campo_registro, char **campo_registro_atualizacao, int *tamanho_campo)
{
    char *current_string = campo_registro;
    char *new_string = u->valor;

    int update_needed = 0;
    if (current_string == NULL)
    {
        if (strlen(new_string) > 0)
            update_needed = 1;
    }
    else
    {
        if (strcmp(new_string, current_string) != 0)
            update_needed = 1;
    }

    if (!update_needed)
    {
        atualizacao->ByteOffset = -1;
        return 0;
    }

    if (*campo_registro_atualizacao != NULL)
    {
        free(*campo_registro_atualizacao);
    }
    *campo_registro_atualizacao = strdup(new_string);
    *tamanho_campo = strlen(new_string);

    return 0;
}

// Converte os resultados da busca em uma lista de tarefas de atualização.
static int processar_atualizacoes_de_busca(int buscas, ResultadoBuscaPessoa *resultadosEmBuscas, Atualizacao *atualizacoes)
{
    int nAtualizacoes = 0;
    for (int i = 0; i < buscas; i++)
    {
        if (resultadosEmBuscas[i].nRegistros == 0 || resultadosEmBuscas[i].update.valor == NULL)
        {
            continue;
        }

        RegistroBuscaPessoa **registrosBusca = resultadosEmBuscas[i].registrosBusca;
        Parametro *u = &resultadosEmBuscas[i].update;
        int nRegistros = resultadosEmBuscas[i].nRegistros;

        int j = 0;

        if (strcmp(u->campo, CAMPO_ID) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexAtualizacao = j + nAtualizacoes;
                inicializa_atualizacao(&atualizacoes[indexAtualizacao], registrosBusca[j]->registro,
                                       registrosBusca[j]->ByteOffset, 0);
                atualizar_campo_inteiro(&atualizacoes[indexAtualizacao], u, registrosBusca[j]->registro->idPessoa,
                                        &atualizacoes[indexAtualizacao].idPessoaNovo);
            }

            nAtualizacoes += j;
        }

        else if (strcmp(u->campo, CAMPO_IDADE) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexAtualizacao = j + nAtualizacoes;
                inicializa_atualizacao(&atualizacoes[indexAtualizacao], registrosBusca[j]->registro,
                                       registrosBusca[j]->ByteOffset, 1);
                atualizar_campo_inteiro(&atualizacoes[indexAtualizacao], u, registrosBusca[j]->registro->idadePessoa,
                                        &atualizacoes[indexAtualizacao].registro->idadePessoa);
            }

            nAtualizacoes += j;
        }

        else if (strcmp(u->campo, CAMPO_NOME) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexAtualizacao = j + nAtualizacoes;
                inicializa_atualizacao(&atualizacoes[indexAtualizacao], registrosBusca[j]->registro,
                                       registrosBusca[j]->ByteOffset, 2);
                atualizar_campo_string(&atualizacoes[indexAtualizacao], u, registrosBusca[j]->registro->nomePessoa,
                                       &atualizacoes[indexAtualizacao].registro->nomePessoa, &atualizacoes[indexAtualizacao].registro->tamanhoNomePessoa);
            }

            nAtualizacoes += j;
        }

        else if (strcmp(u->campo, CAMPO_USUARIO) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexAtualizacao = j + nAtualizacoes;
                inicializa_atualizacao(&atualizacoes[indexAtualizacao], registrosBusca[j]->registro,
                                       registrosBusca[j]->ByteOffset, 3);
                atualizar_campo_string(&atualizacoes[indexAtualizacao], u, registrosBusca[j]->registro->nomeUsuario,
                                       &atualizacoes[indexAtualizacao].registro->nomeUsuario, &atualizacoes[indexAtualizacao].registro->tamanhoNomeUsuario);
            }

            nAtualizacoes += j;
        }
    }

    return nAtualizacoes;
}

// Unifica múltiplas atualizações para o mesmo registro em uma única tarefa.
static void unificarResultados(Atualizacao *atualizacoes, int *nRegsEncontrados)
{
    if (*nRegsEncontrados == 0)
        return;

    int write_idx = 0;
    if (*nRegsEncontrados > 1)
    {
        for (int read_idx = 1; read_idx < (*nRegsEncontrados); read_idx++)
        {
            if (atualizacoes[read_idx].ByteOffset == -1)
            {
                destroi_registro_pessoa(atualizacoes[read_idx].registro);
                continue;
            }

            if (atualizacoes[read_idx].idOriginal != atualizacoes[write_idx].idOriginal)
            {
                write_idx++;
                if (write_idx != read_idx)
                {
                    atualizacoes[write_idx] = atualizacoes[read_idx];
                }
            }
            else
            {
                switch (atualizacoes[read_idx].indiceDaRegra)
                {
                case 0:
                    atualizacoes[write_idx].idPessoaNovo = atualizacoes[read_idx].idPessoaNovo;
                    break;
                case 1:
                    atualizacoes[write_idx].registro->idadePessoa = atualizacoes[read_idx].registro->idadePessoa;
                    break;
                case 2:
                    if (atualizacoes[write_idx].registro->nomePessoa)
                        free(atualizacoes[write_idx].registro->nomePessoa);
                    atualizacoes[write_idx].registro->nomePessoa = strdup(atualizacoes[read_idx].registro->nomePessoa);
                    atualizacoes[write_idx].registro->tamanhoNomePessoa = atualizacoes[read_idx].registro->tamanhoNomePessoa;
                    break;
                case 3:
                    if (atualizacoes[write_idx].registro->nomeUsuario)
                        free(atualizacoes[write_idx].registro->nomeUsuario);
                    atualizacoes[write_idx].registro->nomeUsuario = strdup(atualizacoes[read_idx].registro->nomeUsuario);
                    atualizacoes[write_idx].registro->tamanhoNomeUsuario = atualizacoes[read_idx].registro->tamanhoNomeUsuario;
                    break;
                }
                destroi_registro_pessoa(atualizacoes[read_idx].registro);
            }
        }
    }
    *nRegsEncontrados = write_idx + 1;

    // Calcula o tamanho final e define a flag para todos os registros unificados.
    for (int i = 0; i < *nRegsEncontrados; i++)
    {
        int new_compact_size = calcula_tamanho_registro_pessoa(atualizacoes[i].registro);

        if (new_compact_size > atualizacoes[i].tamanhoRegistroOriginal)
        {
            atualizacoes[i].flagNovoByteOffset = '1';
            atualizacoes[i].registro->tamanhoRegistro = new_compact_size;
        }
        else
        {
            atualizacoes[i].flagNovoByteOffset = '0';
            atualizacoes[i].registro->tamanhoRegistro = atualizacoes[i].tamanhoRegistroOriginal;
        }
    }
}

// Aplica as atualizações aos registros no arquivo de dados e ao índice em memória.
static int aplicar_atualizacoes_de_busca(FILE *fp, RegistroIndice **indice_em_memoria, Atualizacao *atualizacoes,
                                         int nRegsEncontrados, int nRemovidosInseridos, CabecalhoPessoa *cabPessoa)
{
    fseek(fp, 17, SEEK_SET);
    long long cursor_position = 17;

    RegistroBuscaPessoa **removidos_inseridos = malloc(nRemovidosInseridos * sizeof(RegistroBuscaPessoa *));
    int removidos_inseridos_idx = 0;

    for (int i = 0; i < nRegsEncontrados; i++)
    {

        RegistroPessoa *reg_atualizado = atualizacoes[i].registro;

        long long byteOffset = atualizacoes[i].ByteOffset;
        char flagNovoByteOffset = atualizacoes[i].flagNovoByteOffset;

        int tamanho_dados = reg_atualizado->tamanhoRegistro;

        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + tamanho_dados;
        long long diffByteOffset = byteOffset - cursor_position;

        fseek(fp, diffByteOffset, SEEK_CUR);

        // Atualiza o índice se o ID da pessoa foi modificado.
        if (atualizacoes[i].idPessoaNovo != -1)
        {

            int id_busca;

            id_busca = reg_atualizado->idPessoa;

            RegistroIndice **p_encontrado_ptr = bsearch(&id_busca,
                                                        indice_em_memoria,
                                                        cabPessoa->quantidadePessoas,
                                                        sizeof(RegistroIndice *),
                                                        comparar_bsearch_indice);

            if (p_encontrado_ptr == NULL)
            {
                free(indice_em_memoria);
                return -1;
            }

            if (atualizacoes[i].indiceDaRegra == 0)
            {
                (*p_encontrado_ptr)->idPessoa = atualizacoes[i].idPessoaNovo;
            }
            reg_atualizado->idPessoa = atualizacoes[i].idPessoaNovo;
        }

        // Se o registro couber no espaço antigo, faz a atualização no local (in-place).
        if (flagNovoByteOffset == '0')
        {
            int new_size = sizeof(int) * 4 + reg_atualizado->tamanhoNomePessoa + reg_atualizado->tamanhoNomeUsuario;

            escreve_registro_pessoa(fp, reg_atualizado);

            if (new_size < tamanho_dados)
            {
                long long padding_size = tamanho_dados - new_size;

                char *dollar = malloc(sizeof(char) * padding_size);
                memset(dollar, '$', padding_size);
                fwrite(dollar, sizeof(char), padding_size, fp);
                free(dollar);
            }
        }
        // Se não couber, o registro é marcado para ser removido e reinserido.
        else
        {
            removidos_inseridos[removidos_inseridos_idx] = malloc(sizeof(RegistroBuscaPessoa));
            removidos_inseridos[removidos_inseridos_idx]->registro = reg_atualizado;
            removidos_inseridos[removidos_inseridos_idx]->ByteOffset = byteOffset;

            cursor_position -= tamanho_real_escrito;
            removidos_inseridos_idx++;
        }

        cursor_position += diffByteOffset + tamanho_real_escrito;
    }
    fseek(fp, 17, SEEK_SET);

    remover_pessoas_e_indices(removidos_inseridos, indice_em_memoria, cabPessoa, nRemovidosInseridos, fp, 1);
    inserir_pessoas(fp, removidos_inseridos, removidos_inseridos_idx);

    // Libera array removidos_inseridos
    for (int i = 0; i < removidos_inseridos_idx; i++)
    {
        free(removidos_inseridos[i]);
    }
    free(removidos_inseridos);

    return 0;
}

// Implementa a funcionalidade 7: atualiza registros de pessoa no arquivo de dados e no índice.
int funcionalidade7(FILE *fp, FILE *fpIndice, const char *nomeArquivoIndice, int buscas)
{

    // Busca: Encontra todos os registros que correspondem aos critérios de busca e atualização.
    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 1);
    if (nRegsEncontrados == 0)
    {
        liberar_resultados_busca_inteira(resultadosEmBuscas, buscas);
        printf("Nenhum registro encontrado.\n");
        return 0;
    }

    // Preparação: Marca os arquivos como instáveis ('0').
    CabecalhoPessoa cabPessoa;
    fseek(fp, 0, SEEK_SET);
    le_cabecalho_pessoa(fp, &cabPessoa);
    toggle_cabecalho_pessoa(fp, &cabPessoa);

    // Criação de Tarefas: Converte os resultados da busca em uma lista de tarefas de atualização.
    Atualizacao *atualizacoes = malloc(sizeof(Atualizacao) * nRegsEncontrados);
    if (atualizacoes == NULL)
    {
        printf(FALHA_AO_ALOCAR);

        liberar_resultados_busca_inteira(resultadosEmBuscas, buscas);
        return -1;
    }

    // Cria objetos de atualização e preenche o array.
    int nAtualizacoes = processar_atualizacoes_de_busca(buscas, resultadosEmBuscas, atualizacoes);

    for (int i = 0; i < buscas; i++)
    {
        if (resultadosEmBuscas[i].registrosBusca != NULL)
        {
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {

                if (resultadosEmBuscas[i].registrosBusca[j] != NULL)
                {
                    free(resultadosEmBuscas[i].registrosBusca[j]);
                }
            }
            free(resultadosEmBuscas[i].registrosBusca);
        }
        if (resultadosEmBuscas[i].busca.campo != NULL)
            free(resultadosEmBuscas[i].busca.campo);
        if (resultadosEmBuscas[i].busca.valor != NULL)
            free(resultadosEmBuscas[i].busca.valor);
        if (resultadosEmBuscas[i].update.campo != NULL)
            free(resultadosEmBuscas[i].update.campo);
        if (resultadosEmBuscas[i].update.valor != NULL)
            free(resultadosEmBuscas[i].update.valor);
    }
    free(resultadosEmBuscas);

    // Unificação: Mescla múltiplas atualizações para o mesmo registro em uma única tarefa.
    qsort(atualizacoes, nAtualizacoes, sizeof(Atualizacao), comparar_atualizacao_por_id);

    unificarResultados(atualizacoes, &nAtualizacoes);

    // Execução: Aplica as tarefas de atualização ao arquivo de dados e ao índice em memória.
    RegistroIndice **indice_em_memoria = carregar_indice_inteiro(fpIndice, cabPessoa.quantidadePessoas);
    if (indice_em_memoria == NULL && cabPessoa.quantidadePessoas > 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(atualizacoes);
        return -1;
    }

    qsort(atualizacoes, nAtualizacoes, sizeof(Atualizacao), comparar_atualizacao_por_byteoffset);

    int nRemovidosInseridos = 0;
    for (int i = 0; i < nAtualizacoes; i++)
    {
        if (atualizacoes[i].flagNovoByteOffset != '0')
        {
            nRemovidosInseridos++;
        }
    }

    aplicar_atualizacoes_de_busca(fp, indice_em_memoria, atualizacoes, nAtualizacoes, nRemovidosInseridos, &cabPessoa);

    // Finalização do Índice: Reordena o índice em memória por ID e o reescreve no disco.
    qsort(indice_em_memoria, cabPessoa.quantidadePessoas, sizeof(RegistroIndice *), comparar_indices_id);

    reescrever_arquivo_indice(nomeArquivoIndice, indice_em_memoria, cabPessoa.quantidadePessoas);

    // Finalização do Arquivo de Dados: Atualiza o cabeçalho com as novas contagens e marca como estável ('1').
    fflush(fp);
    fseek(fp, 0, SEEK_SET);
    cabPessoa.quantidadeRemovidos += nRemovidosInseridos;
    toggle_cabecalho_pessoa(fp, &cabPessoa);

    for (int i = 0; i < nAtualizacoes; i++)
    {
        destroi_registro_pessoa(atualizacoes[i].registro);
    }
    free(atualizacoes);

    return 0;
}