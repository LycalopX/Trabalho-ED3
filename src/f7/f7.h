#ifndef F7_H
#define F7_H

/**
 * @brief Função para executar a funcionalidade 8: converter um arquivo CSV de seguidores em um arquivo binário.
 * @param fp_csv Ponteiro para o arquivo CSV de entrada.
 * @param fp_bin Ponteiro para o arquivo binário de saída.
 * @return 0 em caso de sucesso, ou um código de erro em caso de falha.
 */
int funcionalidade7(FILE *fp, FILE *fpIndice, int buscas);

#endif