#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pessoa.h"
#include "indice.h"

// FUNÇÕES STRUCT PESSOA

// Lê o cabeçalho do arquivo de dados. Retorna 1 em caso de erro.
int le_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab)
{
    fseek(fp, 0, SEEK_SET);

    if (fread(&cab->status, sizeof(char), 1, fp) < 1)
        return 1;
    if (fread(&cab->quantidadePessoas, sizeof(int), 1, fp) < 1)
        return 1;
    if (fread(&cab->quantidadeRemovidos, sizeof(int), 1, fp) < 1)
        return 1;
    if (fread(&cab->proxByteOffset, sizeof(long long), 1, fp) < 1)
        return 1;

    return 0;
}

void toggle_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab)
{
    fseek(fp, 0, SEEK_SET);

    cab->status = (cab->status == '0') ? '1' : '0';
    escreve_cabecalho_pessoa(fp, cab);
    return;
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

    reg->nomeUsuario = malloc(reg->tamanhoNomeUsuario);
    if (reg->nomeUsuario == NULL)
    {
        if (reg->nomePessoa != NULL)
            free(reg->nomePessoa);
        free(reg);
        return 1;
    }
    fread(reg->nomeUsuario, reg->tamanhoNomeUsuario, 1, fp);

    // Calcula o total de bytes lidos para os campos de dados do registro.
    int bytes_lidos = sizeof(reg->idPessoa) +
                      sizeof(reg->idadePessoa) +
                      sizeof(reg->tamanhoNomePessoa) +
                      reg->tamanhoNomePessoa +
                      sizeof(reg->tamanhoNomeUsuario) +
                      reg->tamanhoNomeUsuario;

    // Calcula a quantidade de lixo (bytes de preenchimento) no final do registro.
    int lixo = reg->tamanhoRegistro - bytes_lidos;

    // Se houver lixo, avança o ponteiro do arquivo para pulá-lo.
    if (lixo > 0)
    {
        fseek(fp, lixo, SEEK_CUR);
    }

    *reg_out = reg;
    return 0;
}

// Construtor da struct RegistroPessoa
RegistroPessoa *cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200])
{
    RegistroPessoa *registroPessoa = malloc(sizeof(RegistroPessoa));

    registroPessoa->removido = '0';
    registroPessoa->idPessoa = idPessoa;
    registroPessoa->idadePessoa = idade;

    registroPessoa->tamanhoNomePessoa = strlen(nomePessoa);
    registroPessoa->nomePessoa = malloc(registroPessoa->tamanhoNomePessoa);
    memcpy(registroPessoa->nomePessoa, nomePessoa, registroPessoa->tamanhoNomePessoa);

    registroPessoa->tamanhoNomeUsuario = strlen(nomeUsuario);
    registroPessoa->nomeUsuario = malloc(registroPessoa->tamanhoNomeUsuario);
    memcpy(registroPessoa->nomeUsuario, nomeUsuario, registroPessoa->tamanhoNomeUsuario);

    registroPessoa->tamanhoRegistro = calcula_tamanho_registro_pessoa(registroPessoa);

    return registroPessoa;
}

// Libera a memória alocada para um registro de pessoa.
void destroi_registro_pessoa(RegistroPessoa *reg)
{
    if (reg == NULL)
        return;

    if (reg->nomePessoa != NULL)
        free(reg->nomePessoa);
    if (reg->nomeUsuario != NULL)
        free(reg->nomeUsuario);
    free(reg);
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

    if (fwrite(reg->nomeUsuario, reg->tamanhoNomeUsuario, 1, fp) < 1)
        return 1;

    return 0;
}

// Imprime um registro de pessoa formatado. Retorna 1 se o registro for nulo.
int imprime_registro_pessoa(RegistroPessoa *reg)
{
    if (reg == NULL)
        return 1;

    printf("Dados da pessoa de codigo %d\n", reg->idPessoa);

    printf("Nome: ");
    if (reg->tamanhoNomePessoa > 0)
    {
        fwrite(reg->nomePessoa, sizeof(char), reg->tamanhoNomePessoa, stdout);
    }
    else
    {
        printf("-");
    }
    printf("\n");

    if (reg->idadePessoa == -1)
    {
        printf("Idade: -\n");
    }
    else
    {
        printf("Idade: %d\n", reg->idadePessoa);
    }

    printf("Usuario: ");
    fwrite(reg->nomeUsuario, sizeof(char), reg->tamanhoNomeUsuario, stdout);

    printf("\n\n");

    return 0;
}

void inserir_pessoas(FILE *fp, RegistroBuscaPessoa **registros, int nInsercoes) {

    fseek(fp, 17, SEEK_SET);
    long long byteOffset = 17;

    for (int i = 0; i < nInsercoes; i++)
    {
        // propriedades do registro
        
        /*
        printf("Inserindo registro com id %d no byte offset %lld\n", registros[i]->registro->idPessoa, byteOffset);
        printf("  - Tamanho Registro: %d\n", registros[i]->registro->tamanhoRegistro);
        printf("  - Idade Pessoa: %d\n", registros[i]->registro->idadePessoa);
        printf("  - Tamanho Nome Pessoa: %d\n", registros[i]->registro->tamanhoNomePessoa);
        printf("  - Nome Pessoa: %s\n", registros[i]->registro->nomePessoa ? registros[i]->registro->nomePessoa : "NULL");
        printf("  - Tamanho Nome Usuario: %d\n", registros[i]->registro->tamanhoNomeUsuario);
        printf("  - Nome Usuario: %s\n", registros[i]->registro->nomeUsuario ? registros[i]->registro->nomeUsuario : "NULL");
        printf(" --- \n");
        */
            


        long long diffByteOffset = registros[i]->ByteOffset - byteOffset;
        fseek(fp, diffByteOffset, SEEK_CUR);
        
        escreve_registro_pessoa(fp, registros[i]->registro);

        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + registros[i]->registro->tamanhoRegistro;
        byteOffset += diffByteOffset + tamanho_real_escrito;
    }
}

int calcula_tamanho_registro_pessoa(RegistroPessoa *reg) {
    if (reg == NULL)
        return 0;

    return sizeof(int) + // idPessoa
           sizeof(int) + // idadePessoa
           sizeof(int) + // tamanhoNomePessoa
           reg->tamanhoNomePessoa +
           sizeof(int) + // tamanhoNomeUsuario
           reg->tamanhoNomeUsuario;
}