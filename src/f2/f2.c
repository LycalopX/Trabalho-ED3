#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f2.h"

// Função de comparação, necessária para o qsort ordenar os índices por idPessoa.
int comparar_indices(const void *a, const void *b)
{
    RegistroIndice *regA = (RegistroIndice *)a;
    RegistroIndice *regB = (RegistroIndice *)b;

    return (regA->idPessoa - regB->idPessoa);
}

// Função auxiliar para interpretar uma linha do CSV e preencher uma struct RegistroPessoa.
int parse_csv_line(char *line, RegistroPessoa *reg)
{
    // Remove quebras de linha (\n ou \r) do final da string para evitar erros de parsing.
    line[strcspn(line, "\r\n")] = 0;

    char *p = line;
    char *virgula;

    // 1. Lê o campo idPessoa.
    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1; // Retorna erro se a linha for mal formatada.

    *virgula = '\0'; // Substitui a vírgula por um terminador nulo para isolar o campo.
    reg->idPessoa = atoi(p);
    p = virgula + 1; // Avança o ponteiro para o início do próximo campo.

    // 2. Lê o campo nomePessoa.
    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;
    if (p == virgula) // Verifica se o campo está vazio (ex: ",,").
    {
        reg->tamanhoNomePessoa = 0;
        reg->nomePessoa = NULL;
    }
    else
    {
        *virgula = '\0';
        reg->tamanhoNomePessoa = strlen(p);
        reg->nomePessoa = malloc(reg->tamanhoNomePessoa);
        memcpy(reg->nomePessoa, p, reg->tamanhoNomePessoa);
    }
    p = virgula + 1;

    // 3. Lê o campo idadePessoa.
    virgula = strchr(p, ',');
    if (virgula == NULL)
        return 1;
    if (p == virgula) // Se o campo for nulo, define a idade como -1.
    {
        reg->idadePessoa = -1;
    }
    else
    {
        *virgula = '\0';
        reg->idadePessoa = atoi(p);
    }
    p = virgula + 1;

    // 4. Lê o campo nomeUsuario (último campo da linha).
    if (*p == '\0') // Verifica se o campo está vazio.
    {
        reg->tamanhoNomeUsuario = 0;
        reg->nomeUsuario = NULL;
    }
    else
    {
        reg->tamanhoNomeUsuario = strlen(p);
        reg->nomeUsuario = malloc(reg->tamanhoNomeUsuario);
        memcpy(reg->nomeUsuario, p, reg->tamanhoNomeUsuario);
    }

    return 0; // Retorna 0 em caso de sucesso.
}

void funcionalidade2(char *nomeArquivoCSV, char *nomeArquivoDados, char *nomeArquivoIndice)
{
    // Prepara os caminhos para os ficheiros de entrada (CSV) e de saída (binários).
    char caminhoCSV[200], caminhoDados[200], caminhoIndice[200];
    sprintf(caminhoCSV, "%s", nomeArquivoCSV);
    sprintf(caminhoDados, "%s", nomeArquivoDados);
    sprintf(caminhoIndice, "%s", nomeArquivoIndice);

    // Abre os três ficheiros nos modos corretos.
    FILE *fp_csv = fopen(caminhoCSV, "r");
    FILE *fp_data = fopen(caminhoDados, "wb+"); // (wb+) para usar fseek e ftell
    FILE *fp_index = fopen(caminhoIndice, "rb+");
    
    if (fp_csv == NULL || fp_data == NULL || fp_index == NULL)
    {
        printf("Falha no processamento do arquivo.\n");

        if (fp_csv != NULL)
            fclose(fp_csv);
        if (fp_data != NULL)
            fclose(fp_data);
        if (fp_index != NULL)
            fclose(fp_index);

        return;
    }

    // Declara e inicializa os cabeçalhos.
    CabecalhoPessoa data_header;
    CabecalhoIndice index_header;

    data_header.status = '0';
    data_header.quantidadePessoas = 0;
    data_header.quantidadeRemovidos = 0;
    data_header.proxByteOffset = 17;

    // Lê o cabeçalho do ficheiro de índice (que já deve existir) e marca-o como inconsistente.
    le_cabecalho_indice(fp_index, &index_header);
    index_header.status = '0';

    // Escreve os cabeçalhos iniciais nos ficheiros binários.
    escreve_cabecalho_pessoa(fp_data, &data_header);
    escreve_cabecalho_indice(fp_index, &index_header);

    char buffer[256];
    fgets(buffer, sizeof(buffer), fp_csv); // Pula a primeira linha (cabeçalho) do ficheiro CSV.

    fseek(fp_data, 0, SEEK_END); // Posiciona o cursor no final do ficheiro de dados para começar a escrita.

    // Prepara o array dinâmico em memória para armazenar o índice.
    RegistroIndice *regs_index = malloc(100 * sizeof(RegistroIndice));
    int regs_length = 100;
    int index = 0;
    int byteOffset = ftell(fp_data);

    // Itera sobre cada linha de dados do ficheiro CSV.
    while (fgets(buffer, sizeof(buffer), fp_csv))
    {
        // Verifica se a alocação do array de índice falhou.
        if (regs_index == NULL)
        {
            printf("Falha no processamento do arquivo.\n");

            fclose(fp_csv);
            fclose(fp_data);
            fclose(fp_index);

            free(regs_index);

            return;
        }

        RegistroPessoa reg_pessoa;

        // Zera os ponteiros para evitar libertar memória inválida se o parse falhar.
        reg_pessoa.nomePessoa = NULL;
        reg_pessoa.nomeUsuario = NULL;

        // Chama a função de parse. Se falhar, pula para a próxima linha do CSV.
        if (parse_csv_line(buffer, &reg_pessoa) != 0)
        {
            continue;
        }

        // Preenche os campos de controlo do registro.
        reg_pessoa.removido = '0';
        reg_pessoa.tamanhoRegistro = sizeof(char) + sizeof(int) * 5 + reg_pessoa.tamanhoNomePessoa + reg_pessoa.tamanhoNomeUsuario;

        // Expande o array de índice em memória se a capacidade for atingida.
        if (regs_length < index + 1)
        {
            regs_length = regs_length * 1.618; // Expansão geométrica.
            regs_index = realloc(regs_index, regs_length * sizeof(RegistroIndice));
        }

        // Adiciona a nova entrada de índice (ID, offset) ao array em memória.
        regs_index[index].idPessoa = reg_pessoa.idPessoa;
        regs_index[index].byteOffset = byteOffset;

        // Atualiza o byteOffset manualmente para o próximo registro.
        byteOffset += reg_pessoa.tamanhoRegistro;

        // Escreve o registro de pessoa completo no ficheiro de dados.
        escreve_registro_pessoa(fp_data, &reg_pessoa);

        // Liberta a memória alocada para as strings temporárias.
        free(reg_pessoa.nomePessoa);
        free(reg_pessoa.nomeUsuario);

        index++;
    }

    // Atualiza os campos do cabeçalho de dados com os valores finais.
    data_header.quantidadePessoas = index;
    data_header.proxByteOffset = ftell(fp_data);

    // Ordena o array de índice em memória usando qsort.
    qsort(regs_index, index, sizeof(RegistroIndice), comparar_indices);

    // Escreve o conteúdo do array (agora ordenado) no ficheiro de índice, campo a campo.
    for (int i = 0; i < data_header.quantidadePessoas; i++)
    {
        fwrite(&regs_index[i].idPessoa, sizeof(int), 1, fp_index);
        fwrite(&regs_index[i].byteOffset, sizeof(long long), 1, fp_index);
    }

    // Atualiza os status dos cabeçalhos para '1' (consistente).
    data_header.status = '1';
    index_header.status = '1';

    // Reescreve os cabeçalhos atualizados nos ficheiros.
    escreve_cabecalho_pessoa(fp_data, &data_header);
    escreve_cabecalho_indice(fp_index, &index_header);

    // Exibe os ficheiros binários gerados na tela, conforme a especificação.
    binarioNaTela(caminhoDados);
    binarioNaTela(caminhoIndice);

    // Fecha todos os ficheiros.
    fclose(fp_csv);
    fclose(fp_data);
    fclose(fp_index);

    // Liberta a memória alocada para o array de índice.
    free(regs_index);
}
