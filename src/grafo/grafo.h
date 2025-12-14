#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Carrega os dados dos arquivos binários e monta o grafo em memória.
 * @param fpPessoa Ponteiro para o arquivo de dados de pessoas.
 * @param fpIndice Ponteiro para o arquivo de índice de pessoas.
 * @param fpSegue Ponteiro para o arquivo de relacionamentos 'segue'.
 * @return Retorna um ponteiro para a estrutura Grafo criada, ou NULL em caso de falha.
 */
Grafo *criar_grafo(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue);

/**
 * @brief Imprime o grafo no formato especificado (Funcionalidade 11).
 * @param grafo Ponteiro para o grafo a ser impresso.
 */
void imprimir_grafo(Grafo *grafo);

/**
 * @brief Gera e retorna o grafo transposto de um grafo de entrada.
 * @param grafoOriginal O grafo que será transposto.
 * @return Retorna um ponteiro para o novo grafo transposto.
 */
Grafo *transpor_grafo(Grafo *grafoOriginal);

/**
 * @brief Libera toda a memória alocada para a estrutura do grafo.
 * @param grafo O grafo a ser destruído.
 */
void destruir_grafo(Grafo *grafo);

/**
 * @brief Encontra o comprimento do ciclo mais curto a partir de um nó inicial no grafo.
 * @param g O grafo original.
 * @param nomeInicio O nome do usuário que inicia o ciclo.
 * @return Retorna o comprimento do ciclo mais curto, ou -1 se não houver ciclo ou erro.
 */
int encontrar_ciclo_mais_curto(Grafo *g, const char *nomeInicio);

/**
 * @brief Encontra e imprime os caminhos mais curtos no grafo transposto a partir de uma celebridade.
 * @param grafoTransposto O grafo transposto.
 * @param nomeCelebridade O nome da celebridade de onde os caminhos começam.
 * @param grafoOriginal O grafo original (não transposto) para referência das arestas.
 */
void encontrar_caminhos_curtos_bfs(Grafo *grafoTransposto, char *nomeCelebridade, Grafo *grafoOriginal);

#endif // GRAFO_H