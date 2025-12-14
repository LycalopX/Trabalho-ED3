#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f11.h"
#include "../grafo/grafo.h"

int funcionalidade11(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue)
{
    Grafo *grafo = criar_grafo(fpPessoa, fpIndice, fpSegue);
    if (!grafo)
    {
        printf(FALHA_AO_EXECUTAR_FUNCIONALIDADE);
        return 1; // Indica falha na criação do grafo
    }

    imprimir_grafo(grafo);
    destruir_grafo(grafo);
    return 0; // Sucesso
}