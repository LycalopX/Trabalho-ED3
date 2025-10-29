#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f5.h"
#include "../f4/f4.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"
#define FALHA_AO_ALOCAR "Falha ao alocar memória.\n"

int comparar_resultados(const void *a, const void *b)
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

int buscaBinaria(RegistroIndice **indice, int tamanho, int idBusca)
{
    int left = 0, right = tamanho - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (indice[mid]->idPessoa == idBusca)
        {
            return mid;
        }
        else if (indice[mid]->idPessoa < idBusca)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return -1; // Não encontrado
}

void funcionalidade5(FILE *fp, FILE *fpIndice, int buscas, char const *nomeArquivoIndice)
{
    // Implementação da funcionalidade 5

    int nRegsEncontrados = 0;
    RegistroBuscaPessoa **resultados = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados);

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_resultados);

    fseek(fp, 0, 0);
    CabecalhoPessoa cp;
    le_cabecalho_pessoa(fp, &cp); // Lê o cabeçalho para posicionar o ponteiro corretamente

    // Atualizar cabeçalho
    cp.status = 0;

    escreve_cabecalho_pessoa(fp, &cp); // Atualiza o cabeçalho para refletir as remoções

    long long nextByteOffset;
    long long previousByteOffset = 17; // Começa após o cabeçalho

    // Para atualizarmos conforme removemos pessoas
    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, cp.quantidadePessoas);
    if (indice == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(resultados);
        return NULL;
    }

    int PosicaoIndice;

    // Imprime os resultados e libera a memória
    if (resultados != NULL)
    {
        for (int i = 0; i < nRegsEncontrados; i++)
        {
            nextByteOffset = resultados[i]->ByteOffset - previousByteOffset;

            // Remover registro
            fseek(fp, nextByteOffset, SEEK_CUR);

            resultados[i]->registro->removido = '1';
            escreve_registro_pessoa(fp, resultados[i]->registro);

            // Remover do indice
            PosicaoIndice = buscaBinaria(indice, cp.quantidadePessoas, resultados[i]->registro->idPessoa);
            indice[PosicaoIndice]->idPessoa = -1;

            // Define qual o ByteOffset que deve servir como referência ao próximo seek
            previousByteOffset += nextByteOffset + sizeof(char) + sizeof(int) + resultados[i]->registro->tamanhoRegistro;

            // Libera a memória em camadas: primeiro o registro interno, depois a struct que o continha.
            destroi_registro(resultados[i]->registro);
            free(resultados[i]);
        }
        free(resultados); // Libera o array de ponteiros
    }
    else
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
    }

    FILE *fpIndiceWB = fopen(nomeArquivoIndice, "wb+");

    fseek(fpIndiceWB, 12, SEEK_SET);
    for (int i = 0; i < cp.quantidadePessoas; i++)
    {
        if (indice[i]->idPessoa > 0)
        {
            fwrite(&indice[i]->idPessoa, sizeof(int), 1, fpIndiceWB);
            fwrite(&indice[i]->byteOffset, sizeof(long long), 1, fpIndiceWB);
        }
        destroi_registro_indice(indice[i]);
    }

    fseek(fp, 0, 0);
    cp.status = 1;
    cp.quantidadeRemovidos = cp.quantidadeRemovidos + nRegsEncontrados;
    cp.quantidadePessoas = cp.quantidadePessoas - nRegsEncontrados;

    escreve_cabecalho_pessoa(fp, &cp);
    fflush(fp);
}
