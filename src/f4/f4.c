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
    fscanf(stdin, "%99s", input);
    char *campo = strtok(input, "=");
    char *valor = strtok(NULL, "=");
    // Verifica se a entrada era válida (tinha '=')
    if (campo == NULL || valor == NULL) {
        return NULL;
    }
    remover_aspas_extremas(valor);

    return cria_busca(campo, valor);
}

void destroi_busca(Busca *busca) {
    if (busca != NULL) {
        free(busca->campo);
        free(busca->valor);
        free(busca);
    }
}

// FUNCIONALIDADE 4
// Retorna 1 se o campo for valido e 0 se inválido
int campo_valido(char *campo) {
    if (strcmp(campo, "idPessoa") == 0) {
        return 1;
    } else if (strcmp(campo, "idadePessoa") == 0) {
        return 1;
    } else if (strcmp(campo, "nomePessoa") == 0) {
        return 1;
    } else if (strcmp(campo, "nomeUsuario") == 0) {
        return 1;
    } else {
        printf("Campo inválido: %s\n", campo);
        return 0;
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
        return;
    }
    CabecalhoPessoa *cab = malloc(sizeof(CabecalhoPessoa));
    if (le_cabecalho_pessoa(fp, cab) != 0) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cab);
        fclose(fp);
        return;
    }
    
    // Carrega o índice na memória
    FILE *fpIndice = fopen(nomeArquivoIndice, "rb");
    if (fpIndice == NULL) 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cab);
        fclose(fp);
        return;
    }
    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, cab->quantidadePessoas);
    if (indice == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cab);
        fclose(fp);
        fclose(fpIndice);
        return;
    }

    // Carrega os campos desejados
    Busca **buscasArray = malloc(sizeof(Busca*) * buscas);
    if (buscasArray == NULL)
    {
        printf(FALHA_AO_ALOCAR);
        fclose(fp);
        fclose(fpIndice);
        for(int i = 0; i < cab->quantidadePessoas; i++) 
        {
            destroi_registro_indice(indice[i]);
        }
        free(indice);
        free(cab);
        return;
    }

    for (int i = 0; i < buscas; i++) 
    {
        printf("%d ", i + 1);
        buscasArray[i] = scanf_busca();

        if(buscasArray[i] == NULL || !campo_valido(buscasArray[i]->campo)) // Busca inválida
        {
            // Buscas
            if(buscasArray[i] != NULL) 
            {
                destroi_busca(buscasArray[i]);
            }
            for (int j = 0; j < i; j++) 
            {
                destroi_busca(buscasArray[j]);
            }
            free(buscasArray);
            // Índice
            for(int j = 0; j < cab->quantidadePessoas; j++) 
            {
                destroi_registro_indice(indice[j]);
            }
            free(indice);
            free(cab);
            // Arquivos
            fclose(fp);
            fclose(fpIndice);

            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            return;
        }
    }
    
    // Realiza as buscas
    for (int i = 0; i < buscas; i++) 
    {
        Busca *b = buscasArray[i];

        // Busca com índice
        if(strcmp(b->campo, "idPessoa") == 0) 
        {
            int idBusca = atoi(b->valor);
            // Busca binária no índice
            int left = 0;
            int right = cab->quantidadePessoas - 1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                if (indice[mid]->idPessoa == idBusca) {
                    fseek(fp, indice[mid]->byteOffset, SEEK_SET);
                    RegistroPessoa *reg = NULL;
                    if (le_registro_pessoa(fp, &reg) == 0) {
                        imprime_registro_pessoa(reg);
                        destroi_registro(reg);
                    }
                    break;
                } else if (indice[mid]->idPessoa < idBusca) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
        } 
        else // Busca linear
        {
            int registrosLidos = 0;
            if(strcmp(b->campo, "idadePessoa") == 0) 
            {
                int idadeBusca = atoi(b->valor);
                // Percorrer todos os registros e exibir os que tiverem idadePessoa igual a idadeBusca
                while(registrosLidos < cab->quantidadePessoas) 
                {
                    RegistroPessoa *reg = NULL;
                    if (le_registro_pessoa(fp, &reg) != 0) // Registro removido 
                    { 
                        continue;
                    }

                    registrosLidos++;
                    if(reg->idadePessoa == idadeBusca) // Encontrou
                    {
                        imprime_registro_pessoa(reg);
                    }
                    destroi_registro(reg); // Destruir registro alocado
                }
            } 
            else if(strcmp(b->campo, "nomePessoa") == 0) 
            {
                char *nomeBusca = b->valor;
                // Percorrer todos os registros e exibir os que tiverem nomePessoa igual a nomeBusca
                while(registrosLidos < cab->quantidadePessoas) 
                {
                    RegistroPessoa *reg = NULL;
                    if (le_registro_pessoa(fp, &reg) != 0) // Registro removido 
                    { 
                        continue;
                    }

                    registrosLidos++;
                    if(reg->tamanhoNomePessoa == strlen(nomeBusca) && strncmp(reg->nomePessoa, nomeBusca, reg->tamanhoNomePessoa) == 0) // Encontrou
                    {
                        imprime_registro_pessoa(reg);
                    }
                    destroi_registro(reg); // Destruir registro alocado
                }
            } 
            else if(strcmp(b->campo, "nomeUsuario") == 0) 
            {
                char *usuarioBusca = b->valor;
                // Percorrer todos os registros e exibir os que tiverem nomeUsuario igual a usuarioBusca

                while(registrosLidos < cab->quantidadePessoas) 
                {
                    RegistroPessoa *reg = NULL;
                    if (le_registro_pessoa(fp, &reg) != 0) // Registro removido 
                    { 
                        continue;
                    }

                    registrosLidos++;
                    if(reg->tamanhoNomeUsuario == strlen(usuarioBusca) && strncmp(reg->nomeUsuario, usuarioBusca, reg->tamanhoNomeUsuario) == 0) // Encontrou
                    {
                        imprime_registro_pessoa(reg);
                    }
                    destroi_registro(reg); // Destruir registro alocado
                }
            }
        } 

        // Retorna o cursor para o início dos registros
        fseek(fp, 17, SEEK_SET); // 17 é o tamanho do cabeçalho
    }

    // Limpeza
    for (int i = 0; i < buscas; i++) 
    {
        destroi_busca(buscasArray[i]);
    }

    for(int i = 0; i < cab->quantidadePessoas; i++) 
    {
        destroi_registro_indice(indice[i]);
    }

    free(buscasArray);
    free(indice);
    free(cab);

    fclose(fp);
    fclose(fpIndice);
}