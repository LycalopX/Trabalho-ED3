#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f6.h"

#define FALHA_AO_PROCESSAR_ARQUIVO "Falha no processamento do arquivo.\n"

// Função de comparação para qsort, para ordenar um array de RegistroBuscaPessoa** pelo tamanho do registro.
static int compara_removidos_busca(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;
    return regA->registro->tamanhoRegistro - regB->registro->tamanhoRegistro;
}

static int compara_inseridos_busca(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;

    if (regA->ByteOffset < regB->ByteOffset)
        return -1;
    else if (regA->ByteOffset > regB->ByteOffset)
        return 1;
    else
        return 0;
}

// Busca binária best-fit em um array de RegistroBuscaPessoa**.
static int busca_binaria_best_fit_busca(RegistroBuscaPessoa **removidos, int n, int tamanho_necessario)
{
    int low = 0;
    int high = n - 1;
    int best_fit_idx = -1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        if (removidos[mid]->registro->tamanhoRegistro >= tamanho_necessario)
        {
            best_fit_idx = mid;
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }
    return best_fit_idx;
}

// Função de comparação para qsort, para ordenar um array de RegistroIndice** pelo idPessoa.
static int comparar_indices_id(const void *a, const void *b)
{
    RegistroIndice *regA = *(RegistroIndice **)a;
    RegistroIndice *regB = *(RegistroIndice **)b;
    return regA->idPessoa - regB->idPessoa;
}

static int parse_insercao_linha(char *line, int *numero_busca, int *idPessoa, char *nomePessoa, char *idadeString, char *nomeUsuario)
{
    line[strcspn(line, "\r\n")] = 0;

    char *token;

    // numero_busca
    token = strtok(line, " ");
    if (token == NULL) return 0;
    *numero_busca = atoi(token);

    // idPessoa
    token = strtok(NULL, ",");
    if (token == NULL) return 0;
    *idPessoa = atoi(token);

    // nomePessoa
    token = strtok(NULL, ",");
    if (token == NULL) return 0;
    // Remove leading space and quotes
    while (*token == ' ' || *token == '"') token++;
    char *end = token + strlen(token) - 1;
    while (end > token && *end == '"') *end-- = '\0';
    strcpy(nomePessoa, token);

    // idadeString
    token = strtok(NULL, ",");
    if (token == NULL) return 0;
    while (*token == ' ') token++; // Trim leading space
    strcpy(idadeString, token);

    // nomeUsuario
    token = strtok(NULL, "\n");
    if (token == NULL) return 0;
    // Remove leading space and quotes
    while (*token == ' ' || *token == '"') token++;
    end = token + strlen(token) - 1;
    while (end > token && *end == '"') *end-- = '\0';
    strcpy(nomeUsuario, token);

    return 5;
}

void funcionalidade6(FILE *fp, FILE *fpIndice, int insercoes)
{

    // Atualizar cabeçalho
    CabecalhoPessoa *cp = malloc(sizeof(CabecalhoPessoa));
    le_cabecalho_pessoa(fp, cp); // Lê o cabeçalho para posicionar o ponteiro corretamente

    toggle_cabecalho_pessoa(fpIndice, cp);

    // Passo 1: Ler todos os registros, guardando os removidos em uma lista de RegistroBuscaPessoa
    RegistroBuscaPessoa **removidos = NULL;
    size_t capacidade_removidos = 0;

    int n_removidos = 0;
    long long offset_corrente = 17; // Início dos dados após o cabeçalho

    // Procurar todos os registros removidos
    fseek(fp, offset_corrente, SEEK_SET);
    RegistroPessoa *reg_temp;
    while (le_registro_pessoa(fp, &reg_temp) == 0)
    {
        int tamanho_registro_atual = reg_temp->tamanhoRegistro;
        if (reg_temp->removido == '1')
        {
            if (n_removidos >= capacidade_removidos)
            {
                realloc_golden((void **)&removidos, &capacidade_removidos, sizeof(RegistroBuscaPessoa *));
                if (removidos == NULL)
                {
                    printf(FALHA_AO_PROCESSAR_ARQUIVO);
                    destroi_registro(reg_temp);
                    free(cp);
                    return;
                }
            }

            RegistroBuscaPessoa *busca_reg = malloc(sizeof(RegistroBuscaPessoa));

            if (busca_reg == NULL)
            {
                printf(FALHA_AO_PROCESSAR_ARQUIVO);
                destroi_registro(reg_temp);
                free(cp);
                return;
            }

            busca_reg->registro = reg_temp;
            busca_reg->ByteOffset = offset_corrente;

            removidos[n_removidos++] = busca_reg;
        } else {
            destroi_registro(reg_temp);
        }

        offset_corrente += sizeof(char) + sizeof(int) + tamanho_registro_atual;
    }

    // Ordenar a lista de removidos por tamanho para usar a estratégia best-fit
    qsort(removidos, n_removidos, sizeof(RegistroBuscaPessoa *), compara_removidos_busca);

    printf("--- DEBUG: Removidos ---\n");
    for (int i = 0; i < n_removidos; i++) {
        printf("Removido %d: offset=%lld, size=%d\n", i, removidos[i]->ByteOffset, removidos[i]->registro->tamanhoRegistro);
    }
    printf("--- END DEBUG: Removidos ---\n");

    // Loop de inserção para os novos registros
    RegistroBuscaPessoa **registrosInseridos = malloc(insercoes * sizeof(RegistroBuscaPessoa *));
    if (registrosInseridos == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cp);
        return;
    }

    int n_registros_substituidos = 0;

    // Consumir o restante da linha do scanf anterior em main.c
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    RegistroPessoa *novo_reg = NULL;
    
    for (int i = 0; i < insercoes; i++)
    {
        int numero_busca, idPessoa, idade;
        char nomePessoa[200], idadeString[20], nomeUsuario[200];

        char input_line[500];
        fgets(input_line, sizeof(input_line), stdin); // Lê a linha inteira

        if (parse_insercao_linha(input_line, &numero_busca, &idPessoa, nomePessoa, idadeString, nomeUsuario) < 5)
        {
            // Tratar erro de parsing, talvez pular esta linha
            continue;
        }

        if (strcmp(idadeString, "NULO") == 0)
        {
            idade = -1;
        }
        else
        {
            idade = atoi(idadeString);
        }

        novo_reg = cria_registro_pessoa(idPessoa, nomePessoa, idade, nomeUsuario);
        if (novo_reg == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            free(cp);
            return;
        }

        // Encontrar local para inserção (best-fit)
        printf("--- DEBUG: Novo Registro ---\n");
        printf("Novo reg: id=%d, size=%d\n", novo_reg->idPessoa, novo_reg->tamanhoRegistro);
        
        int idx_best_fit = busca_binaria_best_fit_busca(removidos, n_removidos, novo_reg->tamanhoRegistro);

        printf("Best-fit idx: %d\n", idx_best_fit);
        printf("--- END DEBUG: Novo Registro ---\n");

        long long offset_insercao;
        if (idx_best_fit != -1)
        {
            n_registros_substituidos++;
            RegistroBuscaPessoa *espaco_removido = removidos[idx_best_fit];
            offset_insercao = espaco_removido->ByteOffset;
            printf("--- DEBUG: Inserindo em slot removido ---\n");
            printf("Offset: %lld\n", offset_insercao);
            printf("--- END DEBUG ---\n");

            novo_reg->tamanhoRegistro = espaco_removido->registro->tamanhoRegistro;

            destroi_registro(espaco_removido->registro);
            free(espaco_removido);          // Libera o RegistroBuscaPessoa
            removidos[idx_best_fit] = NULL; // Marca como usado/removido da lista
        }
        else
        {
            // Inserir no final do arquivo
            offset_insercao = cp->proxByteOffset;
            printf("--- DEBUG: Inserindo no final ---\n");
            printf("Offset: %lld\n", offset_insercao);
            printf("--- END DEBUG ---\n");
            cp->proxByteOffset += sizeof(char) + sizeof(int) + novo_reg->tamanhoRegistro;
        }

        // Criar o novo registro de índice
        registrosInseridos[i] = malloc(sizeof(RegistroBuscaPessoa));
        if (registrosInseridos[i] == NULL) 
        { // Erro de malloc
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            // TODO: Liberar tudo que foi alocado até agora
            free(cp);
            return;
        }
        registrosInseridos[i]->registro = novo_reg;
        registrosInseridos[i]->ByteOffset = offset_insercao;
    }

    // Para juntarmos com o indice comum
    qsort(registrosInseridos, insercoes, sizeof(RegistroBuscaPessoa *), compara_inseridos_busca);

    // DEBUGGING
    for (int i = 0; i < insercoes; i++)
    {
        printf("Inseriremos %d em ByteOffset %lld\n", registrosInseridos[i]->registro->idPessoa, registrosInseridos[i]->ByteOffset);
    }

    // Voltando para depois do cabeçalho
    fseek(fp, 17, SEEK_SET);
    long long byteOffset = 17;

    for (int i = 0; i < insercoes; i++)
    {
        long long diffByteOffset = registrosInseridos[i]->ByteOffset - byteOffset;

        fseek(fp, diffByteOffset, SEEK_CUR);
        
        RegistroPessoa* reg_a_inserir = registrosInseridos[i]->registro;
        escreve_registro_pessoa(fp, reg_a_inserir);

        // Calculate the size of the actual data written
        int bytes_escritos = sizeof(int) + sizeof(int) + sizeof(int) + reg_a_inserir->tamanhoNomePessoa + sizeof(int) + reg_a_inserir->tamanhoNomeUsuario;
        
        // Calculate garbage
        int lixo = reg_a_inserir->tamanhoRegistro - bytes_escritos;

        // Write garbage padding
        if (lixo > 0) {
            char garbage_char = '$';
            for (int j = 0; j < lixo; j++) {
                fwrite(&garbage_char, sizeof(char), 1, fp);
            }
        }

        byteOffset += diffByteOffset + sizeof(char) + sizeof(int) + reg_a_inserir->tamanhoRegistro;
    }

    // A lista de removidos não é mais necessária, podemos liberar sua memória.
    for (int i = 0; i < n_removidos; i++)
    {
        if (removidos[i] != NULL)
        {
            destroi_registro(removidos[i]->registro);
            free(removidos[i]);
        }
    }
    free(removidos);

    // Criar a lista final de novos índices (do tipo RegistroIndice)
    RegistroIndice **indices_novos = malloc(insercoes * sizeof(RegistroIndice *));
    for (int i = 0; i < insercoes; i++)
    {
        indices_novos[i] = malloc(sizeof(RegistroIndice));
        indices_novos[i]->idPessoa = registrosInseridos[i]->registro->idPessoa;
        indices_novos[i]->byteOffset = registrosInseridos[i]->ByteOffset;
    }

    // A lista temporária de registros inseridos não é mais necessária.
    for (int i = 0; i < insercoes; i++)
    {
        destroi_registro(registrosInseridos[i]->registro);
        free(registrosInseridos[i]);
    }
    free(registrosInseridos);

    // Ordenar os novos índices por ID para o merge
    qsort(indices_novos, insercoes, sizeof(RegistroIndice *), comparar_indices_id);

    // Rebobina o arquivo de índice para o início para reescrevê-lo.
    fseek(fpIndice, 0, SEEK_SET);

    // Indices que já estavam no arquivo
    RegistroIndice **indices_antigos = carregar_indice_inteiro(fpIndice, cp->quantidadePessoas);
    if (indices_antigos == NULL && cp->quantidadePessoas > 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cp);
        return;
    }

    // Intercalar os índices antigos e novos
    int n_final = cp->quantidadePessoas + insercoes;
    RegistroIndice **indice_final = intercalar_indices(indices_antigos, cp->quantidadePessoas, indices_novos, insercoes);

    free(indices_antigos);
    free(indices_novos);

    CabecalhoIndice index_header;
    le_cabecalho_indice(fpIndice, &index_header);

    toggle_cabecalho_indice(fpIndice, &index_header);

    // Reescreve os registros de índice válidos.
    fseek(fpIndice, 12, SEEK_SET);
    for (int i = 0; i < n_final; i++)
    {
        if (indice_final[i] != NULL && indice_final[i]->byteOffset > 0)
        {
            fwrite(&indice_final[i]->idPessoa, sizeof(int), 1, fpIndice);
            fwrite(&indice_final[i]->byteOffset, sizeof(long long), 1, fpIndice);
        }
        destroi_registro_indice(indice_final[i]);
    }
    free(indice_final);

    // Trunca o arquivo para remover lixo do final, caso o novo conteúdo seja menor.
    long finalPos = ftell(fpIndice);
    ftruncate(fileno(fpIndice), finalPos);

    index_header.status = '1';
    escreve_cabecalho_indice(fpIndice, &index_header);

    // Atualiza o cabeçalho do arquivo de dados.
    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cp->status = '1';
    cp->quantidadeRemovidos = cp->quantidadeRemovidos - n_registros_substituidos;
    cp->quantidadePessoas = cp->quantidadePessoas + insercoes;

    escreve_cabecalho_pessoa(fp, cp);
    fflush(fp);
    fflush(fpIndice);

    // Liberar o cabeçalho do arquivo de dados
    free(cp);

    printf("Agora imprimindo o arquivo atualizado:\n");
    imprimir_registros_raw(fp);
}
