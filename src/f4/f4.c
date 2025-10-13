#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isspace

#include "f4.h"
#include "../arquivos.h"
#include "../utils/utils.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"
#define FALHA_AO_ALOCAR "Falha ao alocar memória.\n"

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

/**
 * @brief (VERSÃO CORRIGIDA) Lê uma busca da entrada padrão, tratando o número da busca.
 * @return Um ponteiro para a estrutura de busca criada, ou NULL em caso de falha.
 */
Busca *scanf_busca_corrigido() {
    char campo[100];
    char valor[100];
    int numero_busca;

    // 1. (CORREÇÃO) Lê e descarta o número que vem antes da busca.
    if (scanf("%d", &numero_busca) != 1) {
        return NULL;
    }

    // 2. Lê o nome do campo até o caractere '='. O espaço no início é importante!
    if (scanf(" %[^=]", campo) != 1) {
        return NULL; // Falha ao ler o campo
    }

    // 3. Consome o caractere '=' que ficou no buffer de entrada
    getchar();

    // 4. Usa a função da professora para ler o valor (com ou sem aspas)
    scan_quote_string(valor);

    // 5. Cria a estrutura de busca com os dados lidos
    return cria_busca(campo, valor);
}

void destroi_busca(Busca *busca) {
    if (busca != NULL) {
        free(busca->campo);
        free(busca->valor);
        free(busca);
    }
}

// --- FUNCIONALIDADE 4 ---

#define CAMPO_ID "idPessoa"
#define CAMPO_IDADE "idadePessoa"
#define CAMPO_NOME "nomePessoa"
#define CAMPO_USUARIO "usuarioPessoa"

int campo_valido(char *campo) {
    if (strcmp(campo, CAMPO_ID) == 0 || strcmp(campo, CAMPO_IDADE) == 0 || strcmp(campo, CAMPO_NOME) == 0 || strcmp(campo, CAMPO_USUARIO) == 0) {
        return 1;
    }
    printf("Campo inválido: %s\n", campo);
    return 0;
}

void funcionalidade4(char *nomeArquivo, char *nomeArquivoIndice, int buscas)
{
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    CabecalhoPessoa cab_pessoa;
    if (le_cabecalho_pessoa(fp, &cab_pessoa) != 0) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        fclose(fp);
        return;
    }

    FILE *fpIndice = fopen(nomeArquivoIndice, "rb");
    if (fpIndice == NULL) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        fclose(fp);
        return;
    }

    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, cab_pessoa.quantidadePessoas);
    if (indice == NULL) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        fclose(fp);
        fclose(fpIndice);
        return;
    }

    Busca **buscasArray = malloc(sizeof(Busca*) * buscas);
    if (buscasArray == NULL) {
        printf(FALHA_AO_ALOCAR);
        // (Liberação de memória omitida para brevidade)
        fclose(fp);
        fclose(fpIndice);
        return;
    }

    // --- Loop de Leitura de Buscas ---
    for (int i = 0; i < buscas; i++) {
        buscasArray[i] = scanf_busca_corrigido(); // Chamando a função corrigida

        if (buscasArray[i] == NULL || !campo_valido(buscasArray[i]->campo)) {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            // (Liberação de memória omitida para brevidade)
            fclose(fp);
            fclose(fpIndice);
            return;
        }
    }

    // --- Loop de Execução de Buscas ---
    for (int i = 0; i < buscas; i++) {
        Busca *b = buscasArray[i];

        // Lógica de busca com índice para 'idPessoa'
        if (strcmp(b->campo, CAMPO_ID) == 0) {
            int idBusca = atoi(b->valor);
            int encontrado = 0;
            // Busca binária no índice
            int left = 0, right = cab_pessoa.quantidadePessoas - 1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                if (indice[mid]->idPessoa == idBusca) {
                    fseek(fp, indice[mid]->byteOffset, SEEK_SET);
                    RegistroPessoa *reg = NULL;
                    if (le_registro_pessoa(fp, &reg) == 0) {
                        imprime_registro_pessoa(reg);
                        destroi_registro(reg);
                    }
                    encontrado = 1;
                    break;
                }
                if (indice[mid]->idPessoa < idBusca) left = mid + 1;
                else right = mid - 1;
            }
            if (!encontrado) {
                 printf("Registro inexistente.\n");
            }
        }
        else { // Busca linear para outros campos
            fseek(fp, 17, SEEK_SET); // Reposiciona para o início dos registros
            int encontrados = 0;
            for(int j = 0; j < cab_pessoa.quantidadePessoas; j++) {
                RegistroPessoa *reg = NULL;
                if (le_registro_pessoa(fp, &reg) != 0) { // Pula removido
                    continue;
                }

                int match = 0;
                if (strcmp(b->campo, CAMPO_IDADE) == 0 && reg->idadePessoa == atoi(b->valor)) {
                    match = 1;
                } else if (strcmp(b->campo, CAMPO_NOME) == 0 && reg->tamanhoNomePessoa == strlen(b->valor) && strncmp(reg->nomePessoa, b->valor, reg->tamanhoNomePessoa) == 0) {
                    match = 1;
                } else if (strcmp(b->campo, CAMPO_USUARIO) == 0 && reg->tamanhoNomeUsuario == strlen(b->valor) && strncmp(reg->nomeUsuario, b->valor, reg->tamanhoNomeUsuario) == 0) {
                    match = 1;
                }

                if(match) {
                    imprime_registro_pessoa(reg);
                    encontrados++;
                }
                destroi_registro(reg);
            }
             if (encontrados == 0) {
                 printf("Registro inexistente.\n");
            }
        }
    }

    // --- Limpeza ---
    for (int i = 0; i < buscas; i++) {
        destroi_busca(buscasArray[i]);
    }
    for (int i = 0; i < cab_pessoa.quantidadePessoas; i++) {
        destroi_registro_indice(indice[i]);
    }
    free(buscasArray);
    free(indice);
    fclose(fp);
    fclose(fpIndice);
}