#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f8.h"

// Função auxiliar para interpretar uma linha do CSV e preencher uma struct RegistroPessoa.
int parse_segue_csv_line(char *line, RegistroSegue *reg)
{
    // Remove quebras de linha (\n ou \r) do final da string para evitar erros de parsing.
    line[strcspn(line, "\r\n")] = 0;

    char *p = line;
    char *virgula;

    // 1. Lê o campo idPessoa.
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->idPessoaQueSegue = atoi(p);
    p = virgula + 1;


    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->idPessoaQueESeguida = atoi(p);
    p = virgula + 1;

    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    snprintf(reg->dataInicioQueSegue, 
             sizeof(reg->dataInicioQueSegue), 
             "%s", p);
    p = virgula + 1;

    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    snprintf(reg->dataFimQueSegue, 
             sizeof(reg->dataFimQueSegue), 
             "%s", p);
    p = virgula + 1;

    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->grauAmizade = p[0];
    p = virgula + 1;

    reg->removido = 0;

    return 0; // Retorna 0 em caso de sucesso.
}