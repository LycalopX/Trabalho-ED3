#ifndef F3_H
#define F3_H

#include <stdio.h>

/**
 * @brief Lê e exibe todos os registros válidos de um arquivo de dados.
 * A função assume que o arquivo já está aberto em modo de leitura binária ("rb").
 * Percorre o arquivo e imprime cada registro não removido.
 * Não fecha o ponteiro de arquivo.
 * 
 * @param fp Ponteiro para o arquivo de dados a ser lido.
 */
void funcionalidade3(FILE *fp);

#endif
