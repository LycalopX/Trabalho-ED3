#ifndef F9_H
#define F9_H

#include <stdio.h>

#include "../arquivos.h"

/**
 * @brief Carrega os dados do arquivo bin e ordena eles por: idPessoaQueSegue, idPessoaQueESeguida, dataInicioQueSegue, dataFimQueSegue
 * 
 * @param fp Ponteiro para o arquivo de dados segue desordenado (aberto em "rb").
 * @param fpDestino Ponteiro para o arquivo de dados segue ordenado (aberto em "wb").
 * @return int Retorna 0 em caso de sucesso e -1 em caso de falha.
 */
int funcionalidade9(FILE *fp, FILE *fpDestino);

/**
 * @brief Função de comparação para ordenar os registros de segue.
 * 
 * @param a Ponteiro para o primeiro registro.
 * @param b Ponteiro para o segundo registro.
 * @return int Retorna um valor negativo se a < b, zero se a == b, e positivo se a > b.
 */
int comparar_registros(const void *a, const void *b);

#endif