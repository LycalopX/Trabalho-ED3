#ifndef SEGUE_H
#define SEGUE_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Escreve o cabeçalho do arquivo de relacionamentos 'segue'.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho com os dados a serem escritos.
 * @return 0 em sucesso, 1 em falha.
 */
int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Lê o cabeçalho do arquivo de relacionamentos 'segue'.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho a ser preenchida.
 * @return 0 em sucesso, 1 em falha.
 */
int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Lê um registro de relacionamento 'segue' da posição atual do arquivo.
 * Se o registro estiver marcado como removido, o cursor é avançado e um código especial é retornado.
 * @param fp Ponteiro para o arquivo.
 * @param reg_segue Ponteiro para a struct a ser preenchida com os dados lidos.
 * @return 0 em sucesso, LE_REGISTRO_SEGUE_REMOVIDO se o registro estiver removido, ou outro valor em erro.
 */
int le_registro_segue(FILE *fp, RegistroSegue *reg_segue);

#define LE_REGISTRO_SEGUE_REMOVIDO 1

/**
 * @brief Escreve um registro de relacionamento 'segue' no arquivo.
 * @param fp Ponteiro para o arquivo.
 * @param reg Ponteiro para a struct com os dados a serem escritos.
 * @return 0 em sucesso, 1 em falha.
 */
int escreve_registro_segue(FILE *fp, RegistroSegue *reg);

/**
 * @brief Imprime os campos de um registro de relacionamento 'segue' de forma formatada.
 * @param reg Ponteiro para o registro a ser impresso.
 */
void imprime_registro_segue(RegistroSegue *reg);

#endif // SEGUE_H

