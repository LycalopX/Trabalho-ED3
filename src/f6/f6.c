#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"
#include "f6.h"

// Função de comparação para qsort, utilizada para ordenar registros por byte offset.
static int compara_inseridos_busca(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;

    if (regA->ByteOffset < regB->ByteOffset)
        return -1;
    if (regA->ByteOffset > regB->ByteOffset)
        return 1;
    return 0;
}

// Analisa uma linha de entrada para inserção, extraindo os campos de um novo registro.
static int parse_insercao_linha(char *line, int *numero_busca, int *idPessoa, char *nomePessoa, char *idadeString, char *nomeUsuario)
{
    line[strcspn(line, "\r\n")] = 0;

    char *token;

    token = strtok(line, " ");
    if (token == NULL)
        return 0;
    *numero_busca = atoi(token);

    token = strtok(NULL, ",");
    if (token == NULL)
        return 0;
    *idPessoa = atoi(token);

    token = strtok(NULL, ",");
    if (token == NULL)
        return 0;
    while (*token == ' ' || *token == '"')
        token++;
    char *end = token + strlen(token) - 1;
    while (end > token && *end == '"')
        *end-- = '\0';
    strcpy(nomePessoa, token);

    token = strtok(NULL, ",");
    if (token == NULL)
        return 0;
    while (*token == ' ')
        token++;
    strcpy(idadeString, token);

    token = strtok(NULL, "\n");
    if (token == NULL)
        return 0;
    while (*token == ' ' || *token == '"')
        token++;
    end = token + strlen(token) - 1;
    while (end > token && *end == '"')
        *end-- = '\0';
    strcpy(nomeUsuario, token);

    return 5;
}

// Implementa a funcionalidade 6: insere novos registros de pessoa no arquivo de dados
// e atualiza o arquivo de índice.
void funcionalidade6(FILE *fp, FILE *fpIndice, const char *nomeArquivoIndice, int insercoes)
{
    // Prepara os arquivos para escrita, marcando o status como instável ('0').
    CabecalhoPessoa *cp = malloc(sizeof(CabecalhoPessoa));
    if (cp == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }
    le_cabecalho_pessoa(fp, cp);
    toggle_cabecalho_pessoa(fp, cp);

    CabecalhoIndice index_header_temp;
    le_cabecalho_indice(fpIndice, &index_header_temp);
    toggle_cabecalho_indice(fpIndice, &index_header_temp);

    // Lê e processa cada registro a ser inserido da entrada padrão.
    RegistroBuscaPessoa **registrosInseridos = malloc(insercoes * sizeof(RegistroBuscaPessoa *));
    if (registrosInseridos == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cp);
        return;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    for (int i = 0; i < insercoes; i++)
    {
        int numero_busca, idPessoa, idade;
        char nomePessoa[200], idadeString[20], nomeUsuario[200];
        char input_line[500];

        fgets(input_line, sizeof(input_line), stdin);

        if (parse_insercao_linha(input_line, &numero_busca, &idPessoa, nomePessoa, idadeString, nomeUsuario) < 5)
        {
            continue;
        }

        if (strcmp(idadeString, "NULO") == 0)
        {
            idade = -1;
        }
        else
        {
            idade = atoi(idadeString);
        }

        RegistroPessoa *novo_reg = cria_registro_pessoa(idPessoa, nomePessoa, idade, nomeUsuario);
        if (novo_reg == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            free(cp);
            free(registrosInseridos);
            return;
        }

        long long offset_insercao = cp->proxByteOffset;
        cp->proxByteOffset += sizeof(char) + sizeof(int) + novo_reg->tamanhoRegistro;

        registrosInseridos[i] = malloc(sizeof(RegistroBuscaPessoa));
        registrosInseridos[i]->registro = novo_reg;
        registrosInseridos[i]->ByteOffset = offset_insercao;
    }

    // Insere os novos registros no arquivo de dados, ordenando-os por byte offset.
    qsort(registrosInseridos, insercoes, sizeof(RegistroBuscaPessoa *), compara_inseridos_busca);
    inserir_pessoas(fp, registrosInseridos, insercoes);

    // Prepara as novas entradas para o arquivo de índice.
    RegistroIndice **indices_novos = malloc(insercoes * sizeof(RegistroIndice *));
    for (int i = 0; i < insercoes; i++)
    {
        indices_novos[i] = malloc(sizeof(RegistroIndice));
        indices_novos[i]->idPessoa = registrosInseridos[i]->registro->idPessoa;
        indices_novos[i]->byteOffset = registrosInseridos[i]->ByteOffset;
        destroi_registro_pessoa(registrosInseridos[i]->registro);
        free(registrosInseridos[i]);
    }
    free(registrosInseridos);

    // Atualiza o arquivo de índice, mesclando o índice antigo com os novos registros.
    qsort(indices_novos, insercoes, sizeof(RegistroIndice *), comparar_indices_id);

    fseek(fpIndice, 0, SEEK_SET);
    RegistroIndice **indices_antigos = carregar_indice_inteiro(fpIndice, cp->quantidadePessoas);

    RegistroIndice **indice_final = intercalar_indices(indices_antigos, cp->quantidadePessoas, indices_novos, insercoes);

    if (indices_antigos)
        free(indices_antigos);
    if (indices_novos)
        free(indices_novos);

    reescrever_arquivo_indice(nomeArquivoIndice, indice_final, cp->quantidadePessoas + insercoes);

    // Atualiza o cabeçalho do arquivo de dados e finaliza.
    fflush(fp);
    fseek(fp, 0, SEEK_SET);
    cp->status = '1';
    cp->quantidadePessoas += insercoes;
    escreve_cabecalho_pessoa(fp, cp);
    fflush(fp);

    free(cp);
}
