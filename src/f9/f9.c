#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f9.h"

// Compara a e b e retorna um valor negativo se a < b, zero se a == b, e positivo se a > b
int comparar_registros(const void *a, const void *b) {
    const RegistroSegue *registroA = (const RegistroSegue *)a;
    const RegistroSegue *registroB = (const RegistroSegue *)b;

    // idPessoaQueSegue
    if (registroA->idPessoaQueSegue != registroB->idPessoaQueSegue) {
        return registroA->idPessoaQueSegue - registroB->idPessoaQueSegue;
    }
    // idPessoaQueESeguida
    if (registroA->idPessoaQueESeguida != registroB->idPessoaQueESeguida) {
        return registroA->idPessoaQueESeguida - registroB->idPessoaQueESeguida;
    }
    // dataInicioQueSegue
    int dataInicioComp = strncmp(registroA->dataInicioQueSegue, registroB->dataInicioQueSegue, 10);
    if (dataInicioComp != 0) {
        return dataInicioComp;
    }
    // dataFimQueSegue
    return strncmp(registroA->dataFimQueSegue, registroB->dataFimQueSegue, 10);
}

// Ordena os registros do arquivo de dados segue e grava no arquivo de destino
int funcionalidade9(FILE *fp, FILE *fpDestino) {
    // Move o ponteiro para o início do arquivo
    fseek(fp, 0, SEEK_SET);

    // Lê o cabeçalho
    CabecalhoSegue cabecalho;
    fread(&cabecalho, sizeof(CabecalhoSegue), 1, fp);

    // Aloca memória para os registros
    RegistroSegue *registros = malloc(cabecalho.quantidadePessoas * sizeof(RegistroSegue));
    if (registros == NULL) {
        return -1; // Falha na alocação de memória
    }

    // Lê os registros
    for (int i = 0; i < cabecalho.quantidadePessoas; i++) {
        fread(&registros[i], sizeof(RegistroSegue), 1, fp);
    }

    // Ordena os registros usando qsort (biblioteca C) e a função de comparação
    qsort(registros, cabecalho.quantidadePessoas, sizeof(RegistroSegue), comparar_registros);

    // Escreve o cabeçalho no arquivo de destino
    fwrite(&cabecalho, sizeof(CabecalhoSegue), 1, fpDestino);

    // Escreve os registros ordenados no arquivo de destino (DEBUG)
    /*for (int i = 0; i < cabecalho.quantidadePessoas; i++) {
        fwrite(&registros[i], sizeof(RegistroSegue), 1, fpDestino);
    }*/

    // Libera a memória alocada
    free(registros);

    return 0; // Sucesso
}