#ifndef SEGUE_H
#define SEGUE_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Escreve o cabeçalho no arquivo de dados 'segue'.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho com os dados a serem escritos.
 * @return Retorna 0 em sucesso, 1 em falha.
 */
int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Lê o cabeçalho do arquivo de dados 'segue'.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho a ser preenchida.
 * @return Retorna 0 em sucesso, 1 em falha.
 */
int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Lê um registro de relacionamento 'segue' da posição atual do arquivo.
 * @param fp Ponteiro para o arquivo.
 * @param reg_segue Ponteiro para a struct a ser preenchida com os dados lidos.
 * @return Retorna 0 em sucesso, ou um código de status em caso de erro ou registro removido.
 */
int le_registro_segue(FILE *fp, RegistroSegue *reg_segue);

// Código de status retornado por le_registro_segue quando um registro logicamente removido é encontrado.
#define LE_REGISTRO_SEGUE_REMOVIDO 1

/**
 * @brief Escreve um registro de relacionamento 'segue' no arquivo.
 * @param fp Ponteiro para o arquivo.
 * @param reg Ponteiro para a struct com os dados a serem escritos.
 * @return Retorna 0 em sucesso, 1 em falha.
 */
int escreve_registro_segue(FILE *fp, RegistroSegue *reg);

/**
 * @brief Imprime os campos de um registro de relacionamento 'segue' de forma formatada.
 * @param reg Ponteiro para o registro a ser impresso.
 */
void imprime_registro_segue(RegistroSegue *reg);

#endif // SEGUE_H
