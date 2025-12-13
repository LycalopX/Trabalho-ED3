#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grafo.h"
#include "pessoa.h"
#include "indice.h"
#include "../data_manip/segue.h"

// Estrutura auxiliar para mapear idPessoa para nomeUsuario de forma eficiente
typedef struct
{
    int idPessoa;
    char *nomeUsuario;
} IdUsuarioMap;

// --- Protótipos de Funções Auxiliares (Estáticas) ---

static RegistroSegue **carregar_todos_registros_segue(FILE *fp, int *numRegistros);
static IdUsuarioMap *criar_mapa_id_usuario(FILE *fpPessoa, FILE *fpIndice, int *numPessoas);
static int comparar_id_usuario_map(const void *a, const void *b);
static int comparar_strings_ptr(const void *a, const void *b);
static int comparar_vertices_nome(const void *a, const void *b);
static void inserir_aresta_ordenada(Vertice *vertice, Aresta *nova_aresta);
static char *get_nomeUsuario_por_id(int idPessoa, IdUsuarioMap *mapa, int numPessoas);
static Vertice *encontrar_vertice_por_nome(Grafo *grafo, const char *nomeUsuario);

Grafo *criar_grafo(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue)
{
    // Passo 1: Carregar todos os dados necessários em memória
    int numPessoas = 0;
    IdUsuarioMap *mapaIdUsuario = criar_mapa_id_usuario(fpPessoa, fpIndice, &numPessoas);
    if (!mapaIdUsuario)
        return NULL;

    int numSegue = 0;
    RegistroSegue **todosSegue = carregar_todos_registros_segue(fpSegue, &numSegue);
    if (!todosSegue)
    {
        free(mapaIdUsuario);
        return NULL;
    }

    // Passo 2: Identificar todos os vértices únicos (usuários que seguem alguém)
    char **nomesUnicos = malloc(numSegue * sizeof(char *));
    int countNomes = 0;
    for (int i = 0; i < numSegue; i++)
    {
        char *nome = get_nomeUsuario_por_id(todosSegue[i]->idPessoaQueSegue, mapaIdUsuario, numPessoas);
        if (nome)
        {
            nomesUnicos[countNomes++] = nome;
        }
    }

    // Ordena para remover duplicatas
    qsort(nomesUnicos, countNomes, sizeof(char *), comparar_strings_ptr);

    int numVerticesUnicos = 0;
    if (countNomes > 0)
    {
        numVerticesUnicos = 1;
        for (int i = 1; i < countNomes; i++)
        {
            if (strcmp(nomesUnicos[i], nomesUnicos[i - 1]) != 0)
            {
                nomesUnicos[numVerticesUnicos++] = nomesUnicos[i];
            }
        }
    }

    // Passo 3: Criar o grafo e alocar os vértices
    Grafo *grafo = malloc(sizeof(Grafo));
    if (!grafo)
        return NULL;

    grafo->numVertices = numVerticesUnicos;
    grafo->vertices = malloc(numVerticesUnicos * sizeof(Vertice));
    for (int i = 0; i < numVerticesUnicos; i++)
    {
        grafo->vertices[i].nomeUsuario = strdup(nomesUnicos[i]);
        grafo->vertices[i].adjacentes = NULL;
    }
    free(nomesUnicos);

    qsort(grafo->vertices, grafo->numVertices, sizeof(Vertice), comparar_vertices_nome);

    // Passo 4: Popular as listas de adjacência (criar as arestas)
    for (int i = 0; i < numSegue; i++)
    {
        char *nomeQueSegue = get_nomeUsuario_por_id(todosSegue[i]->idPessoaQueSegue, mapaIdUsuario, numPessoas);
        char *nomeQueESeguido = get_nomeUsuario_por_id(todosSegue[i]->idPessoaQueESeguida, mapaIdUsuario, numPessoas);

        if (!nomeQueSegue || !nomeQueESeguido)
            continue;

        Vertice *verticeOrigem = encontrar_vertice_por_nome(grafo, nomeQueSegue);
        if (!verticeOrigem)
            continue;

        Aresta *novaAresta = malloc(sizeof(Aresta));
        novaAresta->nomeUsuarioSeguido = strdup(nomeQueESeguido);
        strcpy(novaAresta->dataInicio, todosSegue[i]->dataInicioQueSegue);
        strcpy(novaAresta->dataFim, todosSegue[i]->dataFimQueSegue);
        novaAresta->grauAmizade = todosSegue[i]->grauAmizade;
        novaAresta->proxima = NULL;

        inserir_aresta_ordenada(verticeOrigem, novaAresta);
    }

    // Passo 5: Liberar memória temporária
    for (int i = 0; i < numPessoas; i++)
    {
        free(mapaIdUsuario[i].nomeUsuario);
    }
    free(mapaIdUsuario);

    for (int i = 0; i < numSegue; i++)
    {
        free(todosSegue[i]);
    }
    free(todosSegue);

    return grafo;
}

void imprimir_grafo(Grafo *grafo)
{
    if (!grafo)
        return;

    for (int i = 0; i < grafo->numVertices; i++)
    {
        Vertice *v = &grafo->vertices[i];
        Aresta *a = v->adjacentes;

        if (a == NULL)
        {
            // Se o vértice não segue ninguém, pode-se optar por imprimir apenas o nome ou nada
            // A especificação da F11 sugere pular uma linha após cada grupo de seguidores de um vértice
            // Vamos assumir que sem seguidores, a linha não deve ser pulada.

            continue;
        }

        while (a != NULL)
        {
            printf("%s, %s, %s, %s, %c\n",
                   v->nomeUsuario,
                   a->nomeUsuarioSeguido,
                   (strcmp(a->dataInicio, NULO_DATA) == 0) ? "NULO" : a->dataInicio,
                   (strcmp(a->dataFim, NULO_DATA) == 0) ? "NULO" : a->dataFim,
                   (a->grauAmizade == NULO_CARACTERE) ? 'N' : a->grauAmizade); // Simplificado para char, ideal seria tratar "NULO"
            a = a->proxima;
        }
        printf("\n"); // Pula linha entre os grupos de um mesmo seguidor
    }
}

Grafo *transpor_grafo(Grafo *grafoOriginal)
{
    if (!grafoOriginal)
        return NULL;

    // Passo 1: Coletar todos os nomes de usuários únicos (seguidores e seguidos)
    // (Esta parte é complexa e requer lógica semelhante à de `criar_grafo` para identificar todos os nós)

    printf("// Lógica de transposição a ser implementada.\n");
    // 1.a. Iterar por grafoOriginal->vertices e arestas para coletar todos os nomes únicos em uma lista temporária.
    // 1.b. Ordenar e remover duplicatas para obter a lista final de vértices para o novo grafo.

    // Passo 2: Criar o novo grafo com os vértices identificados
    Grafo *grafoTransposto = malloc(sizeof(Grafo));
    // 2.a. Alocar `grafoTransposto->vertices` e preenchê-lo com os nomes únicos.

    // Passo 3: Inverter as arestas
    for (int i = 0; i < grafoOriginal->numVertices; i++)
    {
        Vertice *u = &grafoOriginal->vertices[i];
        Aresta *aresta = u->adjacentes;
        while (aresta != NULL)
        {
            // Aresta original é: u -> v (aresta)
            // No transposto, será: v -> u

            // Encontra o vértice `v` no grafo transposto
            Vertice *v_transposto = encontrar_vertice_por_nome(grafoTransposto, aresta->nomeUsuarioSeguido);

            // Cria a nova aresta invertida
            Aresta *arestaInvertida = malloc(sizeof(Aresta));
            arestaInvertida->nomeUsuarioSeguido = strdup(u->nomeUsuario);
            strcpy(arestaInvertida->dataInicio, aresta->dataInicio);
            strcpy(arestaInvertida->dataFim, aresta->dataFim);
            arestaInvertida->grauAmizade = aresta->grauAmizade;

            // Insere a aresta invertida na lista de adjacências de `v`
            inserir_aresta_ordenada(v_transposto, arestaInvertida);

            aresta = aresta->proxima;
        }
    }

    return grafoTransposto;
}

void destruir_grafo(Grafo *grafo)
{
    if (!grafo)
        return;

    for (int i = 0; i < grafo->numVertices; i++)
    {
        Aresta *aresta_atual = grafo->vertices[i].adjacentes;
        while (aresta_atual != NULL)
        {
            Aresta *proxima = aresta_atual->proxima;
            free(aresta_atual->nomeUsuarioSeguido);
            free(aresta_atual);
            aresta_atual = proxima;
        }
        free(grafo->vertices[i].nomeUsuario);
    }
    free(grafo->vertices);
    free(grafo);
}

void encontrar_caminho_mais_curto_bfs(Grafo *grafo, const char *nomeUsuarioOrigem, const char *nomeUsuarioDestino)
{
    printf("// Lógica de BFS a ser implementada.\n");
    // 1. Encontrar os índices dos vértices de origem e destino no vetor `grafo->vertices`.
    //    Se não encontrar, o caminho não existe.

    // 2. Alocar e inicializar estruturas de controle para o BFS:
    //    - int *distancia = malloc(grafo->numVertices * sizeof(int)); // inicializar com -1 (infinito)
    //    - int *predecessor = malloc(grafo->numVertices * sizeof(int)); // inicializar com -1
    //    - Fila *fila = criar_fila();

    // 3. Configurar o vértice de origem:
    //    - distancia[indice_origem] = 0;
    //    - enfileirar(fila, indice_origem);

    // 4. Loop principal do BFS: while (!fila_vazia(fila))
    //    - vertice_atual = desenfileirar(fila);
    //    - Para cada aresta na lista de adjacências de `vertice_atual`:
    //        - Encontrar o índice do vértice vizinho.
    //        - Se `distancia[vizinho]` for -1 (não visitado):
    //            - distancia[vizinho] = distancia[vertice_atual] + 1;
    //            - predecessor[vizinho] = vertice_atual;
    //            - enfileirar(fila, vizinho);

    // 5. Após o loop, verificar `distancia[indice_destino]`.
    //    - Se for -1, não há caminho.
    //    - Se houver, reconstruir o caminho de trás para frente usando o array `predecessor`.

    // 6. Imprimir o caminho no formato exigido.

    // 7. Liberar a memória da fila, `distancia` e `predecessor`.
}




// --- Implementação das Funções Auxiliares ---

static RegistroSegue **carregar_todos_registros_segue(FILE *fp, int *numRegistros)
{
    if (fp == NULL)
        return NULL;

    fseek(fp, 0, SEEK_SET);
    CabecalhoSegue cab;
    if (le_cabecalho_segue(fp, &cab) != 0 || cab.status == '0')
    {
        *numRegistros = 0;
        return NULL;
    }
    *numRegistros = cab.quantidadePessoas;
    fseek(fp, 9, SEEK_SET);

    RegistroSegue **registros = malloc((*numRegistros) * sizeof(RegistroSegue *));
    if (registros == NULL)
        return NULL;

    int count = 0;
    while (count < *numRegistros)
    {
        registros[count] = malloc(sizeof(RegistroSegue));
        if (le_registro_segue(fp, registros[count]) == 0)
        {
            count++;
        }
    }
    return registros;
}

static IdUsuarioMap *criar_mapa_id_usuario(FILE *fpPessoa, FILE *fpIndice, int *numPessoas)
{
    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fpPessoa, &cabPessoa);
    if (cabPessoa.status == '0')
        return NULL;
    *numPessoas = cabPessoa.quantidadePessoas;

    RegistroIndice **indice = carregar_indice_inteiro(fpIndice, *numPessoas);
    if (!indice)
        return NULL;

    IdUsuarioMap *mapa = malloc(*numPessoas * sizeof(IdUsuarioMap));
    if (!mapa)
        return NULL;

    for (int i = 0; i < *numPessoas; i++)
    {
        fseek(fpPessoa, indice[i]->byteOffset, SEEK_SET);
        RegistroPessoa *p;
        if (le_registro_pessoa(fpPessoa, &p) == 0)
        {
            mapa[i].idPessoa = p->idPessoa;
            mapa[i].nomeUsuario = strdup(p->nomeUsuario);
            destroi_registro_pessoa(p);
        }
        destroi_registro_indice(indice[i]);
    }
    free(indice);

    qsort(mapa, *numPessoas, sizeof(IdUsuarioMap), comparar_id_usuario_map);
    return mapa;
}

static int comparar_id_usuario_map(const void *a, const void *b)
{
    IdUsuarioMap *mapA = (IdUsuarioMap *)a;
    IdUsuarioMap *mapB = (IdUsuarioMap *)b;
    return (mapA->idPessoa - mapB->idPessoa);
}

static char *get_nomeUsuario_por_id(int idPessoa, IdUsuarioMap *mapa, int numPessoas)
{
    IdUsuarioMap key_map = {.idPessoa = idPessoa};
    IdUsuarioMap *res = bsearch(&key_map, mapa, numPessoas, sizeof(IdUsuarioMap), comparar_id_usuario_map);
    return (res) ? res->nomeUsuario : NULL;
}

static int comparar_strings_ptr(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

static int comparar_vertices_nome(const void *a, const void *b)
{
    Vertice *vA = (Vertice *)a;
    Vertice *vB = (Vertice *)b;
    return strcmp(vA->nomeUsuario, vB->nomeUsuario);
}

static void inserir_aresta_ordenada(Vertice *vertice, Aresta *nova_aresta)
{
    // Caso especial: a lista está vazia ou o novo nó deve ser o primeiro
    if (vertice->adjacentes == NULL || strcmp(nova_aresta->nomeUsuarioSeguido, vertice->adjacentes->nomeUsuarioSeguido) < 0)
    {
        nova_aresta->proxima = vertice->adjacentes;
        vertice->adjacentes = nova_aresta;
    }
    else
    {
        Aresta *atual = vertice->adjacentes;
        // Encontra o local correto para inserção
        while (atual->proxima != NULL && strcmp(nova_aresta->nomeUsuarioSeguido, atual->proxima->nomeUsuarioSeguido) > 0)
        {
            atual = atual->proxima;
        }
        nova_aresta->proxima = atual->proxima;
        atual->proxima = nova_aresta;
    }
}

static Vertice *encontrar_vertice_por_nome(Grafo *grafo, const char *nomeUsuario)
{
    // A busca binária seria mais eficiente aqui, já que o vetor de vértices está ordenado
    for (int i = 0; i < grafo->numVertices; i++)
    {
        if (strcmp(grafo->vertices[i].nomeUsuario, nomeUsuario) == 0)
        {
            return &grafo->vertices[i];
        }
    }
    return NULL; // Não encontrado
}
