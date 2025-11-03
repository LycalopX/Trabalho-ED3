#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f6.h"



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

static int parse_insercao_linha(char *line, int *numero_busca, int *idPessoa, char *nomePessoa, char *idadeString, char *nomeUsuario)
{
    line[strcspn(line, "\r\n")] = 0;

    char *token;

    token = strtok(line, " ");
    if (token == NULL) return 0;
    *numero_busca = atoi(token);

    token = strtok(NULL, ",");
    if (token == NULL) return 0;
    *idPessoa = atoi(token);

    token = strtok(NULL, ",");
    if (token == NULL) return 0;
    while (*token == ' ' || *token == '"') token++;
    char *end = token + strlen(token) - 1;
    while (end > token && *end == '"') *end-- = '\0';
    strcpy(nomePessoa, token);

    token = strtok(NULL, ",");
    if (token == NULL) return 0;
    while (*token == ' ') token++;
    strcpy(idadeString, token);

    token = strtok(NULL, "\n");
    if (token == NULL) return 0;
    while (*token == ' ' || *token == '"') token++;
    end = token + strlen(token) - 1;
    while (end > token && *end == '"') *end-- = '\0';
    strcpy(nomeUsuario, token);

    return 5;
}

void funcionalidade6(FILE *fp, FILE *fpIndice, int insercoes)
{
    CabecalhoPessoa *cp = malloc(sizeof(CabecalhoPessoa));
    if (cp == NULL) {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }
    le_cabecalho_pessoa(fp, cp);

    toggle_cabecalho_pessoa(fp, cp);
    
    CabecalhoIndice index_header_temp;
    le_cabecalho_indice(fpIndice, &index_header_temp);
    toggle_cabecalho_indice(fpIndice, &index_header_temp);

    RegistroBuscaPessoa **registrosInseridos = malloc(insercoes * sizeof(RegistroBuscaPessoa *));
    if (registrosInseridos == NULL)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cp);
        return;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    RegistroPessoa *novo_reg = NULL;
    
    for (int i = 0; i < insercoes; i++)
    {
        int numero_busca, idPessoa, idade;
        char nomePessoa[200], idadeString[20], nomeUsuario[200];

        char input_line[500];
        fgets(input_line, sizeof(input_line), stdin);

        if (parse_insercao_linha(input_line, &numero_busca, &idPessoa, nomePessoa, idadeString, nomeUsuario) < 5)
        {
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
            free(registrosInseridos);
            return;
        }

        long long offset_insercao = cp->proxByteOffset;
        cp->proxByteOffset += sizeof(char) + sizeof(int) + novo_reg->tamanhoRegistro;

        registrosInseridos[i] = malloc(sizeof(RegistroBuscaPessoa));
        if (registrosInseridos[i] == NULL) 
        { 
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            free(cp);
            destroi_registro_pessoa(novo_reg);
            // TODO: Liberar o resto do array
            free(registrosInseridos);
            return;
        }
        registrosInseridos[i]->registro = novo_reg;
        registrosInseridos[i]->ByteOffset = offset_insercao;
    }

    qsort(registrosInseridos, insercoes, sizeof(RegistroBuscaPessoa *), compara_inseridos_busca);

    fseek(fp, 17, SEEK_SET);
    long long byteOffset = 17;

    for (int i = 0; i < insercoes; i++)
    {
        long long diffByteOffset = registrosInseridos[i]->ByteOffset - byteOffset;

        fseek(fp, diffByteOffset, SEEK_CUR);
        
        escreve_registro_pessoa(fp, registrosInseridos[i]->registro);

        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + registrosInseridos[i]->registro->tamanhoRegistro;

        byteOffset += diffByteOffset + tamanho_real_escrito;
    }

    RegistroIndice **indices_novos = malloc(insercoes * sizeof(RegistroIndice *));
    for (int i = 0; i < insercoes; i++)
    {
        indices_novos[i] = malloc(sizeof(RegistroIndice));
        indices_novos[i]->idPessoa = registrosInseridos[i]->registro->idPessoa;
        indices_novos[i]->byteOffset = registrosInseridos[i]->ByteOffset;
    }

    for (int i = 0; i < insercoes; i++)
    {
        destroi_registro_pessoa(registrosInseridos[i]->registro);
        free(registrosInseridos[i]);
    }
    free(registrosInseridos);

    qsort(indices_novos, insercoes, sizeof(RegistroIndice *), comparar_indices_id);

    fseek(fpIndice, 0, SEEK_SET);

    RegistroIndice **indices_antigos = carregar_indice_inteiro(fpIndice, cp->quantidadePessoas);
    if (indices_antigos == NULL && cp->quantidadePessoas > 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(cp);
        // Liberar mais coisas
        return;
    }

    int n_final = cp->quantidadePessoas + insercoes;
    RegistroIndice **indice_final = intercalar_indices(indices_antigos, cp->quantidadePessoas, indices_novos, insercoes);

    if(indices_antigos) free(indices_antigos);
    if(indices_novos) free(indices_novos);

    CabecalhoIndice index_header;
    le_cabecalho_indice(fpIndice, &index_header);

    toggle_cabecalho_indice(fpIndice, &index_header);

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

    long finalPos = ftell(fpIndice);
    ftruncate(fileno(fpIndice), finalPos);

    index_header.status = '1';
    escreve_cabecalho_indice(fpIndice, &index_header);

    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    cp->status = '1';
    cp->quantidadePessoas = cp->quantidadePessoas + insercoes;

    escreve_cabecalho_pessoa(fp, cp);
    fflush(fp);
    fflush(fpIndice);

    free(cp);
}
