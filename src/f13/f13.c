#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f13.h"
#include "../data_manip/grafo.h"

int funcionalidade13(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue, char *nomeCelebridade)
{
    // Grafo
    Grafo *grafo = criar_grafo(fpPessoa, fpIndice, fpSegue);
    if (!grafo)
    {
        printf(FALHA_AO_EXECUTAR_FUNCIONALIDADE);
        return 1; // Indica falha na criação do grafo
    }

    // Grafo transposto
    Grafo *grafoTransposto = transpor_grafo(grafo);
    if (!grafoTransposto)
    {
        destruir_grafo(grafo);
        printf(FALHA_AO_EXECUTAR_FUNCIONALIDADE);
        return 1; // Indica falha na criação do grafo transposto
    }

    // Encontrar caminhos curtos BFS e imprimir resultados
    encontrar_caminhos_curtos_bfs(grafoTransposto, nomeCelebridade, grafo);
    
    destruir_grafo(grafo);
    destruir_grafo(grafoTransposto);
    return 0; // Sucesso
}