#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/segue.h"

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
    if(*p == '\0') 
    {
        // Campo nulo
        reg->idPessoaQueSegue = NULO_INTEIRO;
    }
    else
    {
        reg->idPessoaQueSegue = atoi(p);
    }
    
    // Lê o campo idPessoaQueESeguida.
    p = virgula + 1;
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 2;

    *virgula = '\0';
    if(*p == '\0') 
    {
        // Campo nulo
        reg->idPessoaQueESeguida = NULO_INTEIRO;
    }
    else
    {
        reg->idPessoaQueESeguida = atoi(p);
    }
    
    // Lê os campos dataInicioQueSegue
    p = virgula + 1;
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 3;

    *virgula = '\0';
    snprintf(reg->dataInicioQueSegue, sizeof(reg->dataInicioQueSegue), "%s", p);

    if(reg->dataInicioQueSegue[0] == '\0') {
        // Campo nulo
        strcpy(reg->dataInicioQueSegue, NULO_DATA);
    }
    
    // Lê o campo dataFimQueSegue
    p = virgula + 1;
    virgula = strchr(p, ',');

    if (virgula == NULL)
        return 4;

    *virgula = '\0';
    snprintf(reg->dataFimQueSegue, sizeof(reg->dataFimQueSegue), "%s", p);
    if(reg->dataFimQueSegue[0] == '\0') {
        // Campo nulo
        strcpy(reg->dataFimQueSegue, NULO_DATA);
    }

    // Lê o campo grauAmizade
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

    return 0; // Retorna 0 em caso de sucesso.
}

// Transforma um arquivo CSV de seguidores em um arquivo binário.
int funcionalidade8(FILE *fp_csv, FILE *fp_bin)
{
    // Escreve o cabeçalho inicial do arquivo binário (será sobrescrito no final).
    CabecalhoSegue *cab = malloc(sizeof(CabecalhoSegue));
    cab->status = '0'; // Arquivo inconsistente até o final do processamento.
    cab->quantidadePessoas = 0;
    cab->proxRRN = 0;
    escreve_cabecalho_segue(fp_bin, cab);

    char linha[256];
    // Ignora a primeira linha do CSV (cabeçalho).
    fgets(linha, sizeof(linha), fp_csv);

    // Lê cada linha do CSV e converte para RegistroSegue.
    RegistroSegue *reg = malloc(sizeof(RegistroSegue));
    while (fgets(linha, sizeof(linha), fp_csv) != NULL)
    {
        int parse_result = parse_segue_csv_line(linha, reg);
        if (parse_result != 0)
        {
            free(reg);
            return 3; // Erro ao parsear uma linha do CSV.
        }

        // DEBUG: printa o registro lido
        //imprime_registro_segue(reg); 
        
        // Escreve o registro no arquivo binário.
        if(escreve_registro_segue(fp_bin, reg) != 0)
        {
            free(reg);
            return 4; // Erro ao escrever o registro no arquivo binário.
        }

        cab->quantidadePessoas++;
        cab->proxRRN++; // Fim dos registros.
    }

    // Atualiza o cabeçalho com os valores corretos.
    fseek(fp_bin, 0, SEEK_SET);
    cab->status = '1'; // Arquivo consistente.
    
    if(escreve_cabecalho_segue(fp_bin, cab) != 0)
    {
        free(reg);
        return 5; // Erro ao atualizar o cabeçalho.
    }

    // Finaliza.
    free(reg);
    free(cab);
    return 0; // Sucesso.
}
