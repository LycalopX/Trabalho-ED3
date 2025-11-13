#ifndef F10_H
#define F10_H

#include <stdio.h>

#include "../arquivos.h"

/**
 * @brief Carrega todos os registros do arquivo 'segueOrdenado' para a memória.
 * @param fp Ponteiro para o arquivo de dados 'segueOrdenado'.
 * @param numeroRegistros Número de registros a serem carregados.
 * @return Retorna um array de ponteiros para os registros carregados, ou NULL em caso de falha.
 */
RegistroSegue **carregar_todos_registros_segue(FILE *fp, int numeroRegistros);

/**
 * @brief Encontra todos os registros 'segue' associados a uma pessoa específica.
 * @param pessoa Ponteiro para o registro da pessoa.
 * @param registrosSegue Array de ponteiros para os registros de 'segue', que DEVEM estar ordenados por 'idPessoaQueSegue'.
 * @param numeroRegistrosSegue Número total de registros de 'segue'.
 * @param nRegsEncontrados Ponteiro para armazenar o número de registros encontrados.
 * @return Retorna um array de ponteiros para os registros de 'segue' encontrados, ou NULL em caso de falha.
 */
RegistroSegue **registro_segue_da_pessoa(RegistroPessoa *pessoa, RegistroSegue **registrosSegue, int numeroRegistrosSegue, int *nRegsEncontrados);

/**
 * @brief Implementa a funcionalidade 10: pesquisa registros do arquivo 'pessoa' e realiza a junção com o arquivo 'segueOrdenado'.
 * @param fpPessoa Ponteiro para o arquivo de dados 'pessoa'.
 * @param fpIndice Ponteiro para o arquivo de índice.
 * @param fpSegueOrdenado Ponteiro para o arquivo de dados 'segueOrdenado'.
 * @param buscas Número de buscas a serem realizadas.
 * @return Retorna 0 em caso de sucesso.
 */
int funcionalidade10(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegueOrdenado, int buscas);

#endif