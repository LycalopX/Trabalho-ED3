#ifndef UTILST2_H
#define UTILST2_H

#include <stdio.h>
#include "../arquivos.h"

// T2-exclusive functions

/**
 * @brief Construtor da struct RegistroPessoa.
 * @param idPessoa ID da pessoa.
 * @param nomePessoa Nome da pessoa.
 * @param idade Idade da pessoa.
 * @param nomeUsuario Nome de usuário da pessoa.
 * @return Retorna um ponteiro para a nova struct RegistroPessoa alocada.
 */
RegistroPessoa* cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200]);

/**
 * @brief Intercala (merge) dois arrays de ponteiros para RegistroIndice.
 * Ambos os arrays de entrada DEVEM estar ordenados por idPessoa.
 *
 * @param indice_antigo O array original de registros de índice.
 * @param n_antigo O número de elementos no array antigo.
 * @param indice_novo O array com os novos registros de índice.
 * @param n_novo O número de elementos no array novo.
 *
 * @return Retorna um novo array, alocado dinamicamente, contendo a intercalação
 * dos dois arrays. Retorna NULL se a alocação de memória falhar.
 */
RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo);

/**
 * @brief Função de comparação para bsearch, para encontrar um registro de índice por idPessoa.
 * @param key Ponteiro para o valor de idPessoa a ser buscado (int*).
 * @param elem Ponteiro para o elemento do array (RegistroIndice**).
 * @return Retorna -1 se a chave for menor, 1 se for maior, 0 se for igual.
 */
int comparar_bsearch_indice(const void *key, const void *elem);

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por ByteOffset.
 * @param a Ponteiro para o primeiro elemento a ser comparado.
 * @param b Ponteiro para o segundo elemento a ser comparado.
 * @return Retorna -1 se a < b, 1 se a > b,
 */
int comparar_indices_id(const void *a, const void *b);

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por ByteOffset.
 * @param a Ponteiro para o primeiro elemento a ser comparado.
 * @param b Ponteiro para o segundo elemento a ser comparado.
 * @return Retorna -1 se a < b, 1 se a > b,
 */
int comparar_registros_busca_offset(const void *a, const void *b);

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por idPessoa.
 * @param a Ponteiro para o primeiro elemento a ser comparado.
 * @param b Ponteiro para o segundo elemento a ser comparado.
 * @return Retorna a diferença entre os ids (regA->idPessoa - regB->idPessoa).
 */
int comparar_registros_busca_id(const void *a, const void *b);

#endif
