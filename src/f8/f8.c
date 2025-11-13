#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/segue.h"

// Analisa uma linha do CSV e preenche uma struct RegistroSegue.
int parse_segue_csv_line(char *line, RegistroSegue *reg)
{

    line[strcspn(line, "\r\n")] = '\0';

    char *p = line;
    char *virgula;

    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    if (*p == '\0')
    {

        reg->idPessoaQueSegue = NULO_INTEIRO;
    }
    else
    {
        reg->idPessoaQueSegue = atoi(p);
    }

    p = virgula + 1;
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 2;

    *virgula = '\0';
    if (*p == '\0')
    {

        reg->idPessoaQueESeguida = NULO_INTEIRO;
    }
    else
    {
        reg->idPessoaQueESeguida = atoi(p);
    }

    p = virgula + 1;
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 3;

    *virgula = '\0';
    snprintf(reg->dataInicioQueSegue, sizeof(reg->dataInicioQueSegue), "%s", p);

    if (reg->dataInicioQueSegue[0] == '\0')
    {

        strcpy(reg->dataInicioQueSegue, NULO_DATA);
    }

    p = virgula + 1;
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 4;

    *virgula = '\0';
    snprintf(reg->dataFimQueSegue, sizeof(reg->dataFimQueSegue), "%s", p);
    if (reg->dataFimQueSegue[0] == '\0')
    {

        strcpy(reg->dataFimQueSegue, NULO_DATA);
    }

    p = virgula + 1;
    if (p == NULL || *p == '\0')
    {
        reg->grauAmizade = NULO_CARACTERE;
    }
    else
    {
        reg->grauAmizade = p[0];
    }

    reg->removido = REGISTRO_SEGUE_NAO_REMOVIDO;

    return 0;
}

// Implementa a funcionalidade 8: converte um arquivo CSV de relacionamentos 'segue'
// em um arquivo binário, escrevendo o cabeçalho e os registros.
int funcionalidade8(FILE *fp_csv, FILE *fp_bin)
{

    CabecalhoSegue *cab = malloc(sizeof(CabecalhoSegue));
    cab->status = '0';
    cab->quantidadePessoas = 0;
    cab->proxRRN = 0;
    escreve_cabecalho_segue(fp_bin, cab);

    char linha[256];

    fgets(linha, sizeof(linha), fp_csv);

    RegistroSegue *reg = malloc(sizeof(RegistroSegue));
    while (fgets(linha, sizeof(linha), fp_csv) != NULL)
    {
        int parse_result = parse_segue_csv_line(linha, reg);
        if (parse_result != 0)
        {
            free(reg);
            return 3;
        }

        if (escreve_registro_segue(fp_bin, reg) != 0)
        {
            free(reg);
            return 4;
        }

        cab->quantidadePessoas++;
        cab->proxRRN++;
    }

    fseek(fp_bin, 0, SEEK_SET);
    cab->status = '1';

    if (escreve_cabecalho_segue(fp_bin, cab) != 0)
    {
        free(reg);
        return 5;
    }

    free(reg);
    free(cab);
    return 0;
}
