#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"

#define GOLDEN_RATIO 1.61803398875

// Calcula e imprime um checksum do arquivo binário na tela.
void binarioNaTela(char *nomeArquivoBinario)
{

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb")))
    {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

// Lê uma string da entrada padrão, tratando aspas e valores nulos.
void scan_quote_string(char *str)
{

    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ;

    if (R == 'N' || R == 'n')
    {
        getchar();
        getchar();
        getchar();
        strcpy(str, "");
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        {
            strcpy(str, "");
        }
        getchar();
    }
    else if (R != EOF)
    {
        int i = 0;
        str[i++] = R;
        while ((R = getchar()) != EOF && !isspace(R))
        {
            str[i++] = R;
        }
        str[i] = '\0';
        if (R != EOF)
            ungetc(R, stdin);
    }
    else
    {
        strcpy(str, "");
    }
}

// Função de comparação para qsort, utilizada para ordenar registros de busca por byte offset.
int comparar_registros_busca_offset(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;

    if (regA->ByteOffset < regB->ByteOffset)
        return -1;
    if (regA->ByteOffset > regB->ByteOffset)
        return 1;
    return 0;
}

// Função de comparação para qsort, utilizada para ordenar registros de busca por ID.
int comparar_registros_busca_id(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;
    return regA->registro->idPessoa - regB->registro->idPessoa;
}

// Realoca um array dinâmico, aumentando sua capacidade usando a proporção áurea.
void realloc_golden(void **ptr, size_t *p_current_capacity, size_t elem_size)
{

    size_t new_capacity;
    if (*p_current_capacity == 0)
    {

        new_capacity = 10;
    }
    else
    {

        new_capacity = (size_t)(*p_current_capacity * GOLDEN_RATIO + 0.5);

        if (new_capacity <= *p_current_capacity)
        {
            new_capacity = *p_current_capacity + 1;
        }
    }

    if (elem_size > 0 && new_capacity > SIZE_MAX / elem_size)
    {
        fprintf(stderr, "Erro: Overflow de alocação (tamanho solicitado excede SIZE_MAX).\n");
        return;
    }

    size_t new_size_bytes = new_capacity * elem_size;

    (*ptr) = realloc(*ptr, new_size_bytes);

    if ((*ptr) == NULL)
    {

        fprintf(stderr, "Erro: Falha ao realocar memória para %zu bytes.\n", new_size_bytes);
        return;
    }

    *p_current_capacity = new_capacity;
}

// Libera a memória alocada para um array de resultados de busca.
void liberar_resultados_busca_inteira(ResultadoBuscaPessoa *resultados, int nBuscas)
{
    if (resultados == NULL)
        return;

    for (int i = 0; i < nBuscas; i++)
    {
        liberar_resultado_busca(&resultados[i]);
    }
    free(resultados);
}

// Libera a memória alocada para uma única struct ResultadoBuscaPessoa.
void liberar_resultado_busca(ResultadoBuscaPessoa *resultado)
{
    if (resultado->registrosBusca != NULL)
    {
        for (int j = 0; j < resultado->nRegistros; j++)
        {
            if (resultado->registrosBusca[j] != NULL)
            {
                destroi_registro_pessoa(resultado->registrosBusca[j]->registro);
                free(resultado->registrosBusca[j]);
            }
        }
        free(resultado->registrosBusca);
    }
    if (resultado->busca.campo != NULL)
        free(resultado->busca.campo);
    if (resultado->busca.valor != NULL)
        free(resultado->busca.valor);
    if (resultado->update.campo != NULL)
        free(resultado->update.campo);
    if (resultado->update.valor != NULL)
        free(resultado->update.valor);
}

// Imprime o conteúdo raw de registros de pessoa na saída padrão para depuração.
void imprimir_registros_raw(FILE *fp)
{
    if (fp == NULL)
    {
        printf("Arquivo inválido.\n");
        return;
    }

    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fp, &cabPessoa);

    printf("Cabeçalho:\n");
    printf("  - Próximo Byte Offset: %lld\n", cabPessoa.proxByteOffset);
    printf("  - Quantidade Removidos: %d\n", cabPessoa.quantidadeRemovidos);
    printf("  - Quantidade Pessoas: %d\n", cabPessoa.quantidadePessoas);
    printf("Status: %c\n\n", cabPessoa.status);
    printf("--- INICIO RAW PRINT ---\n");

    while (1)
    {
        long currentPos = ftell(fp);
        RegistroPessoa *reg = NULL;

        if (le_registro_pessoa(fp, &reg) != 0)
        {

            break;
        }

        printf("Registro em %ld | Tamanho Declarado: %d | Removido: '%c'\n",
               currentPos, reg->tamanhoRegistro, reg->removido);

        long tamanho_real_lido = sizeof(reg->idPessoa) + sizeof(reg->idadePessoa) +
                                 sizeof(reg->tamanhoNomePessoa) + reg->tamanhoNomePessoa +
                                 sizeof(reg->tamanhoNomeUsuario) + reg->tamanhoNomeUsuario;

        printf("  - Nome (%d): ", reg->tamanhoNomePessoa);
        if (reg->tamanhoNomePessoa > 0)
        {
            fwrite(reg->nomePessoa, 1, reg->tamanhoNomePessoa, stdout);
        }
        printf("\n");

        long lixo = reg->tamanhoRegistro - tamanho_real_lido;
        printf("  - Lixo: %ld bytes\n", lixo);

        printf("  - ID: %d\n", reg->idPessoa);
        printf("  - Idade: %d\n", reg->idadePessoa);

        printf("  - Usuario (%d): ", reg->tamanhoNomeUsuario);
        fwrite(reg->nomeUsuario, 1, reg->tamanhoNomeUsuario, stdout);

        printf("\n\n");

        destroi_registro_pessoa(reg);
    }

    printf("--- FIM RAW PRINT ---\n");
    fflush(stdout);
}

// Imprime o conteúdo raw de registros de pessoa em um arquivo de saída para depuração.
void imprimir_registros_raw_em_arquivo(FILE *fp, char *nome_arquivo_saida)
{
    if (fp == NULL)
    {
        printf("Arquivo inválido.\n");
        return;
    }

    FILE *output_fp = fopen(nome_arquivo_saida, "w");
    if (output_fp == NULL)
    {
        printf("Não foi possível abrir o arquivo de saída.\n");
        return;
    }

    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fp, &cabPessoa);

    fprintf(output_fp, "Cabeçalho:\n");
    fprintf(output_fp, "  - Próximo Byte Offset: %lld\n", cabPessoa.proxByteOffset);
    fprintf(output_fp, "  - Quantidade Removidos: %d\n", cabPessoa.quantidadeRemovidos);
    fprintf(output_fp, "  - Quantidade Pessoas: %d\n", cabPessoa.quantidadePessoas);
    fprintf(output_fp, "Status: %c\n\n", cabPessoa.status);

    while (1)
    {
        long currentPos = ftell(fp);
        RegistroPessoa *reg = NULL;

        if (le_registro_pessoa(fp, &reg) != 0)
        {

            break;
        }

        fprintf(output_fp, "Registro em %ld | Tamanho Declarado: %d | Removido: '%c'\n",
                currentPos, reg->tamanhoRegistro, reg->removido);

        long tamanho_real_lido = sizeof(reg->idPessoa) + sizeof(reg->idadePessoa) +
                                 sizeof(reg->tamanhoNomePessoa) + reg->tamanhoNomePessoa +
                                 sizeof(reg->tamanhoNomeUsuario) + reg->tamanhoNomeUsuario;

        fprintf(output_fp, "  - Nome (%d): ", reg->tamanhoNomePessoa);
        if (reg->tamanhoNomePessoa > 0)
        {
            fwrite(reg->nomePessoa, 1, reg->tamanhoNomePessoa, output_fp);
        }
        fprintf(output_fp, "\n");

        long lixo = reg->tamanhoRegistro - tamanho_real_lido;
        fprintf(output_fp, "  - Lixo: %ld bytes\n", lixo);

        fprintf(output_fp, "  - ID: %d\n", reg->idPessoa);
        fprintf(output_fp, "  - Idade: %d\n", reg->idadePessoa);

        fprintf(output_fp, "  - Usuario (%d): ", reg->tamanhoNomeUsuario);
        fwrite(reg->nomeUsuario, 1, reg->tamanhoNomeUsuario, output_fp);

        fprintf(output_fp, "\n\n");

        destroi_registro_pessoa(reg);
    }

    fflush(stdout);
}

// Marca registros de pessoa como removidos no arquivo de dados e atualiza o índice em memória.
void remover_pessoas_e_indices(RegistroBuscaPessoa **resultados, RegistroIndice **indices, CabecalhoPessoa *cabPessoa, int nRegsEncontrados, FILE *fp, int flagUpdate)
{
    long long nextByteOffset;
    long long previousByteOffset = 17;

    if (resultados != NULL)
    {
        for (int i = 0; i < nRegsEncontrados; i++)
        {
            nextByteOffset = resultados[i]->ByteOffset - previousByteOffset;

            fseek(fp, nextByteOffset, SEEK_CUR);

            char removido = '1';
            fwrite(&removido, sizeof(char), 1, fp);

            int id_a_remover = resultados[i]->registro->idPessoa;

            RegistroIndice **p_encontrado_ptr = bsearch(&id_a_remover,
                                                        indices,
                                                        cabPessoa->quantidadePessoas,
                                                        sizeof(RegistroIndice *),
                                                        comparar_bsearch_indice);

            long long tamanho_real_escrito = sizeof(char) + sizeof(int) + resultados[i]->registro->tamanhoRegistro;

            if (p_encontrado_ptr != NULL)
            {
                if (flagUpdate)
                {

                    (*p_encontrado_ptr)->byteOffset = cabPessoa->proxByteOffset;
                    resultados[i]->ByteOffset = cabPessoa->proxByteOffset;
                    cabPessoa->proxByteOffset += tamanho_real_escrito;
                }
                else
                {
                    (*p_encontrado_ptr)->byteOffset = -1;
                }
            }

            previousByteOffset += nextByteOffset + 1;
        }
    }
    else
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
    }
}
