#ifndef PESSOA_H
#define PESSOA_H

#include <stdio.h>
#include "../arquivos.h"

/**
 * @brief Lê o cabeçalho do arquivo de dados de pessoas.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho a ser preenchida.
 * @return 0 em sucesso, 1 em falha.
 */
int le_cabecalho_pessoa(FILE* fp, CabecalhoPessoa* cab);

/**
 * @brief Alterna o status do cabeçalho ('0'/'1') e o reescreve no arquivo.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho a ser modificada.
 */
void toggle_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab);

/**
 * @brief Escreve o cabeçalho no arquivo de dados de pessoas.
 * @param fp Ponteiro para o arquivo.
 * @param cab Ponteiro para a struct de cabeçalho com os dados a serem escritos.
 * @return 0 em sucesso, 1 em falha.
 */
int escreve_cabecalho_pessoa(FILE* fp, CabecalhoPessoa* cab);

/**
 * @brief Lê um registro de pessoa da posição atual do arquivo.
 * @param fp Ponteiro para o arquivo.
 * @param reg_out Endereço de um ponteiro que receberá o registro alocado.
 * @return 0 em sucesso, 1 em erro (EOF, falha de alocação, etc.).
 */
int le_registro_pessoa(FILE* fp, RegistroPessoa** reg_out);

/**
 * @brief Aloca e inicializa uma struct RegistroPessoa em memória.
 * @param idPessoa ID da pessoa.
 * @param nomePessoa Nome da pessoa.
 * @param idade Idade da pessoa.
 * @param nomeUsuario Nome de usuário.
 * @return Ponteiro para a nova struct alocada.
 */
RegistroPessoa* cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200]);

/**
 * @brief Libera a memória alocada para uma struct RegistroPessoa.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro_pessoa(RegistroPessoa* reg);

/**
 * @brief Cria uma cópia profunda de um registro de pessoa.
 * @param reg Ponteiro para o registro a ser copiado.
 * @return Ponteiro para a cópia do registro, ou NULL em caso de falha.
 */
RegistroPessoa* copia_registro_pessoa(RegistroPessoa* reg);

/**
 * @brief Escreve um registro de pessoa no arquivo.
 * @param fp Ponteiro para o arquivo.
 * @param reg Ponteiro para o registro com os dados a serem escritos.
 * @return 0 em sucesso, 1 em falha.
 */
int escreve_registro_pessoa(FILE* fp, RegistroPessoa* reg);

/**
 * @brief Imprime os campos de um registro de pessoa de forma formatada.
 * @param reg Ponteiro para o registro a ser impresso.
 * @return 0 em sucesso, 1 se o registro for nulo.
 */
int imprime_registro_pessoa(RegistroPessoa* reg);

/**
 * @brief Insere múltiplos registros de pessoa no arquivo de dados.
 * @param fp Ponteiro para o arquivo.
 * @param registros Array de ponteiros para os registros a serem inseridos.
 * @param nInsercoes Número de registros a inserir.
 */
void inserir_pessoas(FILE *fp, RegistroBuscaPessoa **registros, int nInsercoes);

/**
 * @brief Calcula o tamanho dos dados de um registro de pessoa (excluindo campos de controle).
 * @param reg Ponteiro para o registro.
 * @return O tamanho dos dados do registro em bytes.
 */
int calcula_tamanho_registro_pessoa(RegistroPessoa *reg);

#endif