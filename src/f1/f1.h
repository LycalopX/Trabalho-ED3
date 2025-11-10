#ifndef F1_H
#define F1_H

#include <stdio.h>

/**
 * @brief Inicializa um arquivo de índice primário, escrevendo o cabeçalho.
 *
 * Esta função cria um cabeçalho para o arquivo de índice, definindo o status como '1'
 * (consistente) e preenchendo o restante com caracteres '$'. O cabeçalho é então
 * escrito no início do arquivo.
 *
 * @param fp Ponteiro para o arquivo de índice a ser inicializado (aberto em modo "wb").
 */
void funcionalidade1(FILE *fp);

#endif