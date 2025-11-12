
#include "../arquivos.h"

typedef struct {
    RegistroPessoa *registro;
    int idOriginal;
    int tamanhoRegistroOriginal;

    long long ByteOffset;
    char flagNovoByteOffset;
    int idPessoaNovo;
    int indiceDaRegra;

} Atualizacao;

void inicializa_atualizacao(Atualizacao *atualizacao, RegistroPessoa *registro, long long byteOffset, int indiceDaRegra) {
    atualizacao->registro = copia_registro_pessoa(registro);
    atualizacao->idOriginal = registro->idPessoa;
    atualizacao->tamanhoRegistroOriginal = registro->tamanhoRegistro;
    atualizacao->ByteOffset = byteOffset;
    atualizacao->flagNovoByteOffset = '0';
    atualizacao->idPessoaNovo = -1;
    atualizacao->indiceDaRegra = indiceDaRegra;
    
}

int comparar_atualizacao_por_id(const void *a, const void *b)
{
    Atualizacao *tarefaA = (Atualizacao *)a;
    Atualizacao *tarefaB = (Atualizacao *)b;

    return (tarefaA->idOriginal - tarefaB->idOriginal);
}

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