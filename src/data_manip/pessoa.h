#ifndef PESSOA_H
#define PESSOA_H

#include <stdio.h>
#include "../arquivos.h"

// FUNÇÕES STRUCT PESSOA
/**
 * @brief Lê o registro de cabeçalho do arquivo de dados.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a struct que será preenchida com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int le_cabecalho_pessoa(FILE* fp, CabecalhoPessoa* cab);

/**
 * @brief Alterna o status do cabeçalho do arquivo de dados entre '0' e '1' e salva no arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a struct CabecalhoPessoa que será modificada.
 */
void toggle_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab);

/**
 * @brief Escreve o registro de cabeçalho no arquivo de dados.
 * @param fp Ponteiro para o arquivo de dados.
 * @param cab Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int escreve_cabecalho_pessoa(FILE* fp, CabecalhoPessoa* cab);

/**
 * @brief Lê um registro de dados do arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_pessoa(FILE* fp, RegistroPessoa** reg_out);

/**
 * @brief Construtor da struct RegistroPessoa.
 * @param idPessoa ID da pessoa.
 * @param nomePessoa Nome da pessoa.
 * @param idade Idade da pessoa.
 * @param nomeUsuario Nome de usuário da pessoa.
 * @return Retorna um ponteiro para a nova struct RegistroPessoa alocada.
 */
RegistroPessoa* cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200]);

/**
 * @brief Libera a memória alocada para um registro de pessoa.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro_pessoa(RegistroPessoa* reg);

/**
 * @brief Escreve um registro de dados completo no arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg Ponteiro para a struct contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso, 1 em caso de falha.
 */
int escreve_registro_pessoa(FILE* fp, RegistroPessoa* reg);

/**
 * @brief Imprime os campos de um registro de pessoa na saída padrão.
 * @param reg Ponteiro para a struct a ser impressa.
 * @return Retorna 0 em caso de sucesso, 1 se o registro for nulo.
 */
int imprime_registro_pessoa(RegistroPessoa* reg);

#endif