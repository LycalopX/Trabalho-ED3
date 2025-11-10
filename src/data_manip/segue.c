#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "segue.h"

// --- Funções de Manipulação de RegistroSegue ---

// Lê o cabeçalho do arquivo de relacionamentos 'segue'.
int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{
    if(fp == NULL || cab == NULL) return 1;

    // Lê os campos do cabeçalho.
    fread(&cab->status, sizeof(char), 1, fp);
    fread(&cab->quantidadePessoas, sizeof(int), 1, fp);
    fread(&cab->proxRRN, sizeof(int), 1, fp);
    return 0; // Sucesso.
}

// Escreve o cabeçalho no arquivo de relacionamentos 'segue'.
int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{
    // Escreve os campos do cabeçalho.
    fwrite(&cab->status, sizeof(char), 1, fp);
    fwrite(&cab->quantidadePessoas, sizeof(unsigned), 1, fp);
    fwrite(&cab->proxRRN, sizeof(unsigned), 1, fp);

    return 0; // Sucesso.
}

// Lê um registro de relacionamento 'segue' do arquivo.
int le_registro_segue(FILE *fp, RegistroSegue *reg_segue)
{
    fread(&reg_segue->removido, sizeof(char), 1, fp);
    // Se o registro estiver marcado como removido, avança o ponteiro e retorna um código especial.
    if(reg_segue->removido == REGISTRO_SEGUE_REMOVIDO) {
        fp += REGISTRO_SEGUE_TAMANHO - 1; // Pula o resto do registro
        return LE_REGISTRO_SEGUE_REMOVIDO;
    }

    // Lê os campos fixos do registro.
    fread(&reg_segue->idPessoaQueSegue, sizeof(int), 1, fp);
    fread(&reg_segue->idPessoaQueESeguida, sizeof(int), 1, fp);
    fread(reg_segue->dataInicioQueSegue, sizeof(char), 10, fp);
    fread(reg_segue->dataFimQueSegue, sizeof(char), 10, fp);
    fread(&reg_segue->grauAmizade, sizeof(char), 1, fp);

    return 0; // Sucesso.
}

// Escreve um registro de relacionamento 'segue' no arquivo.
int escreve_registro_segue(FILE *fp, RegistroSegue *reg)
{
    if (reg == NULL)
        return 1;

    // Escreve os campos do registro.
    fwrite(&reg->removido, sizeof(char), 1, fp);
    fwrite(&reg->idPessoaQueSegue, sizeof(int), 1, fp);
    fwrite(&reg->idPessoaQueESeguida, sizeof(int), 1, fp);
    fwrite(reg->dataInicioQueSegue, sizeof(char), 10, fp);
    fwrite(reg->dataFimQueSegue, sizeof(char), 10, fp);
    fwrite(&reg->grauAmizade, sizeof(char), 1, fp);
    
    return 0; // Sucesso.
}

// Imprime os dados de um registro de relacionamento 'segue' de forma formatada.
void imprime_registro_segue(RegistroSegue *reg) {
    if (reg == NULL) {
        printf("Registro nulo.\n");
        return;
    }

    printf("Segue a pessoa de codigo: ");
    if(reg->idPessoaQueESeguida == NULO_INTEIRO) 
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

    printf("Começou a seguir em: ");
    if (reg->dataInicioQueSegue[0] == NULO_CARACTERE) {
        printf("-\n");
    } else {
        printf("%s\n", reg->dataInicioQueSegue);
    }

    printf("Parou de seguir em: ");
    if (reg->dataFimQueSegue[0] == NULO_CARACTERE) {
        printf("-\n");
    } else {
        printf("%s\n", reg->dataFimQueSegue);
    }

    printf("\n");
}
