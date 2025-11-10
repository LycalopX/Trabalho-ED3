#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"
#include "f5.h"
#include "../f4/f4.h"

void funcionalidade5(FILE *fp, FILE *fpIndice, const char *nomeArquivoIndice, int buscas)
{
    // 1. Encontrar todos os registros que correspondem aos critérios de busca.
    // A funcionalidade 4 é reutilizada aqui para realizar as buscas.
    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 0);

    // Se não houver resultados, não há nada a fazer.
    if (nRegsEncontrados == 0) {
        // Libera a memória alocada pela busca, se houver.
        if (resultadosEmBuscas != NULL) free(resultadosEmBuscas);
        return;
    }

    // 2. Unificar os resultados em um único array.
    // Como múltiplas buscas podem retornar o mesmo registro, eles são coletados
    // em um único array para processamento.
    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));
    if (resultados == NULL) {
        // Lidar com falha de alocação.
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        return;
    }

    int index = 0;
    for (int i = 0; i < buscas; i++)
    {
        for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
        {
            resultados[index++] = resultadosEmBuscas[i].registrosBusca[j];
        }
        // Libera estruturas internas da busca que não são mais necessárias.
        free(resultadosEmBuscas[i].registrosBusca);
        free(resultadosEmBuscas[i].busca.campo);
        free(resultadosEmBuscas[i].busca.valor);
    }
    free(resultadosEmBuscas);

    // 3. Remover registros duplicados da lista de resultados.
    // Os resultados são ordenados por byte offset para agrupar duplicatas.
    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_offset);

    if (nRegsEncontrados > 0)
    {
        // Itera sobre o array ordenado e move apenas os registros únicos para o início.
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
                // Libera a memória do registro duplicado que será descartado.
                destroi_registro_pessoa(resultados[read_idx]->registro);
                free(resultados[read_idx]);
            }
        }
        nRegsEncontrados = write_idx; // Atualiza a contagem para o número de registros únicos.
    }

    // 4. Preparar arquivos para escrita.
    // O status de ambos os arquivos é definido como '0' (instável) antes de iniciar as modificações.
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

    // 5. Marcar registros como removidos.
    // Itera sobre os registros a serem removidos, marcando-os no arquivo de dados
    // e invalidando suas entradas correspondentes no array de índice em memória.
    remover_pessoas_e_indices(resultados, indices, &cabPessoa, nRegsEncontrados, fp, 0);

    // 6. Reescrever o arquivo de índice.
    // A função `reescrever_arquivo_indice` usa o array de índice modificado em memória
    // para criar um novo arquivo de índice, agora sem as entradas dos registros removidos.
    reescrever_arquivo_indice(nomeArquivoIndice, indices, cabPessoa.quantidadePessoas);

    // 7. Liberar a memória dos resultados da busca.
    for (int i = 0; i < nRegsEncontrados; i++)
    {
        destroi_registro_pessoa(resultados[i]->registro);
        free(resultados[i]);
    }
    free(resultados);

    // 8. Atualizar o cabeçalho do arquivo de dados e finalizar.
    // O cabeçalho é atualizado com as novas contagens de registros e o status é
    // definido de volta para '1' (estável).
    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cabPessoa.status = '1';
    cabPessoa.quantidadeRemovidos += nRegsEncontrados;
    cabPessoa.quantidadePessoas -= nRegsEncontrados;

    escreve_cabecalho_pessoa(fp, &cabPessoa);
    fflush(fp);
}
