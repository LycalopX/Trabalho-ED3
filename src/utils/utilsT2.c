#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilsT2.h"
#include "utils.h"

// Construtor da struct RegistroPessoa
RegistroPessoa *cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200])
{
    RegistroPessoa *registroPessoa = malloc(sizeof(RegistroPessoa));

    registroPessoa->removido = '0';
    registroPessoa->idPessoa = idPessoa;
    registroPessoa->idadePessoa = idade;

    registroPessoa->tamanhoNomePessoa = strlen(nomePessoa);
    registroPessoa->nomePessoa = malloc(registroPessoa->tamanhoNomePessoa);
    memcpy(registroPessoa->nomePessoa, nomePessoa, registroPessoa->tamanhoNomePessoa);

    registroPessoa->tamanhoNomeUsuario = strlen(nomeUsuario);
    registroPessoa->nomeUsuario = malloc(registroPessoa->tamanhoNomeUsuario);
    memcpy(registroPessoa->nomeUsuario, nomeUsuario, registroPessoa->tamanhoNomeUsuario);

    registroPessoa->tamanhoRegistro = sizeof(int) + sizeof(int) + sizeof(int) + registroPessoa->tamanhoNomePessoa + sizeof(int) + registroPessoa->tamanhoNomeUsuario;

    return registroPessoa;
}

RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo)
{
    int n_total = n_antigo + n_novo;
    RegistroIndice **indice_final = malloc(n_total * sizeof(RegistroIndice *));
    if (indice_final == NULL)
    {
        return NULL;
    }

    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n_antigo && j < n_novo)
    {
        if (indice_antigo[i]->idPessoa <= indice_novo[j]->idPessoa)
        {
            indice_final[k++] = indice_antigo[i++];
        }
        else
        {
            indice_final[k++] = indice_novo[j++];
        }
    }

    while (i < n_antigo)
    {
        indice_final[k++] = indice_antigo[i++];
    }
    while (j < n_novo)
    {
        indice_final[k++] = indice_novo[j++];
    }

    return indice_final;
}

// Função de comparação para bsearch, para encontrar um registro de índice por idPessoa.
int comparar_bsearch_indice(const void *key, const void *elem)
{
    int id_busca = *(const int *)key;
    const RegistroIndice *reg = *(const RegistroIndice **)elem;

    if (id_busca < reg->idPessoa)
        return -1;
    if (id_busca > reg->idPessoa)
        return 1;
    return 0;
}

int comparar_indices_id(const void *a, const void *b)
{
    RegistroIndice *regA = *(RegistroIndice **)a;
    RegistroIndice *regB = *(RegistroIndice **)b;
    return regA->idPessoa - regB->idPessoa;
}

// FUNÇÕES DE COMPARAÇÃO PARA BUSCA

int comparar_registros_busca_offset(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;

    if (regA->ByteOffset < regB->ByteOffset)
        return -1;
    else if (regA->ByteOffset > regB->ByteOffset)
        return 1;
    else
        return 0;
}

int comparar_registros_busca_id(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;

    return regA->registro->idPessoa - regB->registro->idPessoa;
}
