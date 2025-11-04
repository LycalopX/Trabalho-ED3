#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utilsT1.h"
#include "utils.h" // For shared functions like destroi_registro_pessoa

// FUNÇÕES STRUCT PESSOA

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

// DEBUG
void imprimir_registros_raw(FILE *fp)
{
    if (fp == NULL)
    {
        printf("Arquivo inválido.\n");
        return;
    }

    fseek(fp, 17, SEEK_SET); // Pula o cabeçalho
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
        long tamanho_real_lido = sizeof(reg->idPessoa) +
                                 sizeof(reg->idadePessoa) +
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
