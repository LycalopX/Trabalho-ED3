#include <stdio.h>
#include <stdlib.h>
#include "segue.h"

// Lê o cabeçalho_segue do arquivo de dados segue. Retorna 1 em caso de erro.
int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{
    if(fp == NULL || cab == NULL)
        return 1;

    fread(&cab->status, sizeof(char), 1, fp);
    fread(&cab->quantidadePessoas, sizeof(int), 1, fp);
    fread(&cab->proxRRN, sizeof(int), 1, fp);
    return 0;
}

// Escreve o cabeçalho_segue no arquivo de dados segue. Retorna 1 em caso de erro.
int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{
    fwrite(&cab->status, sizeof(char), 1, fp);
    fwrite(&cab->quantidadePessoas, sizeof(unsigned), 1, fp);
    fwrite(&cab->proxRRN, sizeof(unsigned), 1, fp);

    return 0; // Sucesso.
}

int le_registro_segue(FILE *fp, RegistroSegue *reg_segue)
{
    fread(&reg_segue->removido, sizeof(char), 1, fp);
    if(reg_segue->removido == REGISTRO_SEGUE_REMOVIDO) {
        fp += REGISTRO_SEGUE_TAMANHO - 1; // Pula o resto do registro
        return LE_REGISTRO_SEGUE_REMOVIDO;
    }

    fread(&reg_segue->idPessoaQueSegue, sizeof(int), 1, fp);
    fread(&reg_segue->idPessoaQueESeguida, sizeof(int), 1, fp);
    fread(reg_segue->dataInicioQueSegue, sizeof(char), 10, fp);
    fread(reg_segue->dataFimQueSegue, sizeof(char), 10, fp);
    fread(&reg_segue->grauAmizade, sizeof(char), 1, fp);

    return 0;
}

int escreve_registro_segue(FILE *fp, RegistroSegue *reg)
{
    if (reg == NULL)
        return 1;

    fwrite(&reg->removido, sizeof(char), 1, fp);
    fwrite(&reg->idPessoaQueSegue, sizeof(int), 1, fp);
    fwrite(&reg->idPessoaQueESeguida, sizeof(int), 1, fp);
    fwrite(reg->dataInicioQueSegue, sizeof(char), 10, fp);
    fwrite(reg->dataFimQueSegue, sizeof(char), 10, fp);
    fwrite(&reg->grauAmizade, sizeof(char), 1, fp);
    
    return 0; // Sucesso.
}

void imprime_registro_segue(RegistroSegue *reg) {
    if (reg == NULL) {
        printf("Registro nulo.\n");
        return;
    }

    // idPessoaQueESeguida
    printf("Segue a pessoa de codigo: ");
    if(reg->idPessoaQueESeguida == NULO_INTEIRO) 
    {
        printf("-\n");
    } 
    else 
    {
        printf("%d\n", reg->idPessoaQueESeguida);
    }

    // grauAmizade
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

    // dataInicioQueSegue
    printf("Começou a seguir em: ");
    if (reg->dataInicioQueSegue[0] == NULO_CARACTERE) {
        printf("-\n");
    } else {
        printf("%s\n", reg->dataInicioQueSegue);
    }

    // dataFimQueSegue
    printf("Parou de seguir em: ");
    if (reg->dataFimQueSegue[0] == NULO_CARACTERE) {
        printf("-\n");
    } else {
        printf("%s\n", reg->dataFimQueSegue);
    }

    printf("\n");
}
