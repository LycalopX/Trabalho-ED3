#ifndef F13_H
#define F13_H

#include <stdio.h>

/**
 * @brief Recebe o nome de uma celebridade e imprime a lista de seguidores dessa celebridade.
 * 
 * @param fpPessoa Ponteiro para o arquivo de dados de pessoas (pessoa.bin).
 * @param fpIndice Ponteiro para o arquivo de índice de pessoas (indexaPessoa.bin).
 * @param fpSegue Ponteiro para o arquivo de relacionamentos ordenados (segueOrdenado.bin).
 * @param nomeCelebridade Nome da celebridade cujos seguidores serão listados.
 * @return Retorna 0 em caso de sucesso, ou 1 em caso de falha.
 */
int funcionalidade13(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue, char *nomeCelebridade);

#endif