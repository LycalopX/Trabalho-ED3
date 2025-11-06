#ifndef SEGUE_H
#define SEGUE_H

#include <stdio.h>
#include "../arquivos.h"

// Funções de manipulação do cabeçalho

/**
 * @brief Lê o cabeçalho do arquivo de dados segue.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a estrutura de cabeçalho a ser preenchida.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha
 */
int le_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

/**
 * @brief Escreve o cabeçalho no arquivo de dados segue.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a estrutura de cabeçalho a ser escrita.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha
 */
int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab);

// Funções de manipulação de registros

/**
 * @brief Lê um registro de dados do arquivo segue.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_segue(FILE* fp, RegistroSegue** reg_out);

/**
 * @brief Escreve um registro de dados no arquivo segue.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha
 */
int escreve_registro_segue(FILE* fp, RegistroSegue* reg);

/**
 * @brief Libera a memória alocada para um registro de segue.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro_segue(RegistroSegue* reg);

/**
 * @brief Imprime um registro de segue formatado.
 * @param reg Ponteiro para o registro a ser impresso.
 * @return Retorna 0 em caso de sucesso, 1 se o registro for nulo.
 */
int imprime_registro_segue(RegistroSegue* reg);

#endif // SEGUE_H
