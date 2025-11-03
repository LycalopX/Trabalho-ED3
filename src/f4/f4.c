#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Necessário para a função isspace

#include "f4.h"
#include "../arquivos.h"
#include "../utils/utils.h"

Parametro *cria_busca(char *campo, char *valor)
{
    Parametro *busca = malloc(sizeof(Parametro));
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
 * @brief Lê uma busca da entrada padrão, tratando o número da busca.
 * @return Um ponteiro para a estrutura de busca criada, ou NULL em caso de falha.
 * @param updates Se não for NULL, armazena os valores lidos para atualizações (usado na funcionalidade 7).
 */
Parametro *scanf_busca(Parametro **updates, int i)
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

    getchar();
    scan_quote_string(valor);

    // Caso haja update
    if (updates != NULL)
    {
        char campo2[100];
        char valor2[100];

        // Remover espaço antes do campo
        getchar();

        if (scanf(" %[^=]", campo2) != 1)
        {
            return NULL; // Falha ao ler o campo
        }

        getchar();
        scan_quote_string(valor2);

        updates[i] = cria_busca(campo2, valor2);
    }

    return cria_busca(campo, valor);
}

void destroi_busca(Parametro *busca)
{
    if (busca != NULL)
    {
        free(busca->campo);
        free(busca->valor);
        free(busca);
    }
}

int campo_valido(char *campo)
{
    if (strcmp(campo, CAMPO_ID) == 0 || strcmp(campo, CAMPO_IDADE) == 0 || strcmp(campo, CAMPO_NOME) == 0 || strcmp(campo, CAMPO_USUARIO) == 0)
    {
        return 1;
    }
    printf("Campo inválido: %s\n", campo);
    return 0;
}

ResultadoBuscaPessoa *funcionalidade4(FILE *fp, FILE *fpIndice, int buscas, int *nRegsEncontrados, int silent, int updateBool)
{
    // Caso tenhamos updates (ela é diferente de NULL), estamos rodando para a funcionalidade 7

    char *falha_ao_processar_arquivo = FALHA_AO_PROCESSAR_ARQUIVO;
    if (silent)
    {
        falha_ao_processar_arquivo = "";
    }

    CabecalhoPessoa cab_pessoa;
    if ((le_cabecalho_pessoa(fp, &cab_pessoa) != 0 || cab_pessoa.status == '0')) {
        printf("%s", falha_ao_processar_arquivo);
        return NULL;
    }

    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, cab_pessoa.quantidadePessoas);
    if ((indice == NULL && cab_pessoa.quantidadePessoas > 0))
    {
        printf("%s", falha_ao_processar_arquivo);
        return NULL;
    }

    Parametro **buscasArray = malloc(sizeof(Parametro *) * buscas);
    ResultadoBuscaPessoa *resultadoBusca = malloc(sizeof(ResultadoBuscaPessoa) * buscas);
    Parametro **updatesArray = NULL;
    
    if (updateBool) {
        updatesArray = malloc(sizeof(Parametro *) * buscas);
        if (updatesArray == NULL) {
            printf(FALHA_AO_ALOCAR);
            free(buscasArray);
            free(resultadoBusca);
            if (indice) {
                for (int j = 0; j < cab_pessoa.quantidadePessoas; j++)
                    destroi_registro_indice(indice[j]);
                free(indice);
            }
            return NULL;
        }
    }

    for (int i = 0; i < buscas; i++)
    {
        buscasArray[i] = scanf_busca(updatesArray, i);
        if (buscasArray[i] == NULL || !campo_valido(buscasArray[i]->campo) || (updateBool && updatesArray[i] != NULL && !campo_valido(updatesArray[i]->campo)))
        {
            printf("%s", falha_ao_processar_arquivo);
            for (int j = 0; j < i; j++)
            {
                destroi_busca(buscasArray[j]);
                if (updateBool && updatesArray[j] != NULL)
                    destroi_busca(updatesArray[j]);
            }
            if (buscasArray)
            {
                free(buscasArray);
            }
            if (indice)
            {
                for (int j = 0; j < cab_pessoa.quantidadePessoas; j++)
                    destroi_registro_indice(indice[j]);
                free(indice);
            }
            if (resultadoBusca)
            {
                free(resultadoBusca);
            }
            return NULL;
        }
    }

    for (int i = 0; i < buscas; i++)
    {
        Parametro *b = buscasArray[i];
        int encontradosNestaBusca = 0;

        resultadoBusca[i].busca.campo = strdup(b->campo);
        resultadoBusca[i].busca.valor = strdup(b->valor);
        if (updateBool && updatesArray[i] != NULL)
        {
            resultadoBusca[i].update.campo = strdup(updatesArray[i]->campo);
            resultadoBusca[i].update.valor = strdup(updatesArray[i]->valor);
        }
        else
        {
            resultadoBusca[i].update.campo = NULL;
            resultadoBusca[i].update.valor = NULL;
        }
        resultadoBusca[i].registrosBusca = NULL;
        resultadoBusca[i].nRegistros = 0;

        size_t tamanhoRegs = 10;
        RegistroBuscaPessoa **regs = malloc(sizeof(RegistroBuscaPessoa *) * tamanhoRegs);

        if (!regs)
        {
            printf(FALHA_AO_ALOCAR);
            continue; // Skip this search
        }

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
                        if (resultadoBusca[i].nRegistros >= tamanhoRegs)
                        {
                            realloc_golden((void **)&regs, &tamanhoRegs, sizeof(RegistroBuscaPessoa *));
                        }
                        regs[resultadoBusca[i].nRegistros] = malloc(sizeof(RegistroBuscaPessoa));
                        regs[resultadoBusca[i].nRegistros]->registro = reg;
                        regs[resultadoBusca[i].nRegistros]->ByteOffset = indice[mid]->byteOffset;

                        (resultadoBusca[i].nRegistros)++;
                        encontradosNestaBusca++;
                    }
                    else if (reg)
                    {
                        destroi_registro_pessoa(reg);
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
            fseek(fp, 17, SEEK_SET);
            for (int j = 0; j < cab_pessoa.quantidadePessoas + cab_pessoa.quantidadeRemovidos; j++)
            {
                long long recordStartOffset = ftell(fp);
                RegistroPessoa *reg = NULL;

                if (le_registro_pessoa(fp, &reg) != 0)
                {
                    break;
                }

                if (reg->removido == '1')
                {
                    destroi_registro_pessoa(reg);
                    continue;
                }

                int match = 0;
                if (strcmp(b->campo, CAMPO_IDADE) == 0)
                {
                    if (strcmp(b->valor, "") == 0)
                    {
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
                    if (reg->tamanhoNomePessoa > 0 && reg->tamanhoNomePessoa == strlen(b->valor) && strncmp(reg->nomePessoa, b->valor, reg->tamanhoNomePessoa) == 0)
                        match = 1;
                }
                else if (strcmp(b->campo, CAMPO_USUARIO) == 0)
                {
                    if (reg->tamanhoNomeUsuario > 0 && reg->tamanhoNomeUsuario == strlen(b->valor) && strncmp(reg->nomeUsuario, b->valor, reg->tamanhoNomeUsuario) == 0)
                        match = 1;
                }

                if (match)
                {
                    if (resultadoBusca[i].nRegistros >= tamanhoRegs)
                    {
                        realloc_golden((void **)&regs, &tamanhoRegs, sizeof(RegistroBuscaPessoa *));
                    }
                    regs[resultadoBusca[i].nRegistros] = malloc(sizeof(RegistroBuscaPessoa));
                    regs[resultadoBusca[i].nRegistros]->registro = reg;
                    regs[resultadoBusca[i].nRegistros]->ByteOffset = recordStartOffset;

                    (resultadoBusca[i].nRegistros)++;
                    encontradosNestaBusca++;
                }
                else
                {
                    destroi_registro_pessoa(reg);
                }
            }
        }

        if (encontradosNestaBusca == 0 && !silent)
        {
            printf("Registro inexistente.\n");
            free(regs); // Libera se não encontrou nada
        }
        else
        {
            resultadoBusca[i].registrosBusca = regs;
        }
        (*nRegsEncontrados) += encontradosNestaBusca;
    }

    // Libera memória interna que não será retornada
    for (int i = 0; i < buscas; i++)
    {
        destroi_busca(buscasArray[i]);
        if (updateBool && updatesArray[i] != NULL)
        {
            destroi_busca(updatesArray[i]);
        }
    }
    free(buscasArray);
    if (updateBool)
    {
        free(updatesArray);
    }
    if (indice)
    {
        for (int i = 0; i < cab_pessoa.quantidadePessoas; i++)
        {
            destroi_registro_indice(indice[i]);
        }
        free(indice);
    }

    return resultadoBusca;
}
