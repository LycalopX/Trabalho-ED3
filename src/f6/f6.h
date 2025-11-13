#ifndef F6_H
#define F6_H

#include <stdio.h>

/**
 * @brief Implementa a funcionalidade 6: insere novos registros no arquivo de dados e atualiza o arquivo de índice.
 * 
 * @param fp Ponteiro para o arquivo de dados de pessoas.
 * @param fpIndice Ponteiro para o arquivo de índice.
 * @param nomeArquivoIndice Nome do arquivo de índice, usado para reescrevê-lo.
 * @param insercoes Número de registros a serem inseridos.
 */
void funcionalidade6(FILE *fp, FILE *fpIndice, const char *nomeArquivoIndice, int insercoes);

#endif