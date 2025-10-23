#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f5.h"
#include "../f4/f4.h"

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

void funcionalidade5(FILE *fp, FILE *fpIndice, int buscas, const char *nomeArquivoDados, const char *nomeArquivoIndice)
{
    // Implementação da funcionalidade 5

    int nRegsEncontrados = 0;
    RegistroBuscaPessoa **resultados = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados);

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_resultados);

    fseek(fp, 0, 0);
    CabecalhoPessoa cp;
    le_cabecalho_pessoa(fp, &cp); // Lê o cabeçalho para posicionar o ponteiro corretamente

    // Atualizar cabeçalho
    cp.quantidadeRemovidos = cp.quantidadeRemovidos + nRegsEncontrados;
    cp.quantidadePessoas = cp.quantidadePessoas - nRegsEncontrados;
    cp.status = 0;

    escreve_cabecalho_pessoa(fp, &cp); // Atualiza o cabeçalho para refletir as remoções

    long long nextByteOffset;
    long long previousByteOffset = 17; // Começa após o cabeçalho

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
        printf("Falha no processamento do arquivo.\n");
    }

    fseek(fp, 0, 0);
    cp.status = 1;

    escreve_cabecalho_pessoa(fp, &cp);
    fflush(fp);
}
