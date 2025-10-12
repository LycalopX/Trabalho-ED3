#ifndef FUNC_H
#define FUNC_H

/**
 * @brief Cria um novo arquivo de índice primário ('indexaPessoa.bin').
 * A função inicializa o arquivo apenas com o registro de cabeçalho,
 * definindo o status como '1' (consistente) e preenchendo o restante com lixo ('$'),
 * conforme especificado no documento do trabalho.
 * Ao final, exibe o conteúdo do arquivo binário gerado na tela.
 * @param nomeArquivoIndice O nome do arquivo de índice a ser criado.
 */
void funcionalidade1(char* nomeArquivoIndice);

#endif