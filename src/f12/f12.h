#ifndef F12_H
#define F12_H

#include <stdio.h>

/**
 * @brief Transpõe o grafo de relacionamentos lido dos arquivos binários e imprime.
 * 
 * @param fpPessoa Ponteiro para o arquivo de dados de pessoas (pessoa.bin).
 * @param fpIndice Ponteiro para o arquivo de índice de pessoas (indexaPessoa.bin).
 * @param fpSegue Ponteiro para o arquivo de relacionamentos ordenados (segueOrdenado.bin).
 * @return Retorna 0 em caso de sucesso, ou 1 em caso de falha.
 */
int funcionalidade12(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue);

#endif