#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isspace

#include "f4.h"
#include "../arquivos.h"
#include "../utils/utils.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"
#define FALHA_AO_ALOCAR "Falha ao alocar memória.\n"

Busca *cria_busca(char *campo, char *valor)
{
    Busca *busca = malloc(sizeof(Busca));
    if (busca == NULL)
    {
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
Busca *scanf_busca_corrigido()
{
    char campo[100];
    char valor[100];
    int numero_busca;

    // 1. Lê e descarta o número que vem antes da busca.
    if (scanf("%d", &numero_busca) != 1)
    {
        return NULL;
    }

    // 2. Lê o nome do campo até o caractere '='. O espaço no início é importante!
    if (scanf(" %[^=]", campo) != 1)
    {
        return NULL; // Falha ao ler o campo
    }

    // 3. Consome o caractere '=' que ficou no buffer de entrada
    getchar();

    // 4. Usa a função da professora para ler o valor (com ou sem aspas)
    scan_quote_string(valor);

    // 5. Cria a estrutura de busca com os dados lidos
    return cria_busca(campo, valor);
}

void destroi_busca(Busca *busca)
{
    if (busca != NULL)
    {
        free(busca->campo);
        free(busca->valor);
        free(busca);
    }
}

// --- FUNCIONALIDADE 4 ---

#define CAMPO_ID "idPessoa"
#define CAMPO_IDADE "idadePessoa"
#define CAMPO_NOME "nomePessoa"
#define CAMPO_USUARIO "nomeUsuario"

int campo_valido(char *campo)
{
    if (strcmp(campo, CAMPO_ID) == 0 || strcmp(campo, CAMPO_IDADE) == 0 || strcmp(campo, CAMPO_NOME) == 0 || strcmp(campo, CAMPO_USUARIO) == 0)
    {
        return 1;
    }
    printf("Campo inválido: %s\n", campo);
    return 0;
}

RegistroBuscaPessoa **funcionalidade4(FILE *fp, FILE *fpIndice, int buscas, int *nRegsEncontrados)
{
    *nRegsEncontrados = 0;
    size_t tamanhoRegs = 10;
    RegistroBuscaPessoa **regs = malloc(sizeof(RegistroBuscaPessoa *) * tamanhoRegs);

    if (!regs)
    {
        printf(FALHA_AO_ALOCAR);
        return NULL;
    }

    CabecalhoPessoa cab_pessoa;
    if (le_cabecalho_pessoa(fp, &cab_pessoa) != 0 || cab_pessoa.status == '0')
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(regs);
        return NULL;
    }

    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, cab_pessoa.quantidadePessoas);
    if (indice == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(regs);
        return NULL;
    }

    Busca **buscasArray = malloc(sizeof(Busca *) * buscas);
    if (buscasArray == NULL)
    {
        printf(FALHA_AO_ALOCAR);
        free(regs);
        // (Liberação de memória do índice omitida para brevidade)
        return NULL;
    }

    // --- Loop de Leitura de Buscas ---
    for (int i = 0; i < buscas; i++)
    {
        buscasArray[i] = scanf_busca_corrigido();
        if (buscasArray[i] == NULL || !campo_valido(buscasArray[i]->campo))
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            // (Liberação de memória omitida para brevidade)
            free(regs);
            free(buscasArray);
            return NULL;
        }
    }

    // --- Loop de Execução de Buscas ---
    for (int i = 0; i < buscas; i++)
    {
        Busca *b = buscasArray[i];
        int encontradosNestaBusca = 0;

        if (strcmp(b->campo, CAMPO_ID) == 0)
        {
            int idBusca = atoi(b->valor);
            int left = 0, right = cab_pessoa.quantidadePessoas - 1;
            while (left <= right)
            {
                int mid = left + (right - left) / 2;
                if (indice[mid]->idPessoa == idBusca)
                {
                    fseek(fp, indice[mid]->byteOffset, SEEK_SET);
                    RegistroPessoa *reg = NULL;
                    if (le_registro_pessoa(fp, &reg) == 0 && reg->removido == '0')
                    {
                        if (*nRegsEncontrados >= tamanhoRegs)
                        {
                            realloc_golden((void **)&regs, &tamanhoRegs, sizeof(RegistroBuscaPessoa *));
                        }
                        regs[*nRegsEncontrados] = malloc(sizeof(RegistroBuscaPessoa));
                        regs[*nRegsEncontrados]->registro = reg;
                        regs[*nRegsEncontrados]->ByteOffset = indice[mid]->byteOffset;

                        (*nRegsEncontrados)++;
                        encontradosNestaBusca++;
                    }
                    else if (reg)
                    {
                        destroi_registro(reg);
                    }
                    break;
                }
                if (indice[mid]->idPessoa < idBusca)
                    left = mid + 1;
                else
                    right = mid - 1;
            }
        }
        else
        {
            long long byteOffset = 17; // Início dos registros

            fseek(fp, 17, SEEK_SET);
            for (int j = 0; j < cab_pessoa.quantidadePessoas + cab_pessoa.quantidadeRemovidos; j++)
            {
                RegistroPessoa *reg = NULL;
                if (le_registro_pessoa(fp, &reg) != 0)
                    continue;

                if (reg->removido == '1')
                {
                    destroi_registro(reg);
                    continue;
                }

                int match = 0;
                if (strcmp(b->campo, CAMPO_IDADE) == 0)
                {
                    if (strcmp(b->valor, "") == 0)
                    { // Busca por idade NULA
                        if (reg->idadePessoa == -1)
                            match = 1;
                    }
                    else
                    {
                        if (reg->idadePessoa == atoi(b->valor))
                            match = 1;
                    }
                }
                else if (strcmp(b->campo, CAMPO_NOME) == 0)
                {
                    if (reg->tamanhoNomePessoa == strlen(b->valor) && strncmp(reg->nomePessoa, b->valor, reg->tamanhoNomePessoa) == 0)
                        match = 1;
                }
                else if (strcmp(b->campo, CAMPO_USUARIO) == 0)
                {
                    if (reg->tamanhoNomeUsuario == strlen(b->valor) && strncmp(reg->nomeUsuario, b->valor, reg->tamanhoNomeUsuario) == 0)
                        match = 1;
                }

                if (match)
                {
                    if (*nRegsEncontrados >= tamanhoRegs)
                    {
                        realloc_golden((void **)&regs, &tamanhoRegs, sizeof(RegistroBuscaPessoa *));
                    }
                    // Aloca memória para a struct que encapsula o resultado
                    regs[*nRegsEncontrados] = malloc(sizeof(RegistroBuscaPessoa));
                    regs[*nRegsEncontrados]->registro = reg;
                    regs[*nRegsEncontrados]->ByteOffset = byteOffset;

                    (*nRegsEncontrados)++;
                    encontradosNestaBusca++;
                }
                else
                {
                    destroi_registro(reg);
                }

                int tamanho_total_em_disco = sizeof(reg->removido) + sizeof(reg->tamanhoRegistro) + sizeof(reg->idPessoa) + sizeof(reg->idadePessoa) + sizeof(reg->tamanhoNomePessoa) + sizeof(reg->tamanhoNomeUsuario) + reg->tamanhoNomePessoa + reg->tamanhoNomeUsuario;
                byteOffset += tamanho_total_em_disco;
            }
        }
        if (encontradosNestaBusca == 0)
        {
            printf("Registro inexistente.\n\n");
        }
    }

    // --- Limpeza ---
    for (int i = 0; i < buscas; i++)
        destroi_busca(buscasArray[i]);
    for (int i = 0; i < cab_pessoa.quantidadePessoas; i++)
        destroi_registro_indice(indice[i]);
    free(buscasArray);
    free(indice);

    return regs;
}