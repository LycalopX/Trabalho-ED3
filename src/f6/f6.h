#ifndef F6_H
#define F6_H

#include <stdio.h>

/**
 * @brief Insere novos registros no arquivo de dados 'pessoa', utilizando a estratégia de reaproveitamento de espaço (best-fit) e atualiza o arquivo de índice correspondente.
 * 
 * @param fp Ponteiro para o arquivo de dados 'pessoa' (aberto em modo rb+).
 * @param fpIndice Ponteiro para o arquivo de índice (aberto em modo rb+).
 * @param insercoes Número de registros a serem inseridos.
 */
void funcionalidade6(FILE *fp, FILE *fpIndice, int insercoes);

#endif