#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para ftruncate

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"

#include "../f4/f4.h"
#include "atualizacao.h"

// Atualizar campos como ID, idade
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

// Atualizar campos como nome, usuario
// flag: lembrar que atualizacao precisa receber o id da operação
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
        atualizacao->ByteOffset = -1; // Mark to be ignored
        return 0;
    }

    // This is the size of the actual record on disk.
    int old_record_disk_size = atualizacao->registro->tamanhoRegistro;

    // Update the in-memory record first to calculate the new size
    if (*campo_registro_atualizacao != NULL)
    {
        free(*campo_registro_atualizacao);
    }
    *campo_registro_atualizacao = strdup(new_string);
    *tamanho_campo = strlen(new_string);

    // Calculate the size the new record *would* need if written compactly.
    int new_compact_size = calcula_tamanho_registro_pessoa(atualizacao->registro);

    if (new_compact_size > old_record_disk_size)
    {
        atualizacao->flagNovoByteOffset = '1';
        atualizacao->registro->tamanhoRegistro = new_compact_size;
    }
    else
    {
        atualizacao->registro->tamanhoRegistro = old_record_disk_size;
    }

    return 0;
}

// Processa as atualizações para cada busca, preenchendo o array de tarefas
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

        int j = 0;
        int nRegistros = resultadosEmBuscas[i].nRegistros;

        // Indice da regra = 0
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
            free(resultadosEmBuscas[i].registrosBusca);
            nAtualizacoes += j;
        }
        // Indice da regra = 1
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
            free(resultadosEmBuscas[i].registrosBusca);
            nAtualizacoes += j;
        }
        // Indice da regra = 2
        else if (strcmp(u->campo, CAMPO_NOME) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexAtualizacao = j + nAtualizacoes;
                inicializa_atualizacao(&atualizacoes[indexAtualizacao], registrosBusca[j]->registro, 
                    registrosBusca[j]->ByteOffset, 2);
                atualizar_campo_string(&atualizacoes[indexAtualizacao], u, registrosBusca[j]->registro->nomePessoa, 
                    &atualizacoes[indexAtualizacao].registro->nomePessoa, &registrosBusca[j]->registro->tamanhoNomePessoa);
            }
            free(resultadosEmBuscas[i].registrosBusca);
            nAtualizacoes += j;
        }
        // Indice da regra = 3
        else if (strcmp(u->campo, CAMPO_USUARIO) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexAtualizacao = j + nAtualizacoes;
                inicializa_atualizacao(&atualizacoes[indexAtualizacao], registrosBusca[j]->registro, 
                    registrosBusca[j]->ByteOffset, 3);
                atualizar_campo_string(&atualizacoes[indexAtualizacao], u, registrosBusca[j]->registro->nomeUsuario, 
                    &atualizacoes[indexAtualizacao].registro->nomeUsuario, &registrosBusca[j]->registro->tamanhoNomeUsuario);
            }
            free(resultadosEmBuscas[i].registrosBusca);
            nAtualizacoes += j;
        }
    }
    free(resultadosEmBuscas);

    return nAtualizacoes;
}

// Apenas vamos manipular atualizacoes, como a única array importante
static void unificarResultados(Atualizacao *atualizacoes, int *nRegsEncontrados)
{
    if (*nRegsEncontrados <= 1)
    {
        return;
    }

    int write_idx = 0;
    for (int read_idx = 1; read_idx < (*nRegsEncontrados); read_idx++)
    {
        if (atualizacoes[read_idx].ByteOffset == -1)
        {
            destroi_registro_pessoa(atualizacoes[read_idx].registro);
            continue;
        }

        if (atualizacoes[read_idx].registro->idPessoa != atualizacoes[write_idx].registro->idPessoa)
        {
            write_idx++;
            atualizacoes[write_idx] = atualizacoes[read_idx];
        }
        else
        {
            switch (atualizacoes[read_idx].indiceDaRegra)
            {
            case 0: // ID
                atualizacoes[write_idx].registro->idPessoa = atualizacoes[read_idx].registro->idPessoa;
                atualizacoes[write_idx].idPessoaNovo = atualizacoes[read_idx].registro->idPessoa;
                break;
            case 1: // Idade
                atualizacoes[write_idx].registro->idadePessoa = atualizacoes[read_idx].registro->idadePessoa;
                break;
            case 2: // Nome
                free(atualizacoes[write_idx].registro->nomePessoa);
                atualizacoes[write_idx].registro->nomePessoa = strdup(atualizacoes[read_idx].registro->nomePessoa);
                atualizacoes[write_idx].registro->tamanhoNomePessoa = atualizacoes[read_idx].registro->tamanhoNomePessoa;
                break;
            case 3: // Usuario
                free(atualizacoes[write_idx].registro->nomeUsuario);
                atualizacoes[write_idx].registro->nomeUsuario = strdup(atualizacoes[read_idx].registro->nomeUsuario);
                atualizacoes[write_idx].registro->tamanhoNomeUsuario = atualizacoes[read_idx].registro->tamanhoNomeUsuario;
                break;
            }

            if (atualizacoes[read_idx].flagNovoByteOffset == '1' || atualizacoes[write_idx].flagNovoByteOffset == '1')
            {
                atualizacoes[write_idx].flagNovoByteOffset = '1';
                atualizacoes[write_idx].registro->tamanhoRegistro = calcula_tamanho_registro_pessoa(atualizacoes[write_idx].registro);
            }

            destroi_registro_pessoa(atualizacoes[read_idx].registro);
        }
    }
    *nRegsEncontrados = write_idx + 1;
}

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

        // Arquivo indice
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
                (*p_encontrado_ptr)->idPessoa = reg_atualizado->idPessoa;
            }

            reg_atualizado->idPessoa = atualizacoes[i].idPessoaNovo;
        }

        // Arquivo pessoa
        if (flagNovoByteOffset == '0')
        {
            int new_size = sizeof(int) * 4 + reg_atualizado->tamanhoNomePessoa + reg_atualizado->tamanhoNomeUsuario;

            escreve_registro_pessoa(fp, reg_atualizado);

            if (new_size < tamanho_dados)
            {
                long long padding_size = tamanho_dados - new_size;

                // Adiciona padding com '$'
                char *dollar = malloc(sizeof(char) * padding_size);
                memset(dollar, '$', padding_size);
                fwrite(dollar, sizeof(char), padding_size, fp);
                free(dollar);
            }
        }
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

    return 0;
}

int funcionalidade7(FILE *fp, FILE *fpIndice, int buscas)
{
    imprimir_registros_raw(fp);
    imprimir_registros_raw_em_arquivo(fp, "./debug/input_7.txt");

    int nRegsEncontrados = 0;
    // A função f4 agora é a única responsável por ler os dados da busca.
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 0, 1);
    if (nRegsEncontrados == 0)
    {
        if (resultadosEmBuscas)
            free(resultadosEmBuscas);
        printf("Nenhum registro encontrado.\n");
        return 0;
    }

    CabecalhoPessoa cabPessoa;

    fseek(fp, 0, SEEK_SET);
    le_cabecalho_pessoa(fp, &cabPessoa);
    toggle_cabecalho_pessoa(fp, &cabPessoa);

    // Aloca um array plano de Atualizacao.
    Atualizacao *atualizacoes = malloc(sizeof(Atualizacao) * nRegsEncontrados);
    if (atualizacoes == NULL)
    {
        printf(FALHA_AO_ALOCAR);
        // A memória de resultadosEmBuscas precisa ser liberada aqui.
        free(resultadosEmBuscas);
        return -1;
    }

    // Cria objetos de atualização e preenche o array.
    int nAtualizacoes = processar_atualizacoes_de_busca(buscas, resultadosEmBuscas, atualizacoes);

    // Ordena por ID para fundir registros duplicados.
    qsort(atualizacoes, nAtualizacoes, sizeof(Atualizacao), comparar_atualizacao_por_id);
    unificarResultados(atualizacoes, &nAtualizacoes);

    RegistroIndice **indice_em_memoria = carregar_indice_inteiro(fpIndice, cabPessoa.quantidadePessoas);
    if (indice_em_memoria == NULL && cabPessoa.quantidadePessoas > 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(atualizacoes);
        return -1;
    }

    // Ordena por byte offset para escrever no arquivo de dados de forma sequencial.
    qsort(atualizacoes, nAtualizacoes, sizeof(Atualizacao), comparar_atualizacao_por_byteoffset);

    // Contar número de removidos e inseridos
    int nRemovidosInseridos = 0;
    for (int i = 0; i < nAtualizacoes; i++)
    {
        if (atualizacoes[i].flagNovoByteOffset != '0')
        {
            nRemovidosInseridos++;
        }
    }

    // Nova função que escreve as alterações no arquivo.
    aplicar_atualizacoes_de_busca(fp, indice_em_memoria, atualizacoes, nAtualizacoes, nRemovidosInseridos, &cabPessoa);

    // Reordena o índice por ID para reescrevê-lo corretamente.
    qsort(indice_em_memoria, cabPessoa.quantidadePessoas, sizeof(RegistroIndice *), comparar_indices_id);
    reescrever_arquivo_indice(fpIndice, indice_em_memoria, cabPessoa.quantidadePessoas);

    // Atualiza o cabeçalho do arquivo de dados.
    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cabPessoa.quantidadeRemovidos += nRemovidosInseridos;
    toggle_cabecalho_pessoa(fp, &cabPessoa);

    // Liberar memória alocada.
    for (int i = 0; i < nAtualizacoes; i++)
    {
        destroi_registro_pessoa(atualizacoes[i].registro);
    }
    free(atualizacoes);

    imprimir_registros_raw_em_arquivo(fp, "./debug/output_f7.txt");
    return 0;
}