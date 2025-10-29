#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f6.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"

void funcionalidade6(FILE *fp, FILE *fpIndice, int insercoes)
{

    CabecalhoPessoa *cab = malloc(sizeof(CabecalhoPessoa));

    if (le_cabecalho_pessoa(fp, cab) != 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cab);
        return;
    }

    // Ordem de leitura: - valor_idPessoa, valor_nomePessoa, valor_idade, valor_nomeUsuario

    int idPessoa, idade;
    char nomePessoa[200], idadeString[4], nomeUsuario[200];
    RegistroPessoa *reg;

    // Definir registroPessoa
    for (int i = 0; i < insercoes; i++)
    {
        int numero_busca;

        // 1. Lê e descarta o número que vem antes da busca.
        if (scanf("%d", &numero_busca) != 1)
        {
            return NULL;
        }

        scanf("%d, %s, %s, %s", idPessoa, nomePessoa, idadeString, nomeUsuario);

        scan_quote_string(nomePessoa);
        scan_quote_string(idadeString);
        scan_quote_string(nomeUsuario);

        if (strcmp(idadeString, "") == 0)
        {
            idade = -1;
        }
        else
        {
            idade = atoi(idadeString);
        }

        reg = cria_registro_pessoa(idPessoa, nomePessoa, idade, nomeUsuario);

        if (reg == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            return;
        }

        

        destroi_registro(reg);
    }

}