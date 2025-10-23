#ifndef F1_H
#define F1_H

#include <stdio.h>

/**
 * @brief Inicializa um arquivo de índice primário, escrevendo o cabeçalho.
 * A função assume que o arquivo já está aberto. Ela escreve o cabeçalho e
 * depois chama binarioNaTela para exibir o resultado.
 * A função NÃO fecha o ponteiro do arquivo.
 * 
 * @param fp Ponteiro para o arquivo de índice a ser inicializado (aberto em "wb").
 * @param caminhoCompleto O nome/caminho do arquivo, necessário para a chamada de binarioNaTela.
 */
void funcionalidade1(FILE *fp);

#endif