#ifndef F8_H
#define F8_H

/**
 * @brief Função auxiliar para transformar uma linha CSV do arquivo de seguidores em um RegistroSegue.
 * @param line Linha do CSV a ser parseada.
 * @param reg Ponteiro para o RegistroSegue onde os dados serão armazenados.
 * @return Retorna 0 em caso de sucesso, ou um código de erro em caso de falha no parsing.
 */
int parse_segue_csv_line(char *line, RegistroSegue *reg);

/**
 * @brief Implementa a funcionalidade 8: converte um arquivo CSV de seguidores em um arquivo binário.
 * @param fp_csv Ponteiro para o arquivo CSV de entrada.
 * @param fp_bin Ponteiro para o arquivo binário de saída.
 * @return Retorna 0 em caso de sucesso, ou um código de erro em caso de falha.
 */
int funcionalidade8(FILE *fp_csv, FILE *fp_bin);

#endif