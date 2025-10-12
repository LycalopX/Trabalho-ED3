#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "../arquivos.h" // Corrigido para refletir a estrutura de pastas

/**
 * @brief Lê o registro de cabeçalho do arquivo de dados.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a struct que será preenchida com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int le_cabecalho_pessoa(FILE* fp, CabecalhoPessoa* cab);

/**
 * @brief Escreve o registro de cabeçalho no arquivo de dados.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int escreve_cabecalho_pessoa(FILE* fp, CabecalhoPessoa* cab);

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
 * @brief Escreve o registro de cabeçalho no arquivo de índice.
 * Posiciona o cursor no início do arquivo e escreve os campos 'status' e 'lixo'
 * a partir da struct fornecida.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso e 1 em caso de falha de escrita.
 */
int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

/**
 * @brief Lê um registro de dados do arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_pessoa(FILE* fp, RegistroPessoa** reg_out);

/**
 * @brief Libera a memória alocada para um registro de pessoa.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro(RegistroPessoa* reg);

/**
 * @brief Escreve um registro de dados completo no arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int escreve_registro_pessoa(FILE* fp, RegistroPessoa* reg);

/**
 * @brief Imprime os campos de um registro de pessoa na saída padrão.
 * @param reg Ponteiro para a struct a ser impressa.
 * @return Retorna 0 em caso de sucesso, 1 se o registro for nulo.
 */
int imprime_registro_pessoa(RegistroPessoa* reg);

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

#endif