
#include "../arquivos.h"

// Estrutura que representa uma tarefa de atualização de registro.
typedef struct {
    RegistroPessoa *registro;
    int idOriginal;
    int tamanhoRegistroOriginal;

    long long ByteOffset;
    char flagNovoByteOffset;
    int idPessoaNovo;
    int indiceDaRegra;

} Atualizacao;

// Inicializa uma struct Atualizacao com os dados de um registro e seu offset.
void inicializa_atualizacao(Atualizacao *atualizacao, RegistroPessoa *registro, long long byteOffset, int indiceDaRegra) {
    atualizacao->registro = copia_registro_pessoa(registro);
    atualizacao->idOriginal = registro->idPessoa;
    atualizacao->tamanhoRegistroOriginal = registro->tamanhoRegistro;
    atualizacao->ByteOffset = byteOffset;
    atualizacao->flagNovoByteOffset = '0';
    atualizacao->idPessoaNovo = -1;
    atualizacao->indiceDaRegra = indiceDaRegra;
    
}

// Função de comparação para qsort, utilizada para ordenar atualizações por ID original.
int comparar_atualizacao_por_id(const void *a, const void *b)
{
    Atualizacao *tarefaA = (Atualizacao *)a;
    Atualizacao *tarefaB = (Atualizacao *)b;

    return (tarefaA->idOriginal - tarefaB->idOriginal);
}

// Função de comparação para qsort, utilizada para ordenar atualizações por byte offset.
int comparar_atualizacao_por_byteoffset(const void *a, const void *b)
{
    Atualizacao *tarefaA = (Atualizacao *)a;
    Atualizacao *tarefaB = (Atualizacao *)b;

    if (tarefaA->ByteOffset < tarefaB->ByteOffset)
        return -1;
    else if (tarefaA->ByteOffset > tarefaB->ByteOffset)
        return 1;
    else
        return 0;
}