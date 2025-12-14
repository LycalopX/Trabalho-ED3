#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grafo.h"
#include "../data_manip/pessoa.h"
#include "../data_manip/indice.h"
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
static void ordenar_indices_dos_vertices_por_nome(int *indices, int n, Grafo *grafo);
static int obter_indice_vertice_por_nome(Grafo *grafo, const char *nome);

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
            char grauAmizade[2];
            grauAmizade[0] = a->grauAmizade;
            grauAmizade[1] = '\0';

            printf("%s, %s, %s, %s, %s\n",
                   v->nomeUsuario,
                   a->nomeUsuarioSeguido,
                   (strcmp(a->dataInicio, NULO_DATA) == 0) ? "NULO" : a->dataInicio,
                   (strcmp(a->dataFim, NULO_DATA) == 0) ? "NULO" : a->dataFim,
                   (a->grauAmizade == NULO_CARACTERE) ? "NULO" : grauAmizade);
            a = a->proxima;
        }
        printf("\n"); // Pula linha entre os grupos de um mesmo seguidor
    }
}

Grafo *transpor_grafo(Grafo *grafoOriginal)
{
    if (!grafoOriginal)
        return NULL;

    // Alocar o grafo transposto
    Grafo *grafoT = malloc(sizeof(Grafo));
    if (!grafoT)
        return NULL; // Verificação

    // Copiar o número de vértices e alocar o vetor de vértices
    grafoT->numVertices = grafoOriginal->numVertices;
    grafoT->vertices = malloc(sizeof(Vertice) * grafoOriginal->numVertices);

    // Inicializamos todos os vértices sem arestas primeiro
    for (int i = 0; i < grafoOriginal->numVertices; i++)
    {
        grafoT->vertices[i].nomeUsuario = strdup(grafoOriginal->vertices[i].nomeUsuario);
        grafoT->vertices[i].adjacentes = NULL;
    }

    // Inverter as Arestas
    for (int i = 0; i < grafoOriginal->numVertices; i++)
    {
        Vertice *u_orig = &grafoOriginal->vertices[i];
        Aresta *arestaAtual = u_orig->adjacentes;

        // Percorremos a lista de quem U segue
        while (arestaAtual != NULL)
        {
            // Se U segue V, no transposto V segue U

            char *nomeV = arestaAtual->nomeUsuarioSeguido;
            Vertice *v_transposto = encontrar_vertice_por_nome(grafoT, nomeV);

            if (v_transposto)
            {
                // Criar nova aresta invertida
                Aresta *novaAresta = malloc(sizeof(Aresta));

                // O destino agora é o U
                novaAresta->nomeUsuarioSeguido = strdup(u_orig->nomeUsuario);

                // Copiar os dados da aresta original
                strcpy(novaAresta->dataInicio, arestaAtual->dataInicio);
                strcpy(novaAresta->dataFim, arestaAtual->dataFim);
                novaAresta->grauAmizade = arestaAtual->grauAmizade;

                // Inserir ordenado na lista de V
                inserir_aresta_ordenada(v_transposto, novaAresta);
            }

            arestaAtual = arestaAtual->proxima;
        }
    }

    return grafoT;
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

void encontrar_caminhos_curtos_bfs(Grafo *grafoTransposto, char *nomeCelebridade, Grafo *grafoOriginal)
{
    // Validações
    if (!grafoTransposto || !nomeCelebridade)
        return;

    int n = grafoTransposto->numVertices;

    // Encontrar o índice da celebridade
    int indiceCeleb = obter_indice_vertice_por_nome(grafoTransposto, nomeCelebridade);

    // Celebridade não encontrada
    if (indiceCeleb == -1)
    {
        printf("Falha na execução da funcionalidade.\n");
        return;
    }

    // Inicializar estruturas da BFS
    DadosBFS *info = calloc(n, sizeof(DadosBFS));
    for (int i = 0; i < n; i++)
    {
        info[i].distancia = -1;
        info[i].visitado = 0;
        info[i].indicePai = -1;
    }

    // Fila para BFS
    int *fila = malloc(sizeof(int) * n);
    int inicio = 0;
    int fim = 0;

    // Configurar o ponto de partida (Celebridade)
    info[indiceCeleb].visitado = 1;
    info[indiceCeleb].distancia = 0;
    fila[fim] = indiceCeleb;
    fim++;

    // Loop Principal da BFS
    while (inicio < fim)
    {
        int u_idx = fila[inicio];
        inicio++;
        Vertice *u = &grafoTransposto->vertices[u_idx];

        Aresta *aresta = u->adjacentes;
        while (aresta != NULL)
        {

            // Se tem texto E esse texto NÃO é lixo, então acabou
            if (strlen(aresta->dataFim) > 0 && aresta->dataFim[0] != '$')
            {
                aresta = aresta->proxima;
                continue;
            }

            // Encontrar índice do vizinho (busca linear)
            int v_idx = obter_indice_vertice_por_nome(grafoTransposto, aresta->nomeUsuarioSeguido);

            if (v_idx != -1 && !info[v_idx].visitado)
            {
                info[v_idx].visitado = 1;
                info[v_idx].distancia = info[u_idx].distancia + 1;
                info[v_idx].indicePai = u_idx;
                info[v_idx].arestaConectora = aresta;
                fila[fim++] = v_idx;
            }
            aresta = aresta->proxima;
        }
    }

    // Preparar para imprimir ordenado
    int *indicesOrdenados = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
        indicesOrdenados[i] = i;

    ordenar_indices_dos_vertices_por_nome(indicesOrdenados, n, grafoTransposto);

    // Exibir resultados
    for (int k = 0; k < n; k++)
    {
        int i = indicesOrdenados[k]; // Usa o índice após a ordenação

        // Pula a própria celebridade
        if (i == indiceCeleb)
            continue;

        if (info[i].visitado != 0)
        {
            // Reconstrução do caminho
            int atual = i;
            while (atual != indiceCeleb)
            {
                int proximo = info[atual].indicePai;
                Aresta *a = info[atual].arestaConectora;

                // Se for vazio ou lixo ($), imprime NULO
                char *dtFim;
                if (strlen(a->dataFim) == 0 || a->dataFim[0] == '$')
                {
                    dtFim = "NULO";
                }
                else
                {
                    dtFim = a->dataFim;
                }

                char grauStr[10]; // Buffer para guardar o texto final

                // Se for '$' é NULO.
                if (a->grauAmizade == '$')
                {
                    strcpy(grauStr, "NULO");
                }
                else
                {
                    // Formatamos como caractere
                    sprintf(grauStr, "%c", a->grauAmizade);
                }

                // NomeQuemSegue, NomeSeguido, DataIni, DataFim, Grau
                printf("%s, %s, %s, %s, %s\n",
                       grafoTransposto->vertices[atual].nomeUsuario,
                       grafoTransposto->vertices[proximo].nomeUsuario,
                       a->dataInicio,
                       dtFim,
                       grauStr);

                atual = proximo;
            }
            printf("\n"); // Pula linha entre os caminhos
        }
    }

    // Limpeza
    free(indicesOrdenados);
    free(info);
    free(fila);
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

    int total_records_in_file = cab.quantidadePessoas;
    fseek(fp, 9, SEEK_SET); // Skip header

    RegistroSegue **registros_validos = malloc(total_records_in_file * sizeof(RegistroSegue *));
    if (registros_validos == NULL)
        return NULL;

    int valid_records_count = 0;

    for (int i = 0; i < total_records_in_file; i++)
    {
        RegistroSegue *temp_reg = malloc(sizeof(RegistroSegue));
        if (temp_reg == NULL)
        {
            for (int j = 0; j < valid_records_count; j++)
                free(registros_validos[j]);
            free(registros_validos);
            return NULL;
        }

        int result = le_registro_segue(fp, temp_reg);

        if (result == 0)
        { // Success, valid record
            registros_validos[valid_records_count++] = temp_reg;
        }
        else if (result == LE_REGISTRO_SEGUE_REMOVIDO)
        { // Removed record, skip
            free(temp_reg);
        }
        else
        { // LE_REGISTRO_SEGUE_ERRO_LEITURA or other error
            free(temp_reg);
            break;
        }
    }

    if (valid_records_count == 0)
    {
        free(registros_validos);
        *numRegistros = 0;
        return NULL;
    }
    RegistroSegue **final_registros = realloc(registros_validos, valid_records_count * sizeof(RegistroSegue *));
    if (final_registros == NULL)
    {
        *numRegistros = valid_records_count;
        return registros_validos;
    }

    *numRegistros = valid_records_count;
    return final_registros;
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

    qsort(indice, *numPessoas, sizeof(RegistroIndice *), comparar_indices_id);

    IdUsuarioMap *mapa = malloc(*numPessoas * sizeof(IdUsuarioMap));
    if (!mapa)
        return NULL;

    fseek(fpPessoa, 0, SEEK_SET);
    long long byteOffset = 0;

    for (int i = 0; i < *numPessoas; i++)
    {
        long long diffByteOffset = indice[i]->byteOffset - byteOffset;
        fseek(fpPessoa, diffByteOffset, SEEK_CUR);

        RegistroPessoa *p;
        if (le_registro_pessoa(fpPessoa, &p) == 0)
        {
            mapa[i].idPessoa = p->idPessoa;
            mapa[i].nomeUsuario = strdup(p->nomeUsuario);
        }

        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + p->tamanhoRegistro;
        byteOffset += diffByteOffset + tamanho_real_escrito;

        destroi_registro_indice(indice[i]);
        destroi_registro_pessoa(p);
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

static void ordenar_indices_dos_vertices_por_nome(int *indices, int n, Grafo *g)
{
    for (int i = 0; i < n - 1; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < n; j++)
        {
            // Compara os nomes nos vértices apontados pelos índices
            if (strcmp(g->vertices[indices[j]].nomeUsuario, g->vertices[indices[min_idx]].nomeUsuario) < 0)
            {
                min_idx = j;
            }
        }
        // Troca
        int temp = indices[min_idx];
        indices[min_idx] = indices[i];
        indices[i] = temp;
    }
}

static int obter_indice_vertice_por_nome(Grafo *g, const char *nome)
{
    if (!g || !nome)
        return -1;

    for (int i = 0; i < g->numVertices; i++)
    {
        if (strcmp(g->vertices[i].nomeUsuario, nome) == 0)
        {
            return i;
        }
    }
    return -1;
}