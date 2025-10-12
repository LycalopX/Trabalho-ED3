#include <stdio.h>
#include <stdlib.h>

#include "f3.h"
#include "../arquivos.h"
#include "../utils/utils.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"

// Lê os registros do arquivo de dados e exibe na tela
void funcionalidade3(char *nomeArquivo) 
{
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    // Coleta o numero de registros que devem ser lidos
    CabecalhoPessoa *cab = malloc(sizeof(CabecalhoPessoa));
    if (le_cabecalho_pessoa(fp, cab) != 0) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        fclose(fp);
        return;
    }

    int numRegistros = cab->quantidadePessoas;

    // Lê e exibe os registros
    RegistroPessoa **reg = malloc(sizeof(RegistroPessoa *));
    int registrosLidos = 0;
    while (registrosLidos < numRegistros) 
    {
        if (le_registro_pessoa(fp, reg) != 0) // Registro removido 
        { 
            printf("Falha ao ler registro %d.\n", registrosLidos + 1);
            continue;
        }
        registrosLidos++;
        imprime_registro_pessoa(*reg);
        destroi_registro(*reg); // Destruir registro alocado
        printf("\n");
    }

    free(cab);
    free(reg);
    fclose(fp);
}
