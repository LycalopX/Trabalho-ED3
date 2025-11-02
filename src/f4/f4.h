#ifndef F4_H
#define F4_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Cria uma estrutura de busca com o campo e valor especificados.
 * @param campo O nome do campo a ser buscado (ex: "idPessoa", "nomePessoa").
 * @param valor O valor a ser buscado no campo especificado.
 * @return Um ponteiro para a estrutura de busca criada, ou NULL em caso de falha.
 */
Parametro *cria_busca(char *campo, char *valor);

/**
 * @brief Lê uma busca da entrada padrão, incluindo o número da busca (que é descartado).
 * @return Um ponteiro para a estrutura de busca criada, ou NULL em caso de falha.
 */
Parametro *scanf_busca_corrigido();

/**
 * @brief Libera a memória alocada para uma estrutura de busca.
 * @param busca O ponteiro para a estrutura de busca a ser destruída.
 */
void destroi_busca(Parametro *busca);

/**
 * @brief Realiza buscas em um arquivo de dados, usando um arquivo de índice para otimização.
 * A função assume que os arquivos já estão abertos. Não os fecha.
 * 
 * @param fp Ponteiro para o arquivo de dados (aberto em "rb").
 * @param fpIndice Ponteiro para o arquivo de índice (aberto em "rb").
 * @param buscas Número de buscas a serem realizadas.
 * @param nRegsEncontrados Ponteiro para um inteiro onde será armazenado o número total de registros encontrados.
 * @param silent Se 1, suprime as mensagens de erro como "Registro inexistente.".
 * @param updates Se não for NULL, armazena os valores lidos para atualizações (usado na funcionalidade 7).
 * @return Um array dinâmico de ponteiros para ResultadoBuscaPessoa, contendo os resultados das buscas.
 */
ResultadoBuscaPessoa *funcionalidade4(FILE *fp, FILE *fpIndice, int buscas, int *nRegsEncontrados, int silent, int updateBool);

#endif