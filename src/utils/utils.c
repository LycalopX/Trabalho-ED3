#include <stddef.h> // Para size_t
#include <stdint.h> // Para SIZE_MAX (para checagem de overflow)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"

#define GOLDEN_RATIO 1.61803398875

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

void scan_quote_string(char *str)
{

    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n')
    { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // ignorar aspas fechando
    }
    else if (R != EOF)
    { 
        int i = 0;
        str[i++] = R;
        while((R = getchar()) != EOF && !isspace(R)) {
            str[i++] = R;
        }
        str[i] = '\0';
        if(R != EOF) ungetc(R, stdin);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}

// FUNÇÕES DE COMPARAÇÃO PARA BUSCA

int comparar_registros_busca_offset(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;

    if (regA->ByteOffset < regB->ByteOffset) return -1;
    if (regA->ByteOffset > regB->ByteOffset) return 1;
    return 0;
}

int comparar_registros_busca_id(const void *a, const void *b)
{
    RegistroBuscaPessoa *regA = *(RegistroBuscaPessoa **)a;
    RegistroBuscaPessoa *regB = *(RegistroBuscaPessoa **)b;
    return regA->registro->idPessoa - regB->registro->idPessoa;
}

// GERENCIAMENTO DE ARRAY EXPANDÍVEL

void realloc_golden(void **ptr, size_t *p_current_capacity, size_t elem_size)
{

    // 1. Calcular a nova capacidade
    size_t new_capacity;
    if (*p_current_capacity == 0)
    {
        // Caso inicial: alocar pela primeira vez
        new_capacity = 10;
    }
    else
    {
        // Arredonda para o inteiro mais próximo para garantir o crescimento
        new_capacity = (size_t)(*p_current_capacity * GOLDEN_RATIO + 0.5);

        // Garantir que a capacidade realmente aumente (caso *p_current_capacity seja 1)
        if (new_capacity <= *p_current_capacity)
        {
            new_capacity = *p_current_capacity + 1;
        }
    }

    // 2. Checagem de segurança contra overflow de multiplicação
    // (new_capacity * elem_size) pode exceder o tamanho máximo de um size_t
    if (elem_size > 0 && new_capacity > SIZE_MAX / elem_size)
    {
        fprintf(stderr, "Erro: Overflow de alocação (tamanho solicitado excede SIZE_MAX).\n");
        return;
    }

    // 3. Calcular o novo tamanho total em bytes
    size_t new_size_bytes = new_capacity * elem_size;

    // 4. Chamar realloc. realloc lida corretamente com ptr == NULL.
    (*ptr) = realloc(*ptr, new_size_bytes);

    // 5. Tratar o resultado
    if ((*ptr) == NULL)
    {
        // Falha na alocação!
        // O ponteiro original 'ptr' ainda é válido e não foi liberado.
        // Não atualizamos *p_current_capacity.
        fprintf(stderr, "Erro: Falha ao realocar memória para %zu bytes.\n", new_size_bytes);
        return;
    }

    // 6. Sucesso! Atualizar a capacidade e retornar o novo ponteiro.
    *p_current_capacity = new_capacity;
}

void liberar_resultados_busca(ResultadoBuscaPessoa *resultados, int nBuscas)
{
    if (resultados == NULL)
        return;

    for (int i = 0; i < nBuscas; i++)
    {
        if (resultados[i].registrosBusca != NULL)
        {
            for (int j = 0; j < resultados[i].nRegistros; j++)
            {
                if (resultados[i].registrosBusca[j] != NULL)
                {
                    destroi_registro_pessoa(resultados[i].registrosBusca[j]->registro);
                    free(resultados[i].registrosBusca[j]);
                }
            }
            free(resultados[i].registrosBusca);
        }
        if (resultados[i].busca.campo != NULL)
            free(resultados[i].busca.campo);
        if (resultados[i].busca.valor != NULL)
            free(resultados[i].busca.valor);
        if (resultados[i].update.campo != NULL)
            free(resultados[i].update.campo);
        if (resultados[i].update.valor != NULL)
            free(resultados[i].update.valor);
    }
    free(resultados);
}

// DEBUG
void imprimir_registros_raw(FILE *fp)
{
    if (fp == NULL)
    {
        printf("Arquivo inválido.\n");
        return;
    }

    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fp, &cabPessoa); // Vamos imprimir o cabeçalho

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

        // Tenta ler o próximo registro
        if (le_registro_pessoa(fp, &reg) != 0)
        {
            // Se a leitura falhar, pode ser o fim do arquivo.
            break;
        }

        printf("Registro em %ld | Removido: '%c' | Tamanho Declarado: %d\n",
               currentPos, reg->removido, reg->tamanhoRegistro);

        // Calcula o tamanho real dos dados lidos para este registro
        long tamanho_real_lido = sizeof(reg->idPessoa) + sizeof(reg->idadePessoa) +
                                 sizeof(reg->tamanhoNomePessoa) + reg->tamanhoNomePessoa +
                                 sizeof(reg->tamanhoNomeUsuario) + reg->tamanhoNomeUsuario;

        printf("  - ID: %d\n", reg->idPessoa);
        printf("  - Idade: %d\n", reg->idadePessoa);

        printf("  - Nome (%d): ", reg->tamanhoNomePessoa);
        if (reg->tamanhoNomePessoa > 0)
        {
            fwrite(reg->nomePessoa, 1, reg->tamanhoNomePessoa, stdout);
        }
        printf("\n");

        printf("  - Usuario (%d): ", reg->tamanhoNomeUsuario);
        fwrite(reg->nomeUsuario, 1, reg->tamanhoNomeUsuario, stdout);

        printf("\n");

        // O lixo é a diferença entre o tamanho declarado e o tamanho real dos campos variáveis + fixos
        long lixo = reg->tamanhoRegistro - tamanho_real_lido;
        printf("  - Lixo: %ld bytes\n\n", lixo);

        // O lixo já é pulado pela função le_registro_pessoa.
        // Manter o fseek aqui causaria um pulo duplo.

        destroi_registro_pessoa(reg);
    }

    printf("--- FIM RAW PRINT ---\n");
    fflush(stdout);
}

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
    le_cabecalho_pessoa(fp, &cabPessoa); // Vamos imprimir o cabeçalho

    fprintf(output_fp, "Cabeçalho:\n");
    fprintf(output_fp, "  - Próximo Byte Offset: %lld\n", cabPessoa.proxByteOffset);
    fprintf(output_fp, "  - Quantidade Removidos: %d\n", cabPessoa.quantidadeRemovidos);
    fprintf(output_fp, "  - Quantidade Pessoas: %d\n", cabPessoa.quantidadePessoas);
    fprintf(output_fp, "Status: %c\n\n", cabPessoa.status);

    while (1)
    {
        long currentPos = ftell(fp);
        RegistroPessoa *reg = NULL;

        // Tenta ler o próximo registro
        if (le_registro_pessoa(fp, &reg) != 0)
        {
            // Se a leitura falhar, pode ser o fim do arquivo.
            break;
        }

        fprintf(output_fp, "Registro em %ld | Removido: '%c' | Tamanho Declarado: %d\n",
               currentPos, reg->removido, reg->tamanhoRegistro);

        // Calcula o tamanho real dos dados lidos para este registro
        long tamanho_real_lido = sizeof(reg->idPessoa) + sizeof(reg->idadePessoa) +
                                 sizeof(reg->tamanhoNomePessoa) + reg->tamanhoNomePessoa +
                                 sizeof(reg->tamanhoNomeUsuario) + reg->tamanhoNomeUsuario;

        fprintf(output_fp, "  - ID: %d\n", reg->idPessoa);
        fprintf(output_fp, "  - Idade: %d\n", reg->idadePessoa);

        fprintf(output_fp, "  - Nome (%d): ", reg->tamanhoNomePessoa);
        if (reg->tamanhoNomePessoa > 0)
        {
            fwrite(reg->nomePessoa, 1, reg->tamanhoNomePessoa, output_fp);
        }
        fprintf(output_fp, "\n");

        fprintf(output_fp, "  - Usuario (%d): ", reg->tamanhoNomeUsuario);
        fwrite(reg->nomeUsuario, 1, reg->tamanhoNomeUsuario, output_fp);

        fprintf(output_fp, "\n");

        // O lixo é a diferença entre o tamanho declarado e o tamanho real dos campos variáveis + fixos
        long lixo = reg->tamanhoRegistro - tamanho_real_lido;
        fprintf(output_fp, "  - Lixo: %ld bytes\n\n", lixo);

        // O lixo já é pulado pela função le_registro_pessoa.
        // Manter o fseek aqui causaria um pulo duplo.

        destroi_registro_pessoa(reg);
    }

    fflush(stdout);
}

// Só funciona quando resultados está ordenado pelo byteoffset do registro.
void remover_pessoas_e_indices(RegistroBuscaPessoa **resultados, RegistroIndice **indices, CabecalhoPessoa *cabPessoa, int nRegsEncontrados, FILE *fp, int flagUpdate)
{
    long long nextByteOffset;
    long long previousByteOffset = 17;

    // Escreve os resultados e libera a memória
    if (resultados != NULL)
    {
        for (int i = 0; i < nRegsEncontrados; i++)
        {
            //debuggin
            printf("Removendo registro com id %d no byte offset %lld\n", resultados[i]->registro->idPessoa, resultados[i]->ByteOffset);
            // propriedades do registro:
            printf("  - Tamanho Registro: %d\n", resultados[i]->registro->tamanhoRegistro);
            printf("  - Idade Pessoa: %d\n", resultados[i]->registro->idadePessoa);
            printf("  - Tamanho Nome Pessoa: %d\n", resultados[i]->registro->tamanhoNomePessoa);
            printf("  - Nome Pessoa: %s\n", resultados[i]->registro->nomePessoa ? resultados[i]->registro->nomePessoa : "NULL");
            printf("  - Tamanho Nome Usuario: %d\n", resultados[i]->registro->tamanhoNomeUsuario);
            printf("  - Nome Usuario: %s\n", resultados[i]->registro->nomeUsuario ? resultados[i]->registro->nomeUsuario : "NULL");
            
            nextByteOffset = resultados[i]->ByteOffset - previousByteOffset;

            // Remover registro
            fseek(fp, nextByteOffset, SEEK_CUR);

            char removido = '1';
            fwrite(&removido, sizeof(char), 1, fp);

            // Remover do indice
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
                    // em vez de remover indice, atualizamos para apontar ao final do arquivo
                    (*p_encontrado_ptr)->byteOffset = cabPessoa->proxByteOffset;
                    resultados[i]->ByteOffset = cabPessoa->proxByteOffset;
                    cabPessoa->proxByteOffset += tamanho_real_escrito;
                }
                else
                {
                    (*p_encontrado_ptr)->byteOffset = -1; // Marca como removido no índice
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
