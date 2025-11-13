#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/segue.h"

// Função de comparação para qsort, utilizada para ordenar registros de relacionamento 'segue'.
int comparar_registros(const void *a, const void *b)
{
    const RegistroSegue *registroA = (const RegistroSegue *)a;
    const RegistroSegue *registroB = (const RegistroSegue *)b;

    if (registroA->idPessoaQueSegue != registroB->idPessoaQueSegue)
    {
        return registroA->idPessoaQueSegue - registroB->idPessoaQueSegue;
    }

    if (registroA->idPessoaQueESeguida != registroB->idPessoaQueESeguida)
    {
        return registroA->idPessoaQueESeguida - registroB->idPessoaQueESeguida;
    }

    int dataInicioComp = strncmp(registroA->dataInicioQueSegue, registroB->dataInicioQueSegue, 10);
    if (dataInicioComp != 0)
    {
        if (registroA->dataInicioQueSegue[0] == NULO_CARACTERE)
            return 1;
        else if (registroB->dataInicioQueSegue[0] == NULO_CARACTERE)
            return -1;
        else
            return dataInicioComp;
    }

    int dataFimComp = strncmp(registroA->dataFimQueSegue, registroB->dataFimQueSegue, 10);
    if (dataFimComp != 0)
    {
        if (registroA->dataFimQueSegue[0] == NULO_CARACTERE)
            return 1;
        else if (registroB->dataFimQueSegue[0] == NULO_CARACTERE)
            return -1;
        else
            return dataFimComp;
    }

    if (registroA->grauAmizade == NULO_CARACTERE)
        return 1;
    else if (registroB->grauAmizade == NULO_CARACTERE)
        return -1;
    return registroA->grauAmizade - registroB->grauAmizade;
}

// Implementa a funcionalidade 9: ordena os registros de um arquivo 'segue'
// e os grava em um novo arquivo de destino.
int funcionalidade9(FILE *fp, FILE *fpDestino)
{

    fseek(fp, 0, SEEK_SET);

    CabecalhoSegue *cabecalho = malloc(sizeof(CabecalhoSegue));
    if (le_cabecalho_segue(fp, cabecalho) != 0)
    {
        free(cabecalho);
        return 1;
    }

    RegistroSegue *registros = malloc(cabecalho->quantidadePessoas * sizeof(RegistroSegue));
    if (registros == NULL)
    {
        return 2;
    }

    for (int i = 0; i < cabecalho->quantidadePessoas; i++)
    {
        int result = le_registro_segue(fp, &registros[i]);
        if (result == 1)
        {
            i--;
            continue;
        }
    }

    qsort(registros, cabecalho->quantidadePessoas, sizeof(RegistroSegue), comparar_registros);

    if (escreve_cabecalho_segue(fpDestino, cabecalho) != 0)
    {
        free(registros);
        free(cabecalho);
        return 5;
    }

    for (int i = 0; i < cabecalho->quantidadePessoas; i++)
    {
        escreve_registro_segue(fpDestino, &registros[i]);
    }

    free(registros);
    free(cabecalho);

    return 0;
}