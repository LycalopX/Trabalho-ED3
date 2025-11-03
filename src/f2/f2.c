#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f2.h"

// Função de comparação, necessária para o qsort ordenar os índices por idPessoa.
int comparar_indices(const void *a, const void *b)
{
    RegistroIndice *regA = (RegistroIndice *)a;
    RegistroIndice *regB = (RegistroIndice *)b;

    return (regA->idPessoa - regB->idPessoa);
}

// Função auxiliar para interpretar uma linha do CSV e preencher uma struct RegistroPessoa.
int parse_pessoa_csv_line(char *line, RegistroPessoa *reg)
{
    // Remove quebras de linha (\n ou \r) do final da string para evitar erros de parsing.
    line[strcspn(line, "\r\n")] = 0;

    char *p = line;
    char *virgula;

    // 1. Lê o campo idPessoa.
    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1; // Retorna erro se a linha for mal formatada.

    *virgula = '\0'; // Substitui a vírgula por um terminador nulo para isolar o campo.
    reg->idPessoa = atoi(p);
    p = virgula + 1; // Avança o ponteiro para o início do próximo campo.

    // 2. Lê o campo nomePessoa.
    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;
    if (p == virgula) // Verifica se o campo está vazio (ex: ",,").
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

    // 3. Lê o campo idadePessoa.
    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;
    if (p == virgula) // Se o campo for nulo, define a idade como -1.
    {
        reg->idadePessoa = -1;
    }
    else
    {
        *virgula = '\0';
        reg->idadePessoa = atoi(p);
    }
    p = virgula + 1;

    // 4. Lê o campo nomeUsuario (último campo da linha).
    if (*p == '\0') // Verifica se o campo está vazio.
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

    return 0; // Retorna 0 em caso de sucesso.
}

void funcionalidade2(FILE *fp_csv, FILE *fp_data, FILE *fp_index, const char *nomeArquivoDados, const char *nomeArquivoIndice)
{
    // Declara e inicializa os cabeçalhos.
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
    fgets(buffer, sizeof(buffer), fp_csv); // Pula a primeira linha (cabeçalho) do ficheiro CSV.

    // Posiciona o cursor no final do cabeçalho para garantir o início da escrita de dados.
    fseek(fp_data, 17, SEEK_SET);

    size_t regs_length = 100;
    RegistroIndice *regs_index = malloc(regs_length * sizeof(RegistroIndice));
    int index = 0;
    long long byteOffset = ftell(fp_data);

    // Itera sobre cada linha de dados do ficheiro CSV.
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
