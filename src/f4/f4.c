#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f4.h"
#include "../arquivos.h"
#include "../utils/utils.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"
#define FALHA_AO_ALOCAR "Falha ao alocar memória.\n"

// String utils
// Remove aspas duplas do início e fim de uma string, se presentes
// Somente strings com aspas nos dois extremos são modificadas
void remover_aspas_extremas(char *str) {
    // String não nula
    if (str == NULL) {
        return;
    }

    int len = strlen(str);

    // Validação
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        // Mover o bloco de caracteres (sem as aspas) para o início da string
        memmove(str, str + 1, len - 2);

        // Adiciona o novo terminador nulo para finalizar a string
        str[len - 2] = '\0';
    }
}

// BUSCA
Busca *cria_busca(char *campo, char *valor) {
    Busca *busca = malloc(sizeof(Busca));
    if (busca == NULL) {
        printf(FALHA_AO_ALOCAR);
        return NULL;
    }
    busca->campo = strdup(campo);
    busca->valor = strdup(valor);
    return busca;
}

Busca *scanf_busca() {
    char input[100];
    scanf("%s", input);
    char *campo = strtok(input, "=");
    char *valor = strtok(NULL, "=");
    return cria_busca(campo, valor);
}

void destroi_busca(Busca *busca) {
    if (busca != NULL) {
        free(busca->campo);
        free(busca->valor);
        free(busca);
    }
}

// Procura registros no arquivo de dados usando o índice ou busca linear e exibe na tela
void funcionalidade4(char *nomeArquivo, char *nomeArquivoIndice, int buscas) 
{
    // Carrega o arquivo de dados
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
    }
    CabecalhoPessoa *cab = malloc(sizeof(CabecalhoPessoa));
    if (le_cabecalho_pessoa(fp, cab) != 0) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        fclose(fp);
        return;
    }
    
    // Carrega o índice na memória
    FILE *fpIndice = fopen(nomeArquivoIndice, "rb");
    if (fpIndice == NULL) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
    }

    // Carrega os campos desejados

    Busca **buscasArray = malloc(sizeof(Busca*) * buscas);
    if (buscasArray == NULL)
    {
        printf(FALHA_AO_ALOCAR);
        fclose(fp);
        fclose(fpIndice);
        return;
    }

    for (int i = 0; i < buscas; i++) 
    {
        buscasArray[i] = scanf_busca();
    }
    
    // Realiza as buscas
    for (int i = 0; i < buscas; i++) 
    {
        Busca *b = buscasArray[i];

        // Busca com índice
        if(strcmp(b->campo, "idPessoa") == 0) 
        {
            int idBusca = atoi(b->valor);
            // Offset do registro pelo idPessoa
            // Exibir o registro encontrado ou mensagem de não encontrado
        } 
        else // Busca linear
        {
            int registrosLidos = 0;
            while(registrosLidos < cab->quantidadePessoas) 
            {
                RegistroPessoa *reg = NULL;
                if (le_registro_pessoa(fp, &reg) != 0) // Registro removido ou erro
                { 
                    continue;
                }
                registrosLidos++;

                int match = 0;
                if (strcmp(b->campo, "nomePessoa") == 0 && reg->tamanhoNomePessoa > 0) 
                {
                    char nomePessoa[reg->tamanhoNomePessoa + 1];
                    memcpy(nomePessoa, reg->nomePessoa, reg->tamanhoNomePessoa);
                    nomePessoa[reg->tamanhoNomePessoa] = '\0'; // Null-terminate
                    remover_aspas_extremas(b->valor);
                    if (strcmp(nomePessoa, b->valor) == 0) 
                    {
                        match = 1;
                    }
                } 
                else if (strcmp(b->campo, "idadePessoa") == 0) 
                {
                    int idadeBusca = atoi(b->valor);
                    if (reg->idadePessoa == idadeBusca) 
                    {
                        match = 1;
                    }
                } 
                else if (strcmp(b->campo, "nomeUsuario") == 0 && reg->tamanhoNomeUsuario > 0) 
                {
                    char nomeUsuario[reg->tamanhoNomeUsuario + 1];
                    memcpy(nomeUsuario, reg->nomeUsuario, reg->tamanhoNomeUsuario);
                    nomeUsuario[reg->tamanhoNomeUsuario] = '\0'; // Null-terminate
                    remover_aspas_extremas(b->valor);
                    if (strcmp(nomeUsuario, b->valor) == 0) 
                    {
                        match = 1;
                    }
                }

                if (match) 
                {
                    imprime_registro_pessoa(reg);
                    printf("\n");
                }

                // Limpeza
                
            }
            // Buscar os registros com o mesmo valor linearmente e exibir
        } 
    }

    // Limpeza
    fclose(fp);
    fclose(fpIndice);
}