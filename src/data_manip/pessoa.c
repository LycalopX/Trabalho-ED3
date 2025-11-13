#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pessoa.h"
#include "indice.h"

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

// Lê um registro de pessoa de tamanho variável do arquivo.
// A função aloca memória para o registro e seus campos.
// Ao final, avança o cursor do arquivo para pular o lixo (padding).
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

        reg->nomePessoa = malloc(reg->tamanhoNomePessoa + 1);
        if (reg->nomePessoa == NULL)
        {
            free(reg);
            return 1;
        }
        fread(reg->nomePessoa, reg->tamanhoNomePessoa, 1, fp);
        reg->nomePessoa[reg->tamanhoNomePessoa] = '\0';
    }
    else
    {
        reg->nomePessoa = NULL;
    }

    fread(&reg->tamanhoNomeUsuario, sizeof(int), 1, fp);

    reg->nomeUsuario = malloc(reg->tamanhoNomeUsuario + 1);
    if (reg->nomeUsuario == NULL)
    {
        if (reg->nomePessoa != NULL)
            free(reg->nomePessoa);
        free(reg);
        return 1;
    }
    fread(reg->nomeUsuario, reg->tamanhoNomeUsuario, 1, fp);
    reg->nomeUsuario[reg->tamanhoNomeUsuario] = '\0';

    int bytes_lidos = sizeof(reg->idPessoa) +
                      sizeof(reg->idadePessoa) +
                      sizeof(reg->tamanhoNomePessoa) +
                      reg->tamanhoNomePessoa +
                      sizeof(reg->tamanhoNomeUsuario) +
                      reg->tamanhoNomeUsuario;

    int lixo = reg->tamanhoRegistro - bytes_lidos;
    if (lixo > 0)
    {
        fseek(fp, lixo, SEEK_CUR);
    }

    *reg_out = reg;
    return 0;
}

// Aloca e inicializa uma nova struct RegistroPessoa em memória.
RegistroPessoa *cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200])
{
    RegistroPessoa *registroPessoa = malloc(sizeof(RegistroPessoa));
    if (registroPessoa == NULL)
        return NULL;

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

// Cria uma cópia profunda (deep copy) de um RegistroPessoa.
RegistroPessoa *copia_registro_pessoa(RegistroPessoa *reg)
{
    if (reg == NULL)
        return NULL;

    RegistroPessoa *copia = malloc(sizeof(RegistroPessoa));
    if (copia == NULL)
        return NULL;

    copia->removido = reg->removido;
    copia->tamanhoRegistro = reg->tamanhoRegistro;
    copia->idPessoa = reg->idPessoa;
    copia->idadePessoa = reg->idadePessoa;
    copia->tamanhoNomePessoa = reg->tamanhoNomePessoa;
    copia->tamanhoNomeUsuario = reg->tamanhoNomeUsuario;

    if (reg->nomePessoa != NULL && reg->tamanhoNomePessoa > 0)
    {
        copia->nomePessoa = malloc(reg->tamanhoNomePessoa + 1);
        if (copia->nomePessoa == NULL)
        {
            free(copia);
            return NULL;
        }
        memcpy(copia->nomePessoa, reg->nomePessoa, reg->tamanhoNomePessoa);
        copia->nomePessoa[reg->tamanhoNomePessoa] = '\0';
    }
    else
    {
        copia->nomePessoa = NULL;
    }

    if (reg->nomeUsuario != NULL && reg->tamanhoNomeUsuario > 0)
    {
        copia->nomeUsuario = malloc(reg->tamanhoNomeUsuario + 1);
        if (copia->nomeUsuario == NULL)
        {
            if (copia->nomePessoa != NULL)
                free(copia->nomePessoa);
            free(copia);
            return NULL;
        }
        memcpy(copia->nomeUsuario, reg->nomeUsuario, reg->tamanhoNomeUsuario);
        copia->nomeUsuario[reg->tamanhoNomeUsuario] = '\0';
    }
    else
    {
        copia->nomeUsuario = NULL;
    }

    return copia;
}

void escreve_string_char_por_char(FILE *fp, const char *str, int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        fputc(str[i], fp);
    }
}

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
        escreve_string_char_por_char(fp, reg->nomePessoa, reg->tamanhoNomePessoa);
    }

    if (fwrite(&reg->tamanhoNomeUsuario, sizeof(int), 1, fp) < 1)
        return 1;

    escreve_string_char_por_char(fp, reg->nomeUsuario, reg->tamanhoNomeUsuario);

    return 0;
}

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

// Itera sobre uma lista de registros e os insere no arquivo de dados
// nas posições corretas, de acordo com os byte offsets pré-calculados.
void inserir_pessoas(FILE *fp, RegistroBuscaPessoa **registros, int nInsercoes)
{

    fseek(fp, 17, SEEK_SET);
    long long byteOffset = 17;

    for (int i = 0; i < nInsercoes; i++)
    {

        long long diffByteOffset = registros[i]->ByteOffset - byteOffset;
        fseek(fp, diffByteOffset, SEEK_CUR);

        escreve_registro_pessoa(fp, registros[i]->registro);

        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + registros[i]->registro->tamanhoRegistro;
        byteOffset += diffByteOffset + tamanho_real_escrito;
    }
}

// Calcula o tamanho da porção de dados de um registro,
// que corresponde ao tamanho total menos os campos de controle 'removido' e 'tamanhoRegistro'.
int calcula_tamanho_registro_pessoa(RegistroPessoa *reg)
{
    if (reg == NULL)
        return 0;

    return sizeof(int) +
           sizeof(int) +
           sizeof(int) +
           reg->tamanhoNomePessoa +
           sizeof(int) +
           reg->tamanhoNomeUsuario;
}