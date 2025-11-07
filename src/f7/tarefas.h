
typedef struct
{
    int idPessoa;

    // ID = 0, Idade = 1, Nome = 2, Usuario = 3
    int indiceDaRegra;

    // Caso o registro precise ser movido para o final, rrnAntigo > 0
    int rrnAntigo;
    int idPessoaAntigo;

} TarefaDeAtualizacao;

void inicializa_tarefa(TarefaDeAtualizacao *tarefas, int idPessoa, int *indexTarefa)
{

    tarefas[*indexTarefa].indiceDaRegra = 0;

    tarefas[*indexTarefa].idPessoa = idPessoa;
    tarefas[*indexTarefa].rrnAntigo = 0;
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

    if (tarefaA->rrnAntigo < tarefaB->rrnAntigo)
        return -1;
    else if (tarefaA->rrnAntigo > tarefaB->rrnAntigo)
        return 1;
    else
        return 0;
}