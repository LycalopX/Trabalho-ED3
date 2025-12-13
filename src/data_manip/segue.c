#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "segue.h"

int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{
    if (fp == NULL || cab == NULL)
        return 1;

    if (fread(&cab->status, sizeof(char), 1, fp) != 1) return 1;
    if (fread(&cab->quantidadePessoas, sizeof(int), 1, fp) != 1) return 1;
    if (fread(&cab->proxRRN, sizeof(int), 1, fp) != 1) return 1;
    return 0;
}

int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{

    if (fwrite(&cab->status, sizeof(char), 1, fp) != 1) return 1;
    if (fwrite(&cab->quantidadePessoas, sizeof(unsigned), 1, fp) != 1) return 1;
    if (fwrite(&cab->proxRRN, sizeof(unsigned), 1, fp) != 1) return 1;

    return 0;
}

// Lê um registro de relacionamento 'segue'. Se o registro estiver
// marcado como removido, o cursor do arquivo é avançado para o próximo
// registro e um código de status especial é retornado.
int le_registro_segue(FILE *fp, RegistroSegue *reg_segue)
{
    // Check fread return value to detect EOF
    if (fread(&reg_segue->removido, sizeof(char), 1, fp) != 1) {
        return LE_REGISTRO_SEGUE_ERRO_LEITURA; // Indicate error/EOF
    }

    if (reg_segue->removido == REGISTRO_SEGUE_REMOVIDO)
    {
        fseek(fp, REGISTRO_SEGUE_TAMANHO - 1, SEEK_CUR);
        return LE_REGISTRO_SEGUE_REMOVIDO;
    }

    if (fread(&reg_segue->idPessoaQueSegue, sizeof(int), 1, fp) != 1) return LE_REGISTRO_SEGUE_ERRO_LEITURA;
    if (fread(&reg_segue->idPessoaQueESeguida, sizeof(int), 1, fp) != 1) return LE_REGISTRO_SEGUE_ERRO_LEITURA;
    if (fread(reg_segue->dataInicioQueSegue, sizeof(char), 10, fp) != 10) return LE_REGISTRO_SEGUE_ERRO_LEITURA;
    reg_segue->dataInicioQueSegue[10] = '\0'; // Null-terminate the string

    if (fread(reg_segue->dataFimQueSegue, sizeof(char), 10, fp) != 10) return LE_REGISTRO_SEGUE_ERRO_LEITURA;
    reg_segue->dataFimQueSegue[10] = '\0'; // Null-terminate the string
    
    if (fread(&reg_segue->grauAmizade, sizeof(char), 1, fp) != 1) return LE_REGISTRO_SEGUE_ERRO_LEITURA;

    return 0;
}

int escreve_registro_segue(FILE *fp, RegistroSegue *reg)
{
    if (reg == NULL)
        return 1;

    if (fwrite(&reg->removido, sizeof(char), 1, fp) != 1) return 1;
    if (fwrite(&reg->idPessoaQueSegue, sizeof(int), 1, fp) != 1) return 1;
    if (fwrite(&reg->idPessoaQueESeguida, sizeof(int), 1, fp) != 1) return 1;
    if (fwrite(reg->dataInicioQueSegue, sizeof(char), 10, fp) != 10) return 1;
    if (fwrite(reg->dataFimQueSegue, sizeof(char), 10, fp) != 10) return 1;
    if (fwrite(&reg->grauAmizade, sizeof(char), 1, fp) != 1) return 1;

    return 0;
}

void imprime_registro_segue(RegistroSegue *reg)
{
    if (reg == NULL)
    {
        printf("Registro nulo.\n");
        return;
    }

    printf("Segue a pessoa de codigo: ");
    if (reg->idPessoaQueESeguida == NULO_INTEIRO)
    {
        printf("-\n");
    }
    else
    {
        printf("%d\n", reg->idPessoaQueESeguida);
    }

    printf("Justificativa para seguir: ");
    switch (reg->grauAmizade)
    {
    case '0':
    {
        printf("%s\n", AMIZADE_0);
        break;
    }
    case '1':
    {
        printf("%s\n", AMIZADE_1);
        break;
    }
    case '2':
    {
        printf("%s\n", AMIZADE_2);
        break;
    }
    case NULO_CARACTERE:
    {
        printf("-\n");
        break;
    }
    default:
    {
        printf("Valor desconhecido (%c)\n", reg->grauAmizade);
        break;
    }
    }

    printf("Começou a seguir em: ");
    if (reg->dataInicioQueSegue[0] == NULO_CARACTERE)
    {
        printf("-\n");
    }
    else
    {
        printf("%s\n", reg->dataInicioQueSegue);
    }

    printf("Parou de seguir em: ");
    if (reg->dataFimQueSegue[0] == NULO_CARACTERE)
    {
        printf("-\n");
    }
    else
    {
        printf("%s\n", reg->dataFimQueSegue);
    }

    printf("\n");
}