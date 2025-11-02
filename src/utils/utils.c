#include <stddef.h> // Para size_t
#include <stdint.h> // Para SIZE_MAX (para checagem de overflow)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

#define GOLDEN_RATIO 1.61803398875

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

void toggle_cabecalho_pessoa(FILE *fp, CabecalhoPessoa *cab) {
    fseek(fp, 0, SEEK_SET);

    cab->status = (cab->status == '0') ? '1' : '0';
    escreve_cabecalho_pessoa(fp, cab);
    return;
}

int le_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);

    if (fread(&cab->status, sizeof(char), 1, fp) < 1 || fread(&cab->lixo, sizeof(char), 11, fp) < 1)
    {
        return 1;
    }
    return 0;
}

void toggle_cabecalho_indice(FILE *fp, CabecalhoIndice *cab) {
    fseek(fp, 0, SEEK_SET);

    cab->status = (cab->status == '0') ? '1' : '0';
    escreve_cabecalho_indice(fp, cab);
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

// Escreve o cabeçalho no arquivo de dados. Retorna 1 em caso de erro.
int escreve_cabecalho_indice(FILE *fp, CabecalhoIndice *cab)
{
    fseek(fp, 0, SEEK_SET);

    if (fwrite(&cab->status, sizeof(char), 1, fp) < 1)
        return 1;
    if (fwrite(&cab->lixo, 1, 11, fp) < 1)
        return 1;

    return 0;
}

int escreve_cabecalho_segue(FILE *fp, CabecalhoSegue *cab)
{
    fwrite(&cab->status, sizeof(char), 1, fp);
    fwrite(&cab->quantidadePessoas, sizeof(unsigned), 1, fp);
    fwrite(&cab->proxRRN, sizeof(unsigned), 1, fp);

    return 0; // Sucesso.
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

    // se malloc não funcionou
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
    if (lixo > 0) {
        fseek(fp, lixo, SEEK_CUR);
    }

    *reg_out = reg;
    return 0;
}

// Construtor da struct RegistroPessoa
RegistroPessoa* cria_registro_pessoa(int idPessoa, char nomePessoa[200], int idade, char nomeUsuario[200])
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

    registroPessoa->tamanhoRegistro = sizeof(int) + sizeof(int) + sizeof(int) + registroPessoa->tamanhoNomePessoa + sizeof(int) + registroPessoa->tamanhoNomeUsuario;

    return registroPessoa;
}

// Libera a memória alocada para um registro de pessoa.
void destroi_registro(RegistroPessoa *reg)
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
    if (reg->tamanhoNomeUsuario > 0)
    {
        fwrite(reg->nomeUsuario, sizeof(char), reg->tamanhoNomeUsuario, stdout);
    }
    printf("\n\n");

    return 0;
}

int le_registro_segue(FILE *fp, RegistroSegue *reg_segue)
{
    fread(&reg_segue->removido, sizeof(char), 1, fp);
    if(reg_segue->removido == REGISTRO_SEGUE_REMOVIDO) {
        fp += REGISTRO_SEGUE_TAMANHO - 1; // Pula o resto do registro
        return 1;
    }

    fread(&reg_segue->idPessoaQueSegue, sizeof(int), 1, fp);
    fread(&reg_segue->idPessoaQueESeguida, sizeof(int), 1, fp);
    fread(reg_segue->dataInicioQueSegue, sizeof(char), 10, fp);
    fread(reg_segue->dataFimQueSegue, sizeof(char), 10, fp);
    fread(&reg_segue->grauAmizade, sizeof(char), 1, fp);

    return 0;
}

int escreve_registro_segue(FILE *fp, RegistroSegue *reg)
{
    if (reg == NULL)
        return 1;

    fwrite(&reg->removido, sizeof(char), 1, fp);
    fwrite(&reg->idPessoaQueSegue, sizeof(int), 1, fp);
    fwrite(&reg->idPessoaQueESeguida, sizeof(int), 1, fp);
    fwrite(reg->dataInicioQueSegue, sizeof(char), 10, fp);
    fwrite(reg->dataFimQueSegue, sizeof(char), 10, fp);
    fwrite(&reg->grauAmizade, sizeof(char), 1, fp);
    
    return 0; // Sucesso.
}

void imprime_registro_segue(RegistroSegue *reg) {
    if (reg == NULL) {
        printf("Registro nulo.\n");
        return;
    }

    printf("%d,%d,%s,", reg->idPessoaQueSegue, reg->idPessoaQueESeguida, reg->dataInicioQueSegue);
    if(reg->dataFimQueSegue[0] == NULO_CARACTERE) {
        printf(",");
    } else {
        printf("%s,", reg->dataFimQueSegue);
    }
    if(reg->grauAmizade == NULO_CARACTERE) {
        printf("\n");
    } else {
        printf("%c\n", reg->grauAmizade);
    }
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

// Lê um registro de dados do arquivo. Retorna 1 em caso de erro ou fim de arquivo.
int le_registro_indice(FILE *fp, RegistroIndice **reg_out)
{
    RegistroIndice *reg = malloc(sizeof(RegistroIndice));
    if (reg == NULL)
        return 1;

    if (fread(&reg->idPessoa, sizeof(int), 1, fp) == 0)
    {
        free(reg);
        return 1;
    }

    if (fread(&reg->byteOffset, sizeof(long long), 1, fp) == 0)
    {
        // Se o ficheiro acabar inesperadamente aqui (muito raro, mas possível)
        free(reg);
        return 1;
    }

    *reg_out = reg;
    return 0;
}

// Libera a memória alocada para um registro de índice.
void destroi_registro_indice(RegistroIndice *reg)
{
    if (reg == NULL)
        return;

    free(reg);
}

// Carrega todo o índice na memória, retornando um array dinâmico de ponteiros para os registros.
RegistroIndice **carregar_indice_inteiro(FILE *fp, int numeroRegistros)
{
    if (fp == NULL)
    {
        return NULL;
    }

    CabecalhoIndice cab;
    if (le_cabecalho_indice(fp, &cab) != 0)
    {
        return NULL;
    }

    // Move o cursor para o início dos registros
    // fseek(fp, 12, SEEK_SET);

    // Aloca um array dinâmico para armazenar os ponteiros dos registros
    RegistroIndice **registros = malloc(numeroRegistros * sizeof(RegistroIndice *));

    for (int i = 0; i < numeroRegistros; i++)
    {
        if (le_registro_indice(fp, &registros[i]) != 0)
        {
            // Em caso de erro, libera a memória alocada até o momento e retorna NULL
            for (int j = 0; j < i; j++)
            {
                free(registros[j]);
            }
            free(registros);
            return NULL;
        }
    }

    return registros;
}

int comparar_registros_busca_offset(const void *a, const void *b)
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

void imprimir_registros_raw(FILE *fp) {
    if (fp == NULL) {
        printf("Arquivo inválido.\n");
        return;
    }

    fseek(fp, 17, SEEK_SET); // Pula o cabeçalho
    printf("--- INICIO RAW PRINT ---\n");

    while (1) {
        long currentPos = ftell(fp);
        RegistroPessoa *reg = NULL;
        
        // Tenta ler o próximo registro
        if (le_registro_pessoa(fp, &reg) != 0) {
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
        if (reg->tamanhoNomePessoa > 0) {
            fwrite(reg->nomePessoa, 1, reg->tamanhoNomePessoa, stdout);
        }
        printf("\n");

        printf("  - Usuario (%d): ", reg->tamanhoNomeUsuario);
        if (reg->tamanhoNomeUsuario > 0) {
            fwrite(reg->nomeUsuario, 1, reg->tamanhoNomeUsuario, stdout);
        }
        printf("\n");

        // O lixo é a diferença entre o tamanho declarado e o tamanho real dos campos variáveis + fixos
        long lixo = reg->tamanhoRegistro - tamanho_real_lido;
        printf("  - Lixo: %ld bytes\n\n", lixo);

        // O lixo já é pulado pela função le_registro_pessoa.
        // Manter o fseek aqui causaria um pulo duplo.
        
        destroi_registro(reg);
    }

    printf("--- FIM RAW PRINT ---\n");
    fflush(stdout);
}

RegistroIndice **intercalar_indices(RegistroIndice **indice_antigo, int n_antigo, RegistroIndice **indice_novo, int n_novo) 
{
    int n_total = n_antigo + n_novo;
    RegistroIndice **indice_final = malloc(n_total * sizeof(RegistroIndice *));
    if (indice_final == NULL) {
        return NULL;
    }

    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n_antigo && j < n_novo) {
        if (indice_antigo[i]->idPessoa <= indice_novo[j]->idPessoa) {
            indice_final[k++] = indice_antigo[i++];
        } else {
            indice_final[k++] = indice_novo[j++];
        }
    }

    while (i < n_antigo) {
        indice_final[k++] = indice_antigo[i++];
    }
    while (j < n_novo) {
        indice_final[k++] = indice_novo[j++];
    }

    return indice_final;
}