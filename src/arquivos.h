#ifndef ARQUIVOS_H
#define ARQUIVOS_H

// Estrutura para representar uma busca
typedef struct {
    char *campo;
    char *valor;
} Busca;

// Cabeçalho do arquivo de dados 'pessoa.bin'
typedef struct
{
    char status;
    int quantidadePessoas;
    int quantidadeRemovidos;

    long long proxByteOffset;

} CabecalhoPessoa;

// Registro de dados de 'pessoa.bin'
typedef struct
{
    char removido;
    int tamanhoRegistro;
    int idPessoa;
    int idadePessoa;

    int tamanhoNomePessoa;
    char *nomePessoa;

    int tamanhoNomeUsuario;
    char *nomeUsuario;
} RegistroPessoa;

// Cabeçalho do arquivo de índice 'indexaPessoa.bin'
typedef struct
{
    char status;
    char lixo[11];

} CabecalhoIndice;

// Registro de dados de 'indexaPessoa.bin'
typedef struct
{
    int idPessoa;
    long long byteOffset;
    
} RegistroIndice;

#endif