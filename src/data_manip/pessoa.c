#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pessoa.h"
#include "indice.h"

// --- Funções de Manipulação de RegistroPessoa ---

// Lê o cabeçalho do arquivo de dados de pessoas.
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

    return 0; // Sucesso.
}

// Alterna o status do cabeçalho entre '0' e '1' e o reescreve no arquivo.
void toggle_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab)
{
    fseek(fp, 0, SEEK_SET);

    cab->status = (cab->status == '0') ? '1' : '0';
    escreve_cabecalho_pessoa(fp, cab);
    return;
}

// Escreve o cabeçalho no arquivo de dados de pessoas.
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

    return 0; // Sucesso.
}

// Lê um registro de pessoa do arquivo, incluindo campos de tamanho variável.
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

    // Lê o campo nomePessoa (tamanho variável).
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
        reg->nomePessoa = NULL; // Campo nulo.
    }

    // Lê o campo nomeUsuario (tamanho variável).
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
    if (lixo > 0)
    {
        fseek(fp, lixo, SEEK_CUR); // Pula os bytes de lixo.
    }

    *reg_out = reg;
    return 0; // Sucesso.
}

// Constrói e aloca um novo registro de pessoa com os dados fornecidos.
RegistroPessoa *cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200])
{
    RegistroPessoa *registroPessoa = malloc(sizeof(RegistroPessoa));
    if (registroPessoa == NULL) return NULL; // Falha na alocação.

    // Inicializa os campos fixos.
    registroPessoa->removido = '0';
    registroPessoa->idPessoa = idPessoa;
    registroPessoa->idadePessoa = idade;

    // Aloca e copia os campos de string.
    registroPessoa->tamanhoNomePessoa = strlen(nomePessoa);
    registroPessoa->nomePessoa = malloc(registroPessoa->tamanhoNomePessoa);
    memcpy(registroPessoa->nomePessoa, nomePessoa, registroPessoa->tamanhoNomePessoa);

    registroPessoa->tamanhoNomeUsuario = strlen(nomeUsuario);
    registroPessoa->nomeUsuario = malloc(registroPessoa->tamanhoNomeUsuario);
    memcpy(registroPessoa->nomeUsuario, nomeUsuario, registroPessoa->tamanhoNomeUsuario);

    // Calcula o tamanho total do registro.
    registroPessoa->tamanhoRegistro = calcula_tamanho_registro_pessoa(registroPessoa);

    return registroPessoa;
}

// Libera toda a memória alocada dinamicamente para um registro de pessoa.
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

// Escreve um registro de pessoa no arquivo, campo por campo.
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

    // Escreve o nome da pessoa se não for nulo.
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

// Imprime os dados de um registro de pessoa na saída padrão, formatando valores nulos.
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

// Insere um array de registros de pessoa no arquivo de dados, respeitando os byte offsets.
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
        
        // Escreve o registro no arquivo.
        escreve_registro_pessoa(fp, registros[i]->registro);

        // Atualiza o byte offset atual.
        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + registros[i]->registro->tamanhoRegistro;
        byteOffset += diffByteOffset + tamanho_real_escrito;
    }
}

// Calcula o tamanho total de um registro de pessoa no disco, incluindo campos de tamanho variável.
int calcula_tamanho_registro_pessoa(RegistroPessoa *reg) {
    if (reg == NULL)
        return 0;

    // Soma os tamanhos dos campos fixos e variáveis.
    return sizeof(int) + // idPessoa
           sizeof(int) + // idadePessoa
           sizeof(int) + // tamanhoNomePessoa
           reg->tamanhoNomePessoa +
           sizeof(int) + // tamanhoNomeUsuario
           reg->tamanhoNomeUsuario;
}