#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pessoa.h"
#include "indice.h"

// Lê o registro de cabeçalho do arquivo de índice.
int le_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);
    // Lê os campos 'status' e 'lixo' para a struct.
    if (fread(&cab->status, sizeof(char), 1, fp) < 1 || fread(&cab->lixo, sizeof(char), 11, fp) < 1)
    {
        return 1; // Retorna erro em caso de falha.
    }
    return 0;
}

// Alterna o status do cabeçalho ('0' -> '1' ou '1' -> '0') e o reescreve no arquivo.
void toggle_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);
    cab->status = (cab->status == '0') ? '1' : '0';
    escreve_cabecalho_indice(fp, cab);
}

// Escreve o cabeçalho no arquivo de índice.
int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&cab->status, sizeof(char), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->lixo, 1, 11, fp) < 1)
        return 1;
    return 0;
}

// Lê um único registro de índice da posição atual do arquivo.
int le_registro_indice(FILE *fp, RegistroIndice **reg_out)
{
    RegistroIndice *reg = malloc(sizeof(RegistroIndice));
    if (reg == NULL)
        return 1;

    // Tenta ler os campos do registro. Se a leitura de 'idPessoa' falhar,
    // assume-se fim de arquivo.
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

// Combina dois arrays de registros de índice (antigo e novo) em um único array ordenado.
RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo)
{
    int n_total = n_antigo + n_novo;
    RegistroIndice **indice_final = malloc(n_total * sizeof(RegistroIndice *));
    if (indice_final == NULL) return NULL;

    int i = 0, j = 0, k = 0;
    // Intercala os dois arrays enquanto ambos têm elementos.
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
    // Adiciona os elementos restantes de cada array.
    while (i < n_antigo) indice_final[k++] = indice_antigo[i++];
    while (j < n_novo) indice_final[k++] = indice_novo[j++];

    return indice_final;
}

// Libera a memória alocada para um registro de índice.
void destroi_registro_indice(RegistroIndice *reg)
{
    if (reg != NULL) free(reg);
}

// Função de comparação para bsearch, para encontrar um registro de índice por idPessoa.
int comparar_bsearch_indice(const void *key, const void *elem)
{
    int id_busca = *(const int *)key;
    const RegistroIndice *reg = *(const RegistroIndice **)elem;
    if (id_busca < reg->idPessoa) return -1;
    if (id_busca > reg->idPessoa) return 1;
    return 0;
}

// Função de comparação para qsort, para ordenar um array de registros de índice por idPessoa.
int comparar_indices_id(const void *a, const void *b)
{
    RegistroIndice *regA = *(RegistroIndice **)a;
    RegistroIndice *regB = *(RegistroIndice **)b;
    return regA->idPessoa - regB->idPessoa;
}

// Carrega todos os registros do arquivo de índice para um array em memória.
RegistroIndice **carregar_indice_inteiro(FILE *fp, int numeroRegistros)
{
    if (fp == NULL || numeroRegistros <= 0) return NULL;

    CabecalhoIndice cab;
    if (le_cabecalho_indice(fp, &cab) != 0) return NULL;

    fseek(fp, 12, SEEK_SET); // Pula o cabeçalho.

    RegistroIndice **registros = malloc(numeroRegistros * sizeof(RegistroIndice *));
    if (registros == NULL) return NULL;

    // Lê cada registro do arquivo para o array.
    for (int i = 0; i < numeroRegistros; i++)
    {
        if (le_registro_indice(fp, &registros[i]) != 0)
        {
            // Em caso de erro, libera toda a memória alocada até o momento.
            for (int j = 0; j < i; j++) free(registros[j]);
            free(registros);
            return NULL;
        }
    }
    return registros;
}

// Reescreve completamente o arquivo de índice a partir de um array de registros em memória.
// Usa uma estratégia de arquivo temporário para garantir a atomicidade e portabilidade da operação.
void reescrever_arquivo_indice(const char *nomeArquivoIndice, RegistroIndice **indice_em_memoria, int numeroRegistros)
{
    char nomeArquivoTemp[] = "indice.tmp";
    FILE *fpTemp = fopen(nomeArquivoTemp, "wb");
    if (fpTemp == NULL) {
        // Libera a memória se não for possível criar o arquivo temporário.
        for (int i = 0; i < numeroRegistros; i++) destroi_registro_indice(indice_em_memoria[i]);
        free(indice_em_memoria);
        return;
    }

    // Escreve o cabeçalho no arquivo temporário, iniciando com status '0' (instável).
    CabecalhoIndice index_header;
    index_header.status = '0';
    memset(index_header.lixo, '$', sizeof(index_header.lixo));
    escreve_cabecalho_indice(fpTemp, &index_header);

    // Escreve os registros do array em memória no arquivo temporário.
    fseek(fpTemp, 12, SEEK_SET);
    for (int i = 0; i < numeroRegistros; i++)
    {
        // Apenas registros válidos (com byteOffset positivo) são escritos.
        if (indice_em_memoria[i] != NULL && indice_em_memoria[i]->byteOffset > 0)
        {
            fwrite(&indice_em_memoria[i]->idPessoa, sizeof(int), 1, fpTemp);
            fwrite(&indice_em_memoria[i]->byteOffset, sizeof(long long), 1, fpTemp);
        }
        destroi_registro_indice(indice_em_memoria[i]);
    }
    free(indice_em_memoria);

    // Atualiza o status do cabeçalho para '1' (estável) no arquivo temporário.
    index_header.status = '1';
    escreve_cabecalho_indice(fpTemp, &index_header);
    fclose(fpTemp);

    // Substitui o arquivo de índice original pelo arquivo temporário.
    remove(nomeArquivoIndice);
    rename(nomeArquivoTemp, nomeArquivoIndice);
}