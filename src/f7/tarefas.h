
typedef struct
{
    int idPessoa;

    // ID = 0, Idade = 1, Nome = 2, Usuario = 3
    int indiceDaRegra;

    // Guarda o byteoffset original do registro
    long long rrn;
    int idPessoaAntigo;

} TarefaDeAtualizacao;

void inicializa_tarefa(TarefaDeAtualizacao *tarefas, int idPessoa, long long rrn, int *indexTarefa)
{

    tarefas[*indexTarefa].indiceDaRegra = 0;

    tarefas[*indexTarefa].idPessoa = idPessoa;
    tarefas[*indexTarefa].rrn = rrn;
    tarefas[*indexTarefa].idPessoaAntigo = 0;
}

int comparar_tarefas_por_id(const void *a, const void *b)
{
    TarefaDeAtualizacao *tarefaA = (TarefaDeAtualizacao *)a;
    TarefaDeAtualizacao *tarefaB = (TarefaDeAtualizacao *)b;

    return (tarefaA->idPessoa - tarefaB->idPessoa);
}

int comparar_tarefas_por_byteoffset(const void *a, const void *b)
{
    TarefaDeAtualizacao *tarefaA = (TarefaDeAtualizacao *)a;
    TarefaDeAtualizacao *tarefaB = (TarefaDeAtualizacao *)b;

    if (tarefaA->rrn < tarefaB->rrn)
        return -1;
    else if (tarefaA->rrn > tarefaB->rrn)
        return 1;
    else
        return 0;
}