#ifndef F5_H
#define F5_H

#include <stdio.h>

/**
 * @brief Remove logicamente registros de um arquivo de dados com base em critérios de busca.
 * A função utiliza a funcionalidade 4 para encontrar os registros, os marca como removidos
 * e atualiza o cabeçalho do arquivo de dados. Ao final, exibe os arquivos de dados e
 * de índice atualizados.
 * 
 * @param fp Ponteiro para o arquivo de dados (aberto em "rb+").
 * @param fpIndice Ponteiro para o arquivo de índice (aberto em "rb+").
 * @param buscas Número de critérios de busca a serem lidos da entrada padrão.
 * @param nomeArquivoDados Nome do arquivo de dados, para passar ao binarioNaTela.
 * @param nomeArquivoIndice Nome do arquivo de índice, para passar ao binarioNaTela.
 */
void funcionalidade5(FILE *fp, FILE *fpIndice, int buscas);

#endif
