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
    line[strcspn(line, "\r\n")] = '\0';

    char *p = line;
    char *virgula;

    // Lê o campo idPessoaQueSegue.
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->idPessoaQueSegue = atoi(p);
    p = virgula + 1;

    // Lê o campo idPessoaQueESeguida.
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->idPessoaQueESeguida = atoi(p);
    p = virgula + 1;

    // Lê os campos dataInicioQueSegue
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    snprintf(reg->dataInicioQueSegue, 
             sizeof(reg->dataInicioQueSegue), 
             "%s", p);
    p = virgula + 1;

    // Lê o campo dataFimQueSegue
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    snprintf(reg->dataFimQueSegue, 
             sizeof(reg->dataFimQueSegue), 
             "%s", p);
    p = virgula + 1;

    // Lê o campo grauAmizade
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 1;

    *virgula = '\0';
    reg->grauAmizade = p[0];
    p = virgula + 1;

    reg->removido = 0;

    return 0; // Retorna 0 em caso de sucesso.
}

int escreve_registro_segue(FILE *fp, RegistroSegue *reg)
{
    fwrite(&reg->removido, sizeof(int), 1, fp);
    fwrite(&reg->idPessoaQueSegue, sizeof(int), 1, fp);
    fwrite(&reg->idPessoaQueESeguida, sizeof(int), 1, fp);
    fwrite(&reg->dataInicioQueSegue, sizeof(char), 10, fp);
    fwrite(&reg->dataFimQueSegue, sizeof(char), 10, fp);
    fwrite(&reg->grauAmizade, sizeof(char), 1, fp);

    return 0; // Sucesso.
}

// Transforma um arquivo CSV de seguidores em um arquivo binário.
int funcionalidade8(FILE *fp_csv, FILE *fp_bin)
{
    // Escreve o cabeçalho inicial do arquivo binário (será sobrescrito no final).
    char status = '0';
    int quantidadePessoas = 0;
    int proxRRN = 0;

    fwrite(&status, sizeof(char), 1, fp_bin);
    fwrite(&quantidadePessoas, sizeof(int), 1, fp_bin);
    fwrite(&proxRRN, sizeof(int), 1, fp_bin);

    char linha[256];
    // Ignora a primeira linha do CSV (cabeçalho).
    fgets(linha, sizeof(linha), fp_csv);

    // Lê cada linha do CSV e converte para RegistroSegue.
    while (fgets(linha, sizeof(linha), fp_csv) != NULL)
    {
        RegistroSegue reg;
        if (parse_segue_csv_line(linha, &reg) != 0)
        {
            fclose(fp_csv);
            fclose(fp_bin);
            return 3; // Erro ao parsear uma linha do CSV.
        }

        // Escreve o registro no arquivo binário.
        if(escreve_registro_segue(fp_bin, &reg) != 0)
        {
            fclose(fp_csv);
            fclose(fp_bin);
            return 4; // Erro ao escrever o registro no arquivo binário.
        }

        quantidadePessoas++;
        proxRRN += sizeof(RegistroSegue); // Fim dos registros.
    }

    // Atualiza o cabeçalho com os valores corretos.
    fseek(fp_bin, 0, SEEK_SET);
    status = '1';
    fwrite(&status, sizeof(char), 1, fp_bin);
    fwrite(&quantidadePessoas, sizeof(int), 1, fp_bin);
    fwrite(&proxRRN, sizeof(int), 1, fp_bin);

    return 0; // Sucesso.
}