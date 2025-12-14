#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f12.h"
#include "../data_manip/grafo.h"

int funcionalidade12(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue)
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

    // Finalização
    imprimir_grafo(grafoTransposto);
    destruir_grafo(grafo);
    destruir_grafo(grafoTransposto);
    return 0; // Sucesso
}