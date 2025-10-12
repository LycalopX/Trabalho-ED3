#ifndef F2_H
#define F2_H

/**
 * @brief Lê registros de um arquivo .csv e os insere em um arquivo de dados binário ('pessoa.bin').
 * Para cada registro inserido no arquivo de dados, uma entrada correspondente (idPessoa e byteOffset)
 * é adicionada ao arquivo de índice primário ('indexaPessoa.bin'). Os cabeçalhos de ambos os
 * arquivos são inicializados e atualizados para refletir um estado consistente ('1') ao final da operação.
 * Ao final, exibe o conteúdo de ambos os arquivos binários gerados usando a função binarioNaTela.
 * @param nomeArquivoCSV O nome do arquivo de entrada no formato .csv.
 * @param nomeArquivoDados O nome do arquivo de dados binário a ser criado/sobrescrito.
 * @param nomeArquivoIndice O nome do arquivo de índice primário que será populado.
 */
void funcionalidade2(char* nomeArquivoCSV, char* nomeArquivoDados, char* nomeArquivoIndice);

/**
 * @brief Processa uma única linha do arquivo .csv e preenche a struct RegistroPessoa.
 * Esta função substitui o uso de strtok, tratando corretamente os campos nulos.
 *
 * @param line A string contendo a linha lida do .csv.
 * @param reg Ponteiro para a struct RegistroPessoa a ser preenchida.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int parse_csv_line(char *line, RegistroPessoa *reg);

#endif // F2_H