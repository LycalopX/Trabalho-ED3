#ifndef F2_H
#define F2_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Implementa a funcionalidade 2: lê registros de um arquivo CSV e os insere nos arquivos de dados e de índice.
 * A função assume que os arquivos já estão abertos. Ela atualiza os cabeçalhos,
 * escreve os registros e, ao final, chama binarioNaTela para exibir os resultados.
 * Não fecha os ponteiros de arquivo.
 * 
 * @param fp_csv Ponteiro para o arquivo de entrada CSV (aberto em modo "r").
 * @param fp_data Ponteiro para o arquivo de dados binário (aberto em modo "wb").
 * @param fp_index Ponteiro para o arquivo de índice (aberto em modo "rb+").
 * @param nomeArquivoDados Nome do arquivo de dados, para passar ao binarioNaTela.
 * @param nomeArquivoIndice Nome do arquivo de índice, para passar ao binarioNaTela.
 */
void funcionalidade2(FILE *fp_csv, FILE *fp_data, FILE *fp_index, const char *nomeArquivoDados, const char *nomeArquivoIndice);

/**
 * @brief Processa uma única linha de um arquivo CSV de pessoas e preenche a struct RegistroPessoa.
 * Esta função substitui o uso de strtok, tratando corretamente os campos nulos.
 *
 * @param line A string contendo a linha lida do CSV.
 * @param reg Ponteiro para a struct RegistroPessoa a ser preenchida.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int parse_pessoa_csv_line(char *line, RegistroPessoa *reg);

#endif // F2_H