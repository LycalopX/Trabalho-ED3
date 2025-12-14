#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/pessoa.h"
#include "../data_manip/segue.h"
#include "f10.h"

#include "../f4/f4.h"

// Carrega todos os registros de um arquivo 'segue' ordenado para a memória.
// Lida com registros logicamente removidos, sobrescrevendo-os no array temporário.
RegistroSegue **carregar_todos_registros_segue(FILE *fp, int numeroRegistros)
{
    if (fp == NULL)
    {
        return NULL;
    }

    RegistroSegue **registros = malloc(numeroRegistros * sizeof(RegistroSegue *));
    if (registros == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < numeroRegistros; i++)
    {
        registros[i] = malloc(sizeof(RegistroSegue));
        if (registros[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(registros[j]);
            }
            free(registros);
            return NULL;
        }

        int result = le_registro_segue(fp, registros[i]);
        switch (result)
        {
        case 0:
        {
            break;
        }
        case LE_REGISTRO_SEGUE_REMOVIDO:
        {
            i--;
            continue;
        }
        default:
        {
            return NULL;
        }
        }
    }

    return registros;
}

// Encontra qualquer ocorrência de um 'targetId' na lista ordenada 'registrosSegue' usando busca binária.
int binary_search_find_any(int targetId, RegistroSegue **registrosSegue, int n)
{
    int low = 0;
    int high = n - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        int midId = registrosSegue[mid]->idPessoaQueSegue;
        if (midId == targetId)
        {
            return mid;
        }
        else if (midId < targetId)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1;
}

// Busca todos os registros 'segue' associados a uma pessoa específica ('idPessoa').
// Utiliza busca binária e expansão para encontrar todas as ocorrências.
RegistroSegue **registro_segue_da_pessoa(RegistroPessoa *pessoa, RegistroSegue **registrosSegue, int numeroRegistrosSegue, int *nRegsEncontrados)
{
    if (pessoa == NULL || registrosSegue == NULL || nRegsEncontrados == NULL)
    {
        *nRegsEncontrados = 0;
        return NULL;
    }

    int initial_match_index = binary_search_find_any(pessoa->idPessoa, registrosSegue, numeroRegistrosSegue);

    if (initial_match_index == -1)
    {
        *nRegsEncontrados = 0;
        return NULL;
    }

    int start_index = initial_match_index;
    while (start_index > 0 && registrosSegue[start_index - 1]->idPessoaQueSegue == pessoa->idPessoa)
    {
        start_index--;
    }

    int end_index = initial_match_index;
    while (end_index < (numeroRegistrosSegue - 1) && registrosSegue[end_index + 1]->idPessoaQueSegue == pessoa->idPessoa)
    {
        end_index++;
    }

    int count = (end_index - start_index) + 1;
    *nRegsEncontrados = count;

    RegistroSegue **resultados = malloc(count * sizeof(RegistroSegue *));
    if (resultados == NULL)
    {
        *nRegsEncontrados = 0;
        return NULL;
    }

    for (int i = 0; i < count; i++)
    {
        resultados[i] = registrosSegue[start_index + i];
    }

    return resultados;
}

// Implementa a funcionalidade 10: busca pessoas e realiza a junção dos resultados
// com os registros de 'segue' ordenados, imprimindo os detalhes combinados.
int funcionalidade10(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegueOrdenado, int buscas)
{

    fseek(fpPessoa, 0, SEEK_SET);
    fseek(fpIndice, 0, SEEK_SET);
    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fpPessoa, fpIndice, buscas, &nRegsEncontrados, 1, 0);

    fseek(fpSegueOrdenado, 0, SEEK_SET);

    CabecalhoSegue *cabecalho = malloc(sizeof(CabecalhoSegue));
    if (le_cabecalho_segue(fpSegueOrdenado, cabecalho) != 0)
    {
        free(cabecalho);
        return 1;
    }

    int numeroRegistrosSegue = cabecalho->quantidadePessoas;

    RegistroSegue **registrosSegue = carregar_todos_registros_segue(fpSegueOrdenado, numeroRegistrosSegue);
    if (registrosSegue == NULL)
    {
        free(cabecalho);
        return 2;
    }

    if (resultadosEmBuscas != NULL)
    {
        for (int i = 0; i < buscas; i++)
        {
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {

                RegistroBuscaPessoa *registroBusca = resultadosEmBuscas[i].registrosBusca[j];
                RegistroPessoa *pessoa = registroBusca->registro;

                int nRegsSegueEncontrados = 0;
                RegistroSegue **registrosSegueDaPessoa = registro_segue_da_pessoa(pessoa, registrosSegue, numeroRegistrosSegue, &nRegsSegueEncontrados);

                imprime_registro_pessoa(pessoa);
                for (int k = 0; k < nRegsSegueEncontrados; k++)
                {
                    imprime_registro_segue(registrosSegueDaPessoa[k]);
                }

                free(registrosSegueDaPessoa);
                printf("\n");
            }
            free(resultadosEmBuscas[i].registrosBusca);
        }
        free(resultadosEmBuscas);
    }

    for (int i = 0; i < numeroRegistrosSegue; i++)
    {
        free(registrosSegue[i]);
    }
    free(registrosSegue);
    free(cabecalho);
    return 0;
}