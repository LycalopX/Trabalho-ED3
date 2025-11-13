#ifndef F9_H
#define F9_H

#include <stdio.h>

#include "../arquivos.h"

/**
 * @brief Implementa a funcionalidade 9: carrega os dados de um arquivo binário de 'segue' e os ordena.
 *
 * @param fp Ponteiro para o arquivo de dados 'segue' desordenado (aberto em "rb").
 * @param fpDestino Ponteiro para o arquivo de dados 'segue' ordenado (aberto em "wb").
 * @return Retorna 0 em caso de sucesso e -1 em caso de falha.
 */
int funcionalidade9(FILE *fp, FILE *fpDestino);

/**
 * @brief Função de comparação para ordenar registros de 'segue' por múltiplos critérios.
 *
 * @param a Ponteiro para o primeiro registro.
 * @param b Ponteiro para o segundo registro.
 * @return Retorna um valor negativo se a < b, zero se a == b, e positivo se a > b.
 */
int comparar_registros(const void *a, const void *b);

#endif