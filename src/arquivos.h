#ifndef ARQUIVOS_H
#define ARQUIVOS_H


// Estrutura genérica para representar pares de campo/valor em buscas e atualizações.
typedef struct {
    char *campo;
    char *valor;
} 
Parametro;


// Representa o cabeçalho do arquivo de dados 'pessoa.bin'.
typedef struct
{
    char status;
    int quantidadePessoas;
    int quantidadeRemovidos;

    long long proxByteOffset;

} 
CabecalhoPessoa;


// Representa um registro de dados de 'pessoa.bin'.
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
} 
RegistroPessoa;


// Estrutura auxiliar para buscas, associando um registro ao seu byte offset no arquivo.
typedef struct {

    RegistroPessoa *registro;
    long long ByteOffset;
} 
RegistroBuscaPessoa;


// Agrupa os resultados de uma operação de busca.
typedef struct {
    RegistroBuscaPessoa **registrosBusca;

    int nRegistros;
    Parametro busca;
    Parametro update;
} 
ResultadoBuscaPessoa;


// Representa o cabeçalho do arquivo de índice 'indexaPessoa.bin'.
typedef struct
{
    char status;
    char lixo[11];

} 
CabecalhoIndice;


// Representa um registro de dados de 'indexaPessoa.bin'.
typedef struct
{
    int idPessoa;
    long long byteOffset;
    
} 
RegistroIndice;


// Representa o cabeçalho do arquivo de dados 'Segue.bin'.
typedef struct
{
    char status;
    unsigned quantidadePessoas;
    unsigned proxRRN;

} 
CabecalhoSegue;


// Representa um registro de dados de 'Segue.bin'.
typedef struct
{
    char removido;
    int idPessoaQueSegue;
    int idPessoaQueESeguida;
    char dataInicioQueSegue[11];
    char dataFimQueSegue[11];
    char grauAmizade;

} 
RegistroSegue;



// Estrutura para representar uma aresta do grafo (uma pessoa que é seguida)
typedef struct Aresta
{
    char *nomeUsuarioSeguido;
    char dataInicio[11];
    char dataFim[11];
    char grauAmizade;
    struct Aresta *proxima;
} 
Aresta;


// Estrutura para representar um vértice do grafo (uma pessoa que segue)
typedef struct Vertice
{
    char *nomeUsuario;
    Aresta *adjacentes; // Ponteiro para a lista de pessoas que este vértice segue
} 
Vertice;


// Estrutura principal do Grafo
typedef struct
{
    Vertice *vertices; // Vetor de vértices
    int numVertices;
} 
Grafo;

typedef struct {
    int visitado;
    int distancia;
    int indicePai; // Índice do vértice anterior no caminho (no grafo original, é o PRÓXIMO)
    Aresta *arestaConectora; // Aresta que liga o Pai ao atual
} DadosBFS;


// Constantes para representar valores nulos e status de registros.
#define NULO_INTEIRO -1
#define NULO_DATA "$$$$$$$$$$"
#define NULO_CARACTERE '$'
#define REGISTRO_SEGUE_REMOVIDO '1'
#define REGISTRO_SEGUE_NAO_REMOVIDO '0'
#define REGISTRO_SEGUE_TAMANHO 30 * sizeof(char) 

// Constantes para mapear nomes de campos e graus de amizade.
#define AMIZADE_0 "celebridade"
#define AMIZADE_1 "amiga de minha amiga"
#define AMIZADE_2 "minha amiga"
#define CAMPO_ID "idPessoa"
#define CAMPO_IDADE "idadePessoa"
#define CAMPO_NOME "nomePessoa"
#define CAMPO_USUARIO "nomeUsuario"

// Mensagens de erro padrão.
#define FALHA_AO_ALOCAR "Falha ao alocar memória.\n"
#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"
#define FALHA_AO_EXECUTAR_FUNCIONALIDADE "Falha na execução da funcionalidade.\n"

#endif