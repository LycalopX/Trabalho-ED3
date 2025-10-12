#ifndef F4_H
#define F4_H

#include "../arquivos.h"

/**
 * @brief Cria uma estrutura de busca com o campo e valor especificados.
 * @param campo O nome do campo a ser buscado (ex: "idPessoa", "nomePessoa").
 * @param valor O valor a ser buscado no campo especificado.
 * @return Um ponteiro para a estrutura de busca criada, ou NULL em caso de falha.
 */
Busca *cria_busca(char *campo, char *valor);

/**
 * @brief Lê uma busca da entrada padrão no formato "<campo>=<valor>".
 * @return Um ponteiro para a estrutura de busca criada, ou NULL em caso de falha.
 */
Busca *scanf_busca();

/**
 * @brief Libera a memória alocada para uma estrutura de busca.
 * @param busca O ponteiro para a estrutura de busca a ser destruída.
 */
void destroi_busca(Busca *busca);

void funcionalidade4(char *nomeArquivo, char *nomeArquivoIndice, int buscas);

#endif
