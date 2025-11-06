#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "indice.h"

// FUNÇÕES STRUCT INDICE

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
    return;
}

// Escreve o cabeçalho no arquivo de dados. Retorna 1 em caso de erro.
int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);

    if (fwrite(&cab->status, sizeof(char), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->lixo, 1, 11, fp) < 1)
        return 1;

    return 0;
}

// Lê um registro de dados do arquivo. Retorna 1 em caso de erro ou fim de arquivo.
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
        // Se o ficheiro acabar inesperadamente aqui (muito raro, mas possível)
        free(reg);
        return 1;
    }

    *reg_out = reg;
    return 0;
}

RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo)
{
    int n_total = n_antigo + n_novo;
    RegistroIndice **indice_final = malloc(n_total * sizeof(RegistroIndice *));
    if (indice_final == NULL)
    {
        return NULL;
    }

    int i = 0;
    int j = 0;
    int k = 0;

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
    {
        indice_final[k++] = indice_antigo[i++];
    }
    while (j < n_novo)
    {
        indice_final[k++] = indice_novo[j++];
    }

    return indice_final;
}

// Libera a memória alocada para um registro de índice.
void destroi_registro_indice(RegistroIndice *reg)
{
    if (reg == NULL)
        return;

    free(reg);
}

// Função de comparação para bsearch, para encontrar um registro de índice por idPessoa.
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

// Carrega todo o índice na memória, retornando um array dinâmico de ponteiros para os registros.
RegistroIndice **carregar_indice_inteiro(FILE *fp, int numeroRegistros)
{
    if (fp == NULL)
    {
        return NULL;
    }

    CabecalhoIndice cab;
    if (le_cabecalho_indice(fp, &cab) != 0)
    {
        return NULL;
    }

    // Move o cursor para o início dos registros
    // fseek(fp, 12, SEEK_SET);

    // Aloca um array dinâmico para armazenar os ponteiros dos registros
    RegistroIndice **registros = malloc(numeroRegistros * sizeof(RegistroIndice *));

    for (int i = 0; i < numeroRegistros; i++)
    {
        if (le_registro_indice(fp, &registros[i]) != 0)
        {
            // Em caso de erro, libera a memória alocada até o momento e retorna NULL
            for (int j = 0; j < i; j++)
            {
                free(registros[j]);
            }
            free(registros);
            return NULL;
        }
    }

    return registros;
}