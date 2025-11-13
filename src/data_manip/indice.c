#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pessoa.h"
#include "indice.h"

int le_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);

    if (fread(&cab->status, sizeof(char), 1, fp) < 1 || fread(&cab->lixo, sizeof(char), 11, fp) < 1)
    {
        return 1;
    }
    return 0;
}

void toggle_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);
    cab->status = (cab->status == '0') ? '1' : '0';
    escreve_cabecalho_indice(fp, cab);
}

int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&cab->status, sizeof(char), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->lixo, 1, 11, fp) < 1)
        return 1;
    return 0;
}

int le_registro_indice(FILE *fp, RegistroIndice **reg_out)
{
    RegistroIndice *reg = malloc(sizeof(RegistroIndice));
    if (reg == NULL)
        return 1;

    if (fread(&reg->idPessoa, sizeof(int), 1, fp) == 0)
    {
        free(reg);
        return 1;
    }
    if (fread(&reg->byteOffset, sizeof(long long), 1, fp) == 0)
    {
        free(reg);
        return 1;
    }

    *reg_out = reg;
    return 0;
}

// Realiza a intercalação (merge) de dois arrays de índice ordenados.
RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo)
{
    int n_total = n_antigo + n_novo;
    RegistroIndice **indice_final = malloc(n_total * sizeof(RegistroIndice *));
    if (indice_final == NULL)
        return NULL;

    int i = 0, j = 0, k = 0;

    while (i < n_antigo && j < n_novo)
    {
        if (indice_antigo[i]->idPessoa <= indice_novo[j]->idPessoa)
        {
            indice_final[k++] = indice_antigo[i++];
        }
        else
        {
            indice_final[k++] = indice_novo[j++];
        }
    }

    while (i < n_antigo)
        indice_final[k++] = indice_antigo[i++];
    while (j < n_novo)
        indice_final[k++] = indice_novo[j++];

    return indice_final;
}

void destroi_registro_indice(RegistroIndice *reg)
{
    if (reg != NULL)
        free(reg);
}

int comparar_bsearch_indice(const void *key, const void *elem)
{
    int id_busca = *(const int *)key;
    const RegistroIndice *reg = *(const RegistroIndice **)elem;
    if (id_busca < reg->idPessoa)
        return -1;
    if (id_busca > reg->idPessoa)
        return 1;
    return 0;
}

int comparar_indices_id(const void *a, const void *b)
{
    RegistroIndice *regA = *(RegistroIndice **)a;
    RegistroIndice *regB = *(RegistroIndice **)b;
    return regA->idPessoa - regB->idPessoa;
}

// Carrega todos os registros de um arquivo de índice para um array em memória.
RegistroIndice **carregar_indice_inteiro(FILE *fp, int numeroRegistros)
{
    if (fp == NULL || numeroRegistros <= 0)
        return NULL;

    CabecalhoIndice cab;
    if (le_cabecalho_indice(fp, &cab) != 0)
        return NULL;

    fseek(fp, 12, SEEK_SET);

    RegistroIndice **registros = malloc(numeroRegistros * sizeof(RegistroIndice *));
    if (registros == NULL)
        return NULL;

    for (int i = 0; i < numeroRegistros; i++)
    {
        if (le_registro_indice(fp, &registros[i]) != 0)
        {

            for (int j = 0; j < i; j++)
                free(registros[j]);
            free(registros);
            return NULL;
        }
    }
    return registros;
}

// Reescreve o arquivo de índice a partir de um array em memória,
// utilizando um arquivo temporário para garantir a atomicidade da operação.
void reescrever_arquivo_indice(const char *nomeArquivoIndice, RegistroIndice **indice_em_memoria, int numeroRegistros)
{
    char nomeArquivoTemp[] = "indice.tmp";
    FILE *fpTemp = fopen(nomeArquivoTemp, "wb");
    if (fpTemp == NULL)
    {

        for (int i = 0; i < numeroRegistros; i++)
            destroi_registro_indice(indice_em_memoria[i]);
        free(indice_em_memoria);
        return;
    }

    CabecalhoIndice index_header;
    index_header.status = '0';
    memset(index_header.lixo, '$', sizeof(index_header.lixo));
    escreve_cabecalho_indice(fpTemp, &index_header);

    fseek(fpTemp, 12, SEEK_SET);
    for (int i = 0; i < numeroRegistros; i++)
    {

        if (indice_em_memoria[i] != NULL && indice_em_memoria[i]->byteOffset > 0)
        {
            fwrite(&indice_em_memoria[i]->idPessoa, sizeof(int), 1, fpTemp);
            fwrite(&indice_em_memoria[i]->byteOffset, sizeof(long long), 1, fpTemp);
        }
        destroi_registro_indice(indice_em_memoria[i]);
    }
    free(indice_em_memoria);

    index_header.status = '1';
    escreve_cabecalho_indice(fpTemp, &index_header);
    fclose(fpTemp);

    remove(nomeArquivoIndice);
    rename(nomeArquivoTemp, nomeArquivoIndice);
}