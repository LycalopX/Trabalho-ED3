#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"
#include "f2.h"

// Função de comparação para qsort, utilizada para ordenar registros de índice por idPessoa.
int comparar_indices(const void *a, const void *b)
{
    RegistroIndice *regA = (RegistroIndice *)a;
    RegistroIndice *regB = (RegistroIndice *)b;

    return (regA->idPessoa - regB->idPessoa);
}

// Interpreta uma linha do CSV e preenche uma struct RegistroPessoa.
int parse_pessoa_csv_line(char *line, RegistroPessoa *reg)
{

    line[strcspn(line, "\r\n")] = 0;

    char *p = line;
    char *virgula;

    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->idPessoa = atoi(p);
    p = virgula + 1;

    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;
    if (p == virgula)
    {
        reg->tamanhoNomePessoa = 0;
        reg->nomePessoa = NULL;
    }
    else
    {
        *virgula = '\0';
        reg->tamanhoNomePessoa = strlen(p);
        reg->nomePessoa = malloc(reg->tamanhoNomePessoa);
        memcpy(reg->nomePessoa, p, reg->tamanhoNomePessoa);
    }
    p = virgula + 1;

    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;
    if (p == virgula)
    {
        reg->idadePessoa = -1;
    }
    else
    {
        *virgula = '\0';
        reg->idadePessoa = atoi(p);
    }
    p = virgula + 1;

    if (*p == '\0')
    {
        reg->tamanhoNomeUsuario = 0;
        reg->nomeUsuario = NULL;
    }
    else
    {
        reg->tamanhoNomeUsuario = strlen(p);
        reg->nomeUsuario = malloc(reg->tamanhoNomeUsuario);
        memcpy(reg->nomeUsuario, p, reg->tamanhoNomeUsuario);
    }

    return 0;
}

// Implementa a funcionalidade 2: lê um arquivo CSV, cria um arquivo de dados binário
// e um arquivo de índice, preenchendo-os com os dados do CSV.
void funcionalidade2(FILE *fp_csv, FILE *fp_data, FILE *fp_index, const char *nomeArquivoDados, const char *nomeArquivoIndice)
{

    CabecalhoPessoa data_header;
    CabecalhoIndice index_header;

    data_header.status = '0';
    data_header.quantidadePessoas = 0;
    data_header.quantidadeRemovidos = 0;
    data_header.proxByteOffset = 17;

    le_cabecalho_indice(fp_index, &index_header);
    index_header.status = '0';

    escreve_cabecalho_pessoa(fp_data, &data_header);
    escreve_cabecalho_indice(fp_index, &index_header);

    char buffer[256];
    fgets(buffer, sizeof(buffer), fp_csv);

    fseek(fp_data, 17, SEEK_SET);

    size_t regs_length = 100;
    RegistroIndice *regs_index = malloc(regs_length * sizeof(RegistroIndice));
    int index = 0;
    long long byteOffset = ftell(fp_data);

    while (fgets(buffer, sizeof(buffer), fp_csv))
    {
        if (regs_index == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            return;
        }

        RegistroPessoa reg_pessoa;
        reg_pessoa.nomePessoa = NULL;
        reg_pessoa.nomeUsuario = NULL;

        if (parse_pessoa_csv_line(buffer, &reg_pessoa) != 0)
        {
            continue;
        }

        reg_pessoa.removido = '0';

        int tamanho_total_em_disco = sizeof(reg_pessoa.removido) + sizeof(reg_pessoa.tamanhoRegistro) + sizeof(reg_pessoa.idPessoa) + sizeof(reg_pessoa.idadePessoa) + sizeof(reg_pessoa.tamanhoNomePessoa) + sizeof(reg_pessoa.tamanhoNomeUsuario) + reg_pessoa.tamanhoNomePessoa + reg_pessoa.tamanhoNomeUsuario;
        reg_pessoa.tamanhoRegistro = tamanho_total_em_disco - (sizeof(reg_pessoa.removido) + sizeof(reg_pessoa.tamanhoRegistro));

        if (index >= regs_length)
        {
            realloc_golden((void **)&regs_index, &regs_length, sizeof(RegistroIndice));
        }

        regs_index[index].idPessoa = reg_pessoa.idPessoa;
        regs_index[index].byteOffset = byteOffset;

        byteOffset += tamanho_total_em_disco;

        escreve_registro_pessoa(fp_data, &reg_pessoa);

        free(reg_pessoa.nomePessoa);
        free(reg_pessoa.nomeUsuario);

        index++;
    }

    data_header.quantidadePessoas = index;
    data_header.proxByteOffset = byteOffset;

    qsort(regs_index, index, sizeof(RegistroIndice), comparar_indices);

    fseek(fp_index, 12, SEEK_SET);
    for (int i = 0; i < data_header.quantidadePessoas; i++)
    {
        fwrite(&regs_index[i].idPessoa, sizeof(int), 1, fp_index);
        fwrite(&regs_index[i].byteOffset, sizeof(long long), 1, fp_index);
    }

    data_header.status = '1';
    index_header.status = '1';

    escreve_cabecalho_pessoa(fp_data, &data_header);
    escreve_cabecalho_indice(fp_index, &index_header);

    fflush(fp_data);
    fflush(fp_index);

    free(regs_index);
}
