#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

// Lê o cabeçalho do arquivo de dados. Retorna 1 em caso de erro.
int le_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab)
{
    fseek(fp, 0, SEEK_SET);
    if (fread(cab, sizeof(CabecalhoPessoa), 1, fp) < 1)
    {
        return 1;
    }
    return 0;
}

// Escreve o cabeçalho no arquivo de dados. Retorna 1 em caso de erro.
int escreve_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab)
{
    fseek(fp, 0, SEEK_SET);

    if (fwrite(&cab->status, sizeof(char), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->quantidadePessoas, sizeof(int), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->quantidadeRemovidos, sizeof(int), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->proxByteOffset, sizeof(long long), 1, fp) < 1)
        return 1;

    return 0;
}

// Lê um registro de dados do arquivo. Retorna 1 em caso de erro ou fim de arquivo.
int le_registro_pessoa(FILE *fp, RegistroPessoa **reg_out)
{
    RegistroPessoa *reg = malloc(sizeof(RegistroPessoa));
    if (reg == NULL)
        return 1;

    if (fread(&reg->removido, sizeof(char), 1, fp) == 0)
    {
        free(reg);
        return 1;
    }

    fread(&reg->tamanhoRegistro, sizeof(int), 1, fp);
    fread(&reg->idPessoa, sizeof(int), 1, fp);
    fread(&reg->idadePessoa, sizeof(int), 1, fp);

    fread(&reg->tamanhoNomePessoa, sizeof(int), 1, fp);
    if (reg->tamanhoNomePessoa > 0)
    {
        reg->nomePessoa = malloc(reg->tamanhoNomePessoa);
        if (reg->nomePessoa == NULL)
        {
            free(reg);
            return 1;
        }
        fread(reg->nomePessoa, reg->tamanhoNomePessoa, 1, fp);
    }
    else
    {
        reg->nomePessoa = NULL;
    }

    fread(&reg->tamanhoNomeUsuario, sizeof(int), 1, fp);
    if (reg->tamanhoNomeUsuario > 0)
    {
        reg->nomeUsuario = malloc(reg->tamanhoNomeUsuario);
        if (reg->nomeUsuario == NULL)
        {
            if (reg->nomePessoa != NULL)
                free(reg->nomePessoa);
            free(reg);
            return 1;
        }
        fread(reg->nomeUsuario, reg->tamanhoNomeUsuario, 1, fp);
    }
    else
    {
        reg->nomeUsuario = NULL;
    }

    *reg_out = reg;
    return 0;
}

// Escreve um registro de dados no arquivo. Retorna 1 em caso de erro.
int escreve_registro_pessoa(FILE *fp, RegistroPessoa *reg)
{
    if (fwrite(&reg->removido, sizeof(char), 1, fp) < 1)
        return 1;
    if (fwrite(&reg->tamanhoRegistro, sizeof(int), 1, fp) < 1)
        return 1;
    if (fwrite(&reg->idPessoa, sizeof(int), 1, fp) < 1)
        return 1;
    if (fwrite(&reg->idadePessoa, sizeof(int), 1, fp) < 1)
        return 1;
    if (fwrite(&reg->tamanhoNomePessoa, sizeof(int), 1, fp) < 1)
        return 1;

    if (reg->tamanhoNomePessoa > 0)
    {
        if (fwrite(reg->nomePessoa, reg->tamanhoNomePessoa, 1, fp) < 1)
            return 1;
    }

    if (fwrite(&reg->tamanhoNomeUsuario, sizeof(int), 1, fp) < 1)
        return 1;

    if (reg->tamanhoNomeUsuario > 0)
    {
        if (fwrite(reg->nomeUsuario, reg->tamanhoNomeUsuario, 1, fp) < 1)
            return 1;
    }

    return 0;
}

// Imprime um registro de pessoa formatado. Retorna 1 se o registro for nulo.
int imprime_registro_pessoa(RegistroPessoa *reg)
{
    if (reg == NULL)
        return 1;

    printf("Nome: ");
    if (reg->tamanhoNomePessoa > 0)
    {
        fwrite(reg->nomePessoa, sizeof(char), reg->tamanhoNomePessoa, stdout);
    }
    printf("\n");

    printf("Idade: %d\n", reg->idadePessoa);

    printf("Usuário: ");
    if (reg->tamanhoNomeUsuario > 0)
    {
        fwrite(reg->nomeUsuario, sizeof(char), reg->tamanhoNomeUsuario, stdout);
    }
    printf("\n");

    return 0;
}

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
    { // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}