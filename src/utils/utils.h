#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "../arquivos.h" // Corrigido para refletir a estrutura de pastas

// Cabeçalhos
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
 * @brief Alterna o status do cabeçalho do arquivo de índice entre '0' e '1' e salva no arquivo.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice que será modificada.
 */
void toggle_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

/**
 * @brief Lê o registro de cabeçalho do arquivo de índice.
 * Posiciona o cursor no início do arquivo e lê os campos 'status' e 'lixo'
 * para dentro da struct fornecida.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice que será preenchida.
 * @return Retorna 0 em caso de sucesso e 1 em caso de falha de leitura.
 */
int le_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

/**
 * @brief Escreve o registro de cabeçalho no arquivo de índice.
 * Posiciona o cursor no início do arquivo e escreve os campos 'status' e 'lixo'
 * a partir da struct fornecida.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para a struct CabecalhoIndice contendo os dados a serem escritos.
 * @return Retorna 0 em caso de sucesso e 1 em caso de falha de escrita.
 */
int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab);

// Indices
/**
 * @brief Lê um registro de índice do arquivo.
 * @param fp Ponteiro para o arquivo de índice.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_indice(FILE *fp, RegistroIndice **reg_out);

/**
 * @brief Libera a memória alocada para um registro de índice.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro_indice(RegistroIndice *reg);

/**
 * @brief Lê um registro de índice do arquivo.
 * @param fp Ponteiro para o arquivo de índice.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
RegistroIndice **carregar_indice_inteiro(FILE *fp, int numeroRegistros);

// Registros
/**
 * @brief Lê um registro de dados do arquivo.
 * @param fp Ponteiro para o arquivo de dados.
 * @param reg_out Endereço de um ponteiro que receberá a struct alocada com os dados.
 * @return Retorna 0 em caso de sucesso, 1 em caso de erro (fim de arquivo, falha de memória, etc).
 */
int le_registro_pessoa(FILE* fp, RegistroPessoa** reg_out);

// Criar @brief depois
RegistroPessoa* cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200]);

/**
 * @brief Libera a memória alocada para um registro de pessoa.
 * @param reg Ponteiro para o registro a ser destruído.
 */
void destroi_registro(RegistroPessoa* reg);

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

// Correção
/**
 * @brief Lê um arquivo binário e imprime na tela uma representação de seu conteúdo.
 * Utilizada para verificação e comparação de arquivos nos testes automatizados (run.codes).
 * O arquivo deve ser fechado (fclose) antes de a função ser chamada.
 * @param nomeArquivoBinario String contendo o nome do arquivo a ser lido.
 */
void binarioNaTela(char *nomeArquivoBinario);

/**
 * @brief Lê uma string da entrada padrão, tratando casos especiais.
 * A função é capaz de ler strings delimitadas por aspas duplas (removendo as aspas)
 * ou a palavra-chave "NULO", que resulta em uma string vazia.
 * @param str Ponteiro para o buffer onde a string lida será armazenada.
 */
void scan_quote_string(char *str);


/**
 * @brief Lê um arquivo binário de dados (pessoa.bin) e imprime seu conteúdo
 * de forma estruturada e legível, campo por campo.
 * @param nomeArquivoDados O nome do arquivo a ser lido e impresso.
 */
void debug_imprime_arquivo_pessoa(const char *nomeArquivoDados);

/**
 * @brief Lê um arquivo binário de índice (indexaPessoa.bin) e imprime seu
 * conteúdo de forma estruturada e legível, campo por campo.
 * @param nomeArquivoIndice O nome do arquivo a ser lido e impresso.
 */
void debug_imprime_arquivo_indice(const char *nomeArquivoIndice);

/**
 * @brief Realoca um array genérico para uma nova capacidade baseada na razão áurea.
 *
 * Esta função é um wrapper seguro para realloc que lida com o crescimento
 * de um array genérico (void*).
 *
 * @param ptr O ponteiro para o array atual (ou NULL para alocar pela primeira vez).
 * @param p_current_capacity Um ponteiro para a variável que armazena a 
 * capacidade *atual* (número de elementos que o array
 * *pode* conter). Este valor será *atualizado* para
 * a nova capacidade se a realocação for bem-sucedida.
 * @param elem_size O tamanho (em bytes) de um único elemento no array (usar sizeof()).
 *
 * @return Um ponteiro para o novo bloco de memória, ou NULL se a alocação falhar.
 * Se falhar, o ponteiro original (ptr) NÃO é liberado e 
 * *p_current_capacity não é alterado.
 */
void realloc_golden(void **ptr, size_t* p_current_capacity, size_t elem_size);

/**
 * @brief Imprime todos os registros de um arquivo de dados, incluindo os removidos.
 * 
 * @param fp Ponteiro para o arquivo de dados.
 */
void imprimir_registros_raw(FILE *fp);

/**
 * @brief Intercala (merge) dois arrays de ponteiros para RegistroIndice.
 * Ambos os arrays de entrada DEVEM estar ordenados por idPessoa.
 * 
 * @param indice_antigo O array original de registros de índice.
 * @param n_antigo O número de elementos no array antigo.
 * @param indice_novo O array com os novos registros de índice.
 * @param n_novo O número de elementos no array novo.
 * 
 * @return Retorna um novo array, alocado dinamicamente, contendo a intercalação
 * dos dois arrays. Retorna NULL se a alocação de memória falhar.
 */
RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo);

#endif