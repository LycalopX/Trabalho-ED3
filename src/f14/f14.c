#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f14.h"
#include "../grafo/grafo.h" // For graph functions
#include "../arquivos.h"    // For FALHA_AO_EXECUTAR_FUNCIONALIDADE, NULO_DATA, etc.

int funcionalidade14(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue, char *nomeUsuarioQueGerouFofoca)
{

    Grafo *grafo = criar_grafo(fpPessoa, fpIndice, fpSegue);
    if (!grafo)
    {
        return 1;
    }

    // Call the new cycle finding function
    int ciclo_length = encontrar_ciclo_mais_curto(grafo, nomeUsuarioQueGerouFofoca);

    if (ciclo_length != -1)
    {
        printf("%d\n", ciclo_length);
    }
    else
    {
        printf("A FOFOCA NAO RETORNOU\n");
    }

    destruir_grafo(grafo); // Clean up the graph
    return 0;
}