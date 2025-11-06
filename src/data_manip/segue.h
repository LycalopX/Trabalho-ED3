#ifndef SEGUE_H
#define SEGUE_H

#include <stdio.h>
#include "../arquivos.h"

// Funções de manipulação do cabeçalho

/**
 * @brief Escreve o registro de cabeçalho no arquivo de seguidores.
 * @param fp Ponteiro para o arquivo de seguidores.
 * @param cab Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Lê o registro de cabeçalho do arquivo de seguidores.
 * @param fp Ponteiro para o arquivo de seguidores.
 * @param cab Ponteiro para a struct que será preenchida com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Lê um registro de dados do arquivo. Caso removido, avança o cursor e retorna.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg_segue Ponteiro para a struct que será preenchida com os dados lidos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_segue(FILE *fp, RegistroSegue *reg_segue);
#define LE_REGISTRO_SEGUE_REMOVIDO 1

/**
 * @brief Escreve um registro no arquivo de dados.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 se o registro for nulo.
 */
int escreve_registro_segue(FILE *fp, RegistroSegue *reg);

/**
 * @brief Imprime os campos de um registro de segue na saída padrão.
 * @param reg Ponteiro para a struct a ser impressa.
 */
void imprime_registro_segue(RegistroSegue *reg);

#endif // SEGUE_H
