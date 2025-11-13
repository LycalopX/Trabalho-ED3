#ifndef F5_H
#define F5_H

#include <stdio.h>

/**
 * @brief Implementa a funcionalidade 5: remove logicamente registros de um arquivo de dados com base em critérios de busca.
 * 
 * @param fp Ponteiro para o arquivo de dados de pessoas.
 * @param fpIndice Ponteiro para o arquivo de índice.
 * @param nomeArquivoIndice Nome do arquivo de índice, usado para reescrevê-lo.
 * @param buscas Número de operações de busca e remoção a serem realizadas.
 */
void funcionalidade5(FILE *fp, FILE *fpIndice, const char *nomeArquivoIndice, int buscas);

#endif
