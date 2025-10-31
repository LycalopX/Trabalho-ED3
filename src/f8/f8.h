#ifndef F8_H
#define F8_H

/**
 * @brief Função para transformar uma linha CSV do arquivo de seguidores em um RegistroSegue.
 * @param line Linha do CSV a ser parseada.
 * @param reg Ponteiro para o RegistroSegue onde os dados serão armazenados.
 */
int parse_segue_csv_line(char *line, RegistroSegue *reg);

/**
 * @brief Função para executar a funcionalidade 8: converter um arquivo CSV de seguidores em um arquivo binário.
 * @param fp_csv Ponteiro para o arquivo CSV de entrada.
 * @param fp_bin Ponteiro para o arquivo binário de saída.
 * @return 0 em caso de sucesso, ou um código de erro em caso de falha.
 */
int funcionalidade8(FILE *fp_csv, FILE *fp_bin);

#endif