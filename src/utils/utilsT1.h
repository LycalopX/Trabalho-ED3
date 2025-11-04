#ifndef UTILST1_H
#define UTILST1_H

#include <stdio.h>
#include "../arquivos.h" // Corrigido para refletir a estrutura de pastas

// FUNÇÕES STRUCT PESSOA

/**
 * @brief Lê um registro de dados do arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_pessoa(FILE* fp, RegistroPessoa** reg_out);

// DEBUG
/**
 * @brief Imprime todos os registros de um arquivo de dados, incluindo os removidos.
 *
 * @param fp Ponteiro para o arquivo de dados.
 */
void imprimir_registros_raw(FILE *fp);

#endif
