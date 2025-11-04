#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../utils/utilsT2.h"
#include "f5.h"

#include "../f4/f4.h"

void funcionalidade5(FILE *fp, FILE *fpIndice, int buscas)
{
    // Implementação da funcionalidade 5

    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 0);

    int index = 0;
    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));

    // Imprime os resultados e libera a memória
    if (resultadosEmBuscas != NULL)
    {
        for (int i = 0; i < buscas; i++)
        {
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                resultados[index] = resultadosEmBuscas[i].registrosBusca[j];
                index++;
            }
            free(resultadosEmBuscas[i].registrosBusca);
            free(resultadosEmBuscas[i].busca.campo);
            free(resultadosEmBuscas[i].busca.valor);
        }
        free(resultadosEmBuscas); // Libera o array de ponteiros
    }

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_offset);

    if (nRegsEncontrados > 0)
    {
        int write_idx = 1;
        for (int read_idx = 1; read_idx < nRegsEncontrados; read_idx++)
        {
            if (resultados[read_idx]->ByteOffset != resultados[write_idx - 1]->ByteOffset)
            {
                resultados[write_idx] = resultados[read_idx];
                write_idx++;
            }
            else
            {
                // Libera a memória do registro duplicado que não será mantido
                destroi_registro_pessoa(resultados[read_idx]->registro);
                free(resultados[read_idx]);
            }
        }
        nRegsEncontrados = write_idx; // Atualiza a contagem para o número de registros únicos
    }

    fseek(fp, 0, 0);
    CabecalhoPessoa cp;
    le_cabecalho_pessoa(fp, &cp); // Lê o cabeçalho para posicionar o ponteiro corretamente

    cp.status = '0';
    toggle_cabecalho_pessoa(fp, &cp);

    long long nextByteOffset;
    long long previousByteOffset = 17;

    // Para atualizarmos conforme removemos pessoas
    rewind(fpIndice);
    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, cp.quantidadePessoas);
    if (indice == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(resultados);
        return;
    }

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
            int id_a_remover = resultados[i]->registro->idPessoa;

            RegistroIndice **p_encontrado_ptr = bsearch(&id_a_remover,
                                                        indice,
                                                        cp.quantidadePessoas,
                                                        sizeof(RegistroIndice *),
                                                        comparar_bsearch_indice);

            if (p_encontrado_ptr != NULL)
            {
                (*p_encontrado_ptr)->byteOffset = -1; // Marca como removido no índice
            }

            long long tamanho_real_escrito = sizeof(char) + sizeof(int) + sizeof(int) + sizeof(int) +
                                             sizeof(int) + resultados[i]->registro->tamanhoNomePessoa +
                                             sizeof(int) + resultados[i]->registro->tamanhoNomeUsuario;

            previousByteOffset += nextByteOffset + tamanho_real_escrito;

            // Libera a memória em camadas: primeiro o registro interno, depois a struct que o continha.
            destroi_registro_pessoa(resultados[i]->registro);
            free(resultados[i]);
        }
        free(resultados); // Libera o array de ponteiros
    }
    else
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
    }

    // Rebobina o arquivo de índice para o início para reescrevê-lo.
    fseek(fpIndice, 0, SEEK_SET);

    // Lê o cabeçalho existente, atualiza o status para '0' e reescreve.
    CabecalhoIndice index_header;
    le_cabecalho_indice(fpIndice, &index_header);

    index_header.status = '0';
    escreve_cabecalho_indice(fpIndice, &index_header);

    // Reescreve os registros de índice válidos.
    fseek(fpIndice, 12, SEEK_SET);
    for (int i = 0; i < cp.quantidadePessoas; i++)
    {
        if (indice[i] != NULL && indice[i]->byteOffset > 0)
        {
            fwrite(&indice[i]->idPessoa, sizeof(int), 1, fpIndice);
            fwrite(&indice[i]->byteOffset, sizeof(long long), 1, fpIndice);
        }
        destroi_registro_indice(indice[i]);
    }
    free(indice);

    // Trunca o arquivo para remover lixo do final, caso o novo conteúdo seja menor.
    long finalPos = ftell(fpIndice);
    ftruncate(fileno(fpIndice), finalPos);

    index_header.status = '1';
    escreve_cabecalho_indice(fpIndice, &index_header);

    // Atualiza o cabeçalho do arquivo de dados.
    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cp.status = '1';
    cp.quantidadeRemovidos = cp.quantidadeRemovidos + nRegsEncontrados;
    cp.quantidadePessoas = cp.quantidadePessoas - nRegsEncontrados;

    escreve_cabecalho_pessoa(fp, &cp);
    fflush(fp);
    fflush(fpIndice);
}
