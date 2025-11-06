#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stddef.h> // Para size_t
#include "../arquivos.h" // Corrigido para refletir a estrutura de pastas

// Correção
/**
 * @brief Lê um arquivo binário e imprime na tela uma representação de seu conteúdo.
 * Utilizada para verificação e comparação de arquivos nos testes automatizados (run.codes).
 * O arquivo deve ser fechado (fclose) antes de a função ser chamada.
 * @param nomeArquivoBinario String contendo o nome do arquivo a ser lido.
 */
void binarioNaTela(char *nomeArquivoBinario);

/**
 * @brief Lê uma string da entrada padrão, tratando casos especiais.
 * A função é capaz de ler strings delimitadas por aspas duplas (removendo as aspas)
 * ou a palavra-chave "NULO", que resulta em uma string vazia.
 * @param str Ponteiro para o buffer onde a string lida será armazenada.
 */
void scan_quote_string(char *str);

// FUNÇÕES DE COMPARAÇÃO PARA BUSCA
/**
 * @brief Função de comparação para qsort, ordenando registros de busca por ByteOffset.
 * @param a Ponteiro para o primeiro elemento a ser comparado.
 * @param b Ponteiro para o segundo elemento a ser comparado.
 * @return Retorna -1 se a < b, 1 se a > b,
 */
int comparar_registros_busca_offset(const void *a, const void *b);

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por idPessoa.
 * @param a Ponteiro para o primeiro elemento a ser comparado.
 * @param b Ponteiro para o segundo elemento a ser comparado.
 * @return Retorna a diferença entre os ids (regA->idPessoa - regB->idPessoa).
 */
int comparar_registros_busca_id(const void *a, const void *b);

// GERENCIAMENTO DE ARRAY EXPANDÍVEL
/**
 * @brief Realoca um array genérico para uma nova capacidade baseada na razão áurea.
 *
 * Esta função é um wrapper seguro para realloc que lida com o crescimento
 * de um array genérico (void*).
 *
 * @param ptr O ponteiro para o array atual (ou NULL para alocar pela primeira vez).
 * @param p_current_capacity Um ponteiro para a variável que armazena a
 * capacidade *atual* (número de elementos que o array
 * *pode* conter). Este valor será *atualizado* para
 * a nova capacidade se a realocação for bem-sucedida.
 * @param elem_size O tamanho (em bytes) de um único elemento no array (usar sizeof()).
 *
 * @return Um ponteiro para o novo bloco de memória, ou NULL se a alocação falhar.
 * Se falhar, o ponteiro original (ptr) NÃO é liberado e
 * *p_current_capacity não é alterado.
 */
void realloc_golden(void **ptr, size_t* p_current_capacity, size_t elem_size);

// DEBUG
/**
 * @brief Imprime todos os registros de um arquivo de dados, incluindo os removidos.
 *
 * @param fp Ponteiro para o arquivo de dados.
 */
void imprimir_registros_raw(FILE *fp);

#endif