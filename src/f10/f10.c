#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/pessoa.h"
#include "../data_manip/segue.h"
#include "f10.h"
#include "../f4/f4.h"

// Carrega todos os registros do arquivo segueOrdenado na memória
RegistroSegue **carregar_todos_registros_segue(FILE *fp, int numeroRegistros) {
    if (fp == NULL) {
        return NULL;
    }

    RegistroSegue **registros = malloc(numeroRegistros * sizeof(RegistroSegue *));
    if (registros == NULL) {
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
            case 0: // Sucesso
            {
                break;
            }
            case LE_REGISTRO_SEGUE_REMOVIDO: // Registro removido
            {
                i--; // Decrementa o índice para sobrescrever este registro
                continue;
            }
            default: // Erro
            {
                return NULL;
            }
        }
    }

    return registros;
}

// Função auxiliar para busca binária que encontra qualquer ocorrência do idPessoaQueSegue
int binary_search_find_any(int targetId, RegistroSegue **registrosSegue, int n) {
    int low = 0;
    int high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int midId = registrosSegue[mid]->idPessoaQueSegue;
        if (midId == targetId) {
            return mid; // Encontrou
        } else if (midId < targetId) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1; // Não encontrou
}

// Usa busca binária para encontrar os registros segue associados a uma pessoa específica
RegistroSegue **registro_segue_da_pessoa(RegistroPessoa *pessoa, RegistroSegue **registrosSegue, int numeroRegistrosSegue, int *nRegsEncontrados) {
    if (pessoa == NULL || registrosSegue == NULL || nRegsEncontrados == NULL) {
        *nRegsEncontrados = 0;
        return NULL;
    }

    // Encontra um registro correspondente
    int initial_match_index = binary_search_find_any(pessoa->idPessoa, registrosSegue, numeroRegistrosSegue);

    if (initial_match_index == -1) {
        *nRegsEncontrados = 0;
        return NULL;
    }

    // Expande para trás
    int start_index = initial_match_index;
    while (start_index > 0 && registrosSegue[start_index - 1]->idPessoaQueSegue == pessoa->idPessoa) {
        start_index--;
    }

    // Expande para frente
    int end_index = initial_match_index;
    while (end_index < (numeroRegistrosSegue - 1) && registrosSegue[end_index + 1]->idPessoaQueSegue == pessoa->idPessoa) {
        end_index++;
    }

    // Calcula contagem usando as pontas
    int count = (end_index - start_index) + 1;
    *nRegsEncontrados = count;

    // Aloca o tamanho
    RegistroSegue **resultados = malloc(count * sizeof(RegistroSegue *));
    if (resultados == NULL) {
        *nRegsEncontrados = 0;
        return NULL;
    }

    // Copia o resultado
    for (int i = 0; i < count; i++) {
        resultados[i] = registrosSegue[start_index + i];
    }

    return resultados;
}

// Busca pessoas e realiza junção com segueOrdenado.
int funcionalidade10(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegueOrdenado, int buscas)
{
    // Busca as pessoas
    fseek(fpPessoa, 0, SEEK_SET); // Inicio do arquivo
    fseek(fpIndice, 0, SEEK_SET); // Inicio do arquivo
    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fpPessoa, fpIndice, buscas, &nRegsEncontrados, 1, 0);

    // Carrega todo o arquivo segueOrdenado na memória
    fseek(fpSegueOrdenado, 0, SEEK_SET); // Inicio do arquivo
    // Carrega o cabeçalho
    CabecalhoSegue *cabecalho = malloc(sizeof(CabecalhoSegue));
    if(le_cabecalho_segue(fpSegueOrdenado, cabecalho) != 0) 
    {
        free(cabecalho);
        return 1; // Falha ao ler o cabeçalho
    }

    // Carrega os registros
    int numeroRegistrosSegue = cabecalho->quantidadePessoas;
    //printf("Número de RegistroSegue carregados: %d\n", numeroRegistrosSegue); // [DEBUG]
    RegistroSegue **registrosSegue = carregar_todos_registros_segue(fpSegueOrdenado, numeroRegistrosSegue);
    if(registrosSegue == NULL) 
    {
        free(cabecalho);
        return 2; // Falha ao carregar os registros segue
    }

    // Realiza a junção com segueOrdenado
    if (resultadosEmBuscas != NULL)
    {
        //printf("Realizando junção com segueOrdenado...\n"); // [DEBUG]
        for (int i = 0; i < buscas; i++)
        {
            //printf("(%d) Resultados da busca %d:\n", resultadosEmBuscas[i].nRegistros, i + 1); // [DEBUG]
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                //printf(" - Registro %d:\n", j + 1); // [DEBUG]
                // Registro da pessoa
                RegistroBuscaPessoa *registroBusca = resultadosEmBuscas[i].registrosBusca[j];
                RegistroPessoa *pessoa = registroBusca->registro;

                // Registros segue da pessoa
                int nRegsSegueEncontrados = 0;
                RegistroSegue **registrosSegueDaPessoa = registro_segue_da_pessoa(pessoa, registrosSegue, numeroRegistrosSegue, &nRegsSegueEncontrados);
            
                // Imprime os registros segue encontrados
                imprime_registro_pessoa(pessoa);
                for(int k = 0; k < nRegsSegueEncontrados; k++) 
                {
                    imprime_registro_segue(registrosSegueDaPessoa[k]);
                }

                free(registrosSegueDaPessoa);
                printf("\n");
            }
            free(resultadosEmBuscas[i].registrosBusca);
        }
        free(resultadosEmBuscas); // Libera o array de ponteiros
    }

    // Finalização
    for (int i = 0; i < numeroRegistrosSegue; i++) {
        free(registrosSegue[i]);
    }
    free(registrosSegue);
    free(cabecalho);
    return 0; // Sucesso
}