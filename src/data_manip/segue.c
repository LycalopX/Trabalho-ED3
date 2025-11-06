#include <stdio.h>
#include <stdlib.h>
#include "segue.h"

int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab) {
    fseek(fp, 0, SEEK_SET);
    if (fread(&cab->status, sizeof(char), 1, fp) < 1) return 1;
    if (fread(&cab->quantidadePessoas, sizeof(int), 1, fp) < 1) return 1;
    if (fread(&cab->proxRRN, sizeof(int), 1, fp) < 1) return 1;
    return 0;
}

int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab) {
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&cab->status, sizeof(char), 1, fp) < 1) return 1;
    if (fwrite(&cab->quantidadePessoas, sizeof(int), 1, fp) < 1) return 1;
    if (fwrite(&cab->proxRRN, sizeof(int), 1, fp) < 1) return 1;
    return 0;
}

int le_registro_segue(FILE* fp, RegistroSegue** reg_out) {
    RegistroSegue *reg = malloc(sizeof(RegistroSegue));
    if (reg == NULL) return 1;

    // O registro 'segue' é de tamanho fixo, então lemos a struct inteira de uma vez.
    size_t items_read = fread(reg, sizeof(RegistroSegue), 1, fp);

    if (items_read < 1) {
        free(reg);
        return 1; // Fim de arquivo ou erro
    }

    *reg_out = reg;
    return 0;
}

void destroi_registro_segue(RegistroSegue* reg) {
    if (reg != NULL) {
        free(reg);
    }
}

int imprime_registro_segue(RegistroSegue* reg) {
    if (reg == NULL) return 1;

    // Exemplo de impressão para depuração. Pode ser ajustado.
    printf("ID Segue: %d | ID Seguido: %d | Grau: %c\n",
           reg->idPessoaQueSegue,
           reg->idPessoaQueESeguida,
           reg->grauAmizade);

    return 0;
}
