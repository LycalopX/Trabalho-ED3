#ifndef F14_H
#define F14_H

#include <stdio.h>

/**
 * @brief Recebe o nome de uma celebridade e imprime a lista de seguidores dessa celebridade.
 * @param fpPessoa Ponteiro para o arquivo de dados de pessoas (pessoa.bin).
 * @param fpIndice Ponteiro para o arquivo de índice de pessoas (indexaPessoa.bin).
 * @param fpSegue Ponteiro para o arquivo de relacionamentos ordenados (segueOrdenado.bin).
 * @param nomeUsuarioQueGerouFofoca Nome do usuário que gerou a fofoca.
 * @return Retorna 0 em caso de sucesso, ou 1 em caso de falha.
 */
int funcionalidade14(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue, char *nomeUsuarioQueGerouFofoca);

#endif