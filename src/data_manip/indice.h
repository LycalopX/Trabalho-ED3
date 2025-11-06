#ifndef INDICE_H
#define INDICE_H

#include <stdio.h>
#include "../arquivos.h" // Corrigido para refletir a estrutura de pastas

// FUNÇÕES STRUCT INDICE
/**
 * @brief Lê o registro de cabeçalho do arquivo de índice.
 * Posiciona o cursor no início do arquivo e lê os campos 'status' e 'lixo'
 * para dentro da struct fornecida.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice que será preenchida.
 * @return Retorna 0 em caso de sucesso e 1 em caso de falha de leitura.
 */
int le_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

/**
 * @brief Alterna o status do cabeçalho do arquivo de índice entre '0' e '1' e salva no arquivo.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice que será modificada.
 */
void toggle_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

/**
 * @brief Escreve o registro de cabeçalho no arquivo de índice.
 * Posiciona o cursor no início do arquivo e escreve os campos 'status' e 'lixo'
 * a partir da struct fornecida.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso e 1 em caso de falha de escrita.
 */
int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

/**
 * @brief Lê um registro de índice do arquivo.
 * @param fp Ponteiro para o arquivo de índice.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_indice(FILE *fp, RegistroIndice **reg_out);

/**
 * @brief Intercala (merge) dois arrays de ponteiros para RegistroIndice.
 * Ambos os arrays de entrada DEVEM estar ordenados por idPessoa.
 *
 * @param indice_antigo O array original de registros de índice.
 * @param n_antigo O número de elementos no array antigo.
 * @param indice_novo O array com os novos registros de índice.
 * @param n_novo O número de elementos no array novo.
 *
 * @return Retorna um novo array, alocado dinamicamente, contendo a intercalação
 * dos dois arrays. Retorna NULL se a alocação de memória falhar.
 */
RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo);

/**
 * @brief Libera a memória alocada para um registro de índice.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro_indice(RegistroIndice *reg);

/**
 * @brief Função de comparação para bsearch, para encontrar um registro de índice por idPessoa.
 * @param key Ponteiro para o valor de idPessoa a ser buscado (int*).
 * @param elem Ponteiro para o elemento do array (RegistroIndice**).
 * @return Retorna -1 se a chave for menor, 1 se for maior, 0 se for igual.
 */
int comparar_bsearch_indice(const void *key, const void *elem);

/**
 * @brief Função de comparação para qsort, ordenando registros de busca por ByteOffset.
 * @param a Ponteiro para o primeiro elemento a ser comparado.
 * @param b Ponteiro para o segundo elemento a ser comparado.
 * @return Retorna -1 se a < b, 1 se a > b,
 */
int comparar_indices_id(const void *a, const void *b);

/**
 * @brief Carrega todo o índice na memória, retornando um array dinâmico de ponteiros para os registros.
 * @param fp Ponteiro para o arquivo de índice.
 * @param numeroRegistros Número total de registros no índice.
 * @return Retorna um array de ponteiros para RegistroIndice, ou NULL em caso de erro.
 */
RegistroIndice **carregar_indice_inteiro(FILE *fp, int numeroRegistros);

#endif