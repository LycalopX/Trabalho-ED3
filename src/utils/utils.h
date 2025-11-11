#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stddef.h> // Para size_t
#include "../arquivos.h"

/**
 * @brief Lê um arquivo binário e imprime na tela uma representação de seu conteúdo.
 *
 * Esta função é utilizada para verificação e comparação de arquivos em testes automatizados.
 * Ela calcula uma soma de verificação (checksum) do conteúdo do arquivo e a imprime.
 * O arquivo deve ser fechado (fclose) antes de a função ser chamada.
 *
 * @param nomeArquivoBinario String contendo o nome do arquivo a ser lido.
 */
void binarioNaTela(char *nomeArquivoBinario);

/**
 * @brief Lê uma string da entrada padrão, tratando casos especiais como aspas e "NULO".
 *
 * A função é capaz de ler strings delimitadas por aspas duplas (removendo as aspas)
 * ou a palavra-chave "NULO", que resulta em uma string vazia. Ignora espaços em branco
 * antes do início da string.
 *
 * @param str Ponteiro para o buffer onde a string lida será armazenada.
 */
void scan_quote_string(char *str);

// --- Funções de Comparação para Busca e Ordenação ---

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por ByteOffset.
 * @param a Ponteiro para o primeiro elemento (RegistroBuscaPessoa**).
 * @param b Ponteiro para o segundo elemento (RegistroBuscaPessoa**).
 * @return Retorna -1 se a < b, 1 se a > b, 0 se a == b.
 */
int comparar_registros_busca_offset(const void *a, const void *b);

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por idPessoa.
 * @param a Ponteiro para o primeiro elemento (RegistroBuscaPessoa**).
 * @param b Ponteiro para o segundo elemento (RegistroBuscaPessoa**).
 * @return Retorna a diferença entre os IDs (regA->idPessoa - regB->idPessoa).
 */
int comparar_registros_busca_id(const void *a, const void *b);

// --- Gerenciamento de Array Expandível ---

/**
 * @brief Realoca um array genérico para uma nova capacidade baseada na razão áurea.
 *
 * Esta função é um wrapper seguro para realloc que lida com o crescimento
 * de um array genérico (void*). A nova capacidade é calculada usando a razão áurea
 * para um crescimento eficiente. Inclui checagem de overflow.
 *
 * @param ptr O ponteiro para o array atual (ou NULL para alocar pela primeira vez).
 * @param p_current_capacity Um ponteiro para a variável que armazena a
 *        capacidade *atual* (número de elementos que o array *pode* conter).
 *        Este valor será *atualizado* para a nova capacidade se a realocação for bem-sucedida.
 * @param elem_size O tamanho (em bytes) de um único elemento no array (usar sizeof()).
 */
void realloc_golden(void **ptr, size_t* p_current_capacity, size_t elem_size);

/**
 * @brief Libera a memória de um array de resultados de busca, incluindo todas as subestruturas.
 *
 * Percorre o array de ResultadoBuscaPessoa e libera toda a memória alocada
 * dinamicamente para os registros, parâmetros de busca e atualização.
 *
 * @param resultados O array de ponteiros para ResultadoBuscaPessoa a ser liberado.
 * @param nBuscas O número de elementos no array.
 */
void liberar_resultados_busca_inteira(ResultadoBuscaPessoa *resultados, int nBuscas);

/**
 * @brief Libera a memória de um único ResultadoBuscaPessoa, incluindo todas as subestruturas.
 *
 * Percorre o ResultadoBuscaPessoa e libera toda a memória alocada
 * dinamicamente para os registros, parâmetros de busca e atualização.
 *
 * @param resultado Ponteiro para o ResultadoBuscaPessoa a ser liberado.
 */
void liberar_resultado_busca(ResultadoBuscaPessoa *resultado);

// --- Funções de Debug ---

/**
 * @brief Imprime todos os registros de um arquivo de dados de pessoa, incluindo os removidos, na saída padrão.
 *
 * Esta função é útil para depuração, mostrando o conteúdo bruto do arquivo,
 * incluindo informações de cabeçalho e detalhes de cada registro.
 *
 * @param fp Ponteiro para o arquivo de dados de pessoa.
 */
void imprimir_registros_raw(FILE *fp);

/**
 * @brief Imprime todos os registros de um arquivo de dados de pessoa, incluindo os removidos, em um arquivo de saída.
 *
 * Similar a `imprimir_registros_raw`, mas direciona a saída para um arquivo especificado.
 *
 * @param fp Ponteiro para o arquivo de dados de pessoa.
 * @param nome_arquivo_saida Nome do arquivo onde a saída será escrita.
 */
void imprimir_registros_raw_em_arquivo(FILE *fp, char *nome_arquivo_saida);

/**
 * @brief Marca registros de pessoa como removidos no arquivo de dados e atualiza o índice em memória.
 *
 * Esta função é usada para realizar a remoção lógica de registros. Ela busca os registros
 * especificados, marca-os como removidos no arquivo de dados e atualiza o array de índice
 * em memória para refletir essa remoção.
 *
 * @param resultados Array de ponteiros para os registros encontrados na busca.
 * @param indices Array de ponteiros para os registros de índice correspondentes.
 * @param cabPessoa Cabeçalho do arquivo de dados de pessoas.
 * @param nRegsEncontrados Número de registros encontrados na busca.
 * @param fp Ponteiro para o arquivo de dados.
 * @param flagUpdate Flag que indica se a remoção faz parte de uma operação de atualização.
 */
void remover_pessoas_e_indices(RegistroBuscaPessoa **resultados, RegistroIndice **indices, CabecalhoPessoa *cabPessoa, int nRegsEncontrados, FILE *fp, int flagUpdate);

#endif