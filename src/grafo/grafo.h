#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Lê os dados dos arquivos binários e constrói a estrutura do grafo em memória.
 * 
 * A função carrega todos os usuários (vértices) e relacionamentos (arestas) dos arquivos
 * fornecidos. Os vértices são identificados pelos nomes de usuário.
 * 
 * @param fpPessoa Ponteiro para o arquivo de dados de pessoas (para obter nomes e IDs).
 * @param fpIndice Ponteiro para o arquivo de índice de pessoas (acesso rápido).
 * @param fpSegue Ponteiro para o arquivo de relacionamentos 'segue'.
 * @return Retorna um ponteiro para a estrutura Grafo alocada ou NULL em caso de erro.
 */
Grafo *criar_grafo(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue);

/**
 * @brief Imprime o grafo (listas de adjacência) no formato especificado pela funcionalidade 11.
 * 
 * Para cada vértice, imprime suas arestas de saída (quem ele segue) com os detalhes
 * (nome seguido, data inicio, data fim, grau de amizade).
 * 
 * @param grafo Ponteiro para o grafo a ser impresso.
 */
void imprimir_grafo(Grafo *grafo);

/**
 * @brief Cria a transposta do grafo fornecido (inverte a direção de todas as arestas).
 * 
 * Útil para algoritmos que precisam percorrer as relações "é seguido por" em vez de "segue".
 * 
 * @param grafoOriginal O grafo direcionado original.
 * @return Retorna um novo grafo contendo a transposta do original ou NULL em caso de erro.
 */
Grafo *transpor_grafo(Grafo *grafoOriginal);

/**
 * @brief Libera toda a memória associada ao grafo.
 * 
 * Destrói vértices, arestas e a estrutura do grafo em si.
 * 
 * @param grafo Ponteiro para o grafo a ser destruído.
 */
void destruir_grafo(Grafo *grafo);

/**
 * @brief Encontra o comprimento do ciclo mais curto que começa e termina no usuário especificado.
 * 
 * Utiliza uma busca em largura (BFS) para encontrar o menor caminho de volta ao nó inicial.
 * 
 * @param g Ponteiro para o grafo.
 * @param nomeInicio Nome do usuário onde o ciclo deve começar e terminar.
 * @return O comprimento do ciclo (número de arestas) ou -1 se não houver ciclo.
 */
int encontrar_ciclo_mais_curto(Grafo *g, const char *nomeInicio);

/**
 * @brief Executa uma BFS no grafo transposto para encontrar caminhos de uma celebridade.
 * 
 * A partir de uma celebridade (no grafo transposto), encontra quem a segue (direta ou indiretamente)
 * e imprime os caminhos.
 * 
 * @param grafoTransposto O grafo transposto (onde arestas apontam de 'seguido' para 'seguidor').
 * @param nomeCelebridade O nome da celebridade (ponto de partida da BFS no transposto).
 * @param grafoOriginal O grafo original (usado para recuperar atributos originais das arestas se necessário).
 */
void encontrar_caminhos_curtos_bfs(Grafo *grafoTransposto, char *nomeCelebridade, Grafo *grafoOriginal);

#endif // GRAFO_H