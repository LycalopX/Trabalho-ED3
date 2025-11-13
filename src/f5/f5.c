#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"
#include "f5.h"
#include "../f4/f4.h"

// Implementa a funcionalidade 5: remove logicamente registros de pessoa do arquivo de dados
// e atualiza o arquivo de índice.
void funcionalidade5(FILE *fp, FILE *fpIndice, const char *nomeArquivoIndice, int buscas)
{
    // Encontra todos os registros que correspondem aos critérios de busca.
    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 0);

    if (nRegsEncontrados == 0)
    {

        if (resultadosEmBuscas != NULL)
            free(resultadosEmBuscas);
        return;
    }

    // Unifica os resultados de múltiplas buscas em um único array.
    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));
    if (resultados == NULL)
    {

        liberar_resultados_busca_inteira(resultadosEmBuscas, buscas);
        return;
    }

    int index = 0;
    for (int i = 0; i < buscas; i++)
    {
        for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
        {
            resultados[index++] = resultadosEmBuscas[i].registrosBusca[j];
        }

        free(resultadosEmBuscas[i].registrosBusca);
        free(resultadosEmBuscas[i].busca.campo);
        free(resultadosEmBuscas[i].busca.valor);
    }
    free(resultadosEmBuscas);

    // Remove registros duplicados da lista de resultados, ordenando por byte offset.
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

                destroi_registro_pessoa(resultados[read_idx]->registro);
                free(resultados[read_idx]);
            }
        }
        nRegsEncontrados = write_idx;
    }

    // Prepara os arquivos para modificação, marcando-os como instáveis.
    fseek(fp, 0, 0);
    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fp, &cabPessoa);
    toggle_cabecalho_pessoa(fp, &cabPessoa);

    rewind(fpIndice);
    RegistroIndice **indices = carregar_indice_inteiro(fpIndice, cabPessoa.quantidadePessoas);
    if (indices == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(resultados);
        return;
    }

    // Marca os registros encontrados como removidos no arquivo de dados
    // e invalida suas entradas no índice em memória.
    remover_pessoas_e_indices(resultados, indices, &cabPessoa, nRegsEncontrados, fp, 0);

    // Reescreve o arquivo de índice com as entradas atualizadas.
    reescrever_arquivo_indice(nomeArquivoIndice, indices, cabPessoa.quantidadePessoas);

    // Libera a memória alocada para os resultados da busca.
    for (int i = 0; i < nRegsEncontrados; i++)
    {
        destroi_registro_pessoa(resultados[i]->registro);
        free(resultados[i]);
    }
    free(resultados);

    // Atualiza o cabeçalho do arquivo de dados com as novas contagens e status.
    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cabPessoa.status = '1';
    cabPessoa.quantidadeRemovidos += nRegsEncontrados;
    cabPessoa.quantidadePessoas -= nRegsEncontrados;

    escreve_cabecalho_pessoa(fp, &cabPessoa);
    fflush(fp);
}
