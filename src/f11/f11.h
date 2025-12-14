#ifndef F11_H
#define F11_H

#include <stdio.h> // For FILE*

/**
 * @brief Implementa a funcionalidade 11: lê registros de arquivos binários,
 *        constrói um grafo em memória e o imprime no formato especificado.
 *
 * @param fpPessoa Ponteiro para o arquivo de dados de pessoas (pessoa.bin).
 * @param fpIndice Ponteiro para o arquivo de índice de pessoas (indexaPessoa.bin).
 * @param fpSegue Ponteiro para o arquivo de relacionamentos ordenados (segueOrdenado.bin).
 * @return Retorna 0 em caso de sucesso, ou 1 em caso de falha.
 */
int funcionalidade11(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue);

#endif // F11_H