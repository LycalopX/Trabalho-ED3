#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"
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
    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fp, &cabPessoa); // Lê o cabeçalho para posicionar o ponteiro corretamente

    cabPessoa.status = '0';
    toggle_cabecalho_pessoa(fp, &cabPessoa);

    // Para atualizarmos conforme removemos pessoas
    rewind(fpIndice);
    RegistroIndice **indices = carregar_indice_inteiro(fpIndice, cabPessoa.quantidadePessoas);
    if (indices == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(resultados);
        return;
    }

    remover_pessoas_e_indices(resultados, indices, cabPessoa, nRegsEncontrados, fp, 0);

    // Rebobina o arquivo de índice para o início para reescrevê-lo.
    fseek(fpIndice, 0, SEEK_SET);

    reescrever_arquivo_indice(fpIndice, indices, cabPessoa.quantidadePessoas);

    // Liberar memória
    for (int i = 0; i < nRegsEncontrados; i++)
    {
        destroi_registro_pessoa(resultados[i]->registro);
        free(resultados[i]);
    }
    free(resultados); // Libera o array de ponteiros

    // Atualiza o cabeçalho do arquivo de dados.
    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cabPessoa.status = '1';
    cabPessoa.quantidadeRemovidos = cabPessoa.quantidadeRemovidos + nRegsEncontrados;
    cabPessoa.quantidadePessoas = cabPessoa.quantidadePessoas - nRegsEncontrados;

    escreve_cabecalho_pessoa(fp, &cabPessoa);
    fflush(fp);
    fflush(fpIndice);
}
