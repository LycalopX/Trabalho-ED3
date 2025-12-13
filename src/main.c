// Alexandre Brenner Weber - 15436911
// André Luis Santos Messias - 15493857


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "arquivos.h"
#include "utils/utils.h"
#include "data_manip/pessoa.h"
#include "f1/f1.h"
#include "f2/f2.h"
#include "f3/f3.h"
#include "f4/f4.h"
#include "f5/f5.h"
#include "f6/f6.h"
#include "f7/f7.h"
#include "f8/f8.h"
#include "f9/f9.h"
#include "f10/f10.h"

void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

// Função principal do programa, responsável por gerenciar as funcionalidades.
int main()
{
    setbuf(stdout, NULL);

    int funcionalidade;
    scanf("%d", &funcionalidade);

    // Seleciona a funcionalidade a ser executada com base na entrada do usuário.
    switch (funcionalidade)
    {
    case 1:
    {
        char nomeArquivoIndice[100];
        scanf("%s", nomeArquivoIndice);

        FILE *fp = fopen(nomeArquivoIndice, "wb");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        funcionalidade1(fp);
        fclose(fp);
        binarioNaTela(nomeArquivoIndice);
        break;
    }
    case 2:
    {
        char nomeArquivoCSV[100];
        char nomeArquivoDados[100];
        char nomeArquivoIndice[100];
        scanf("%s %s %s", nomeArquivoCSV, nomeArquivoDados, nomeArquivoIndice);

        FILE *fp_csv = fopen(nomeArquivoCSV, "r");
        FILE *fp_data = fopen(nomeArquivoDados, "wb");
        FILE *fp_index = fopen(nomeArquivoIndice, "rb+");

        if (fp_csv == NULL || fp_data == NULL || fp_index == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            if (fp_csv != NULL)
                fclose(fp_csv);
            if (fp_data != NULL)
                fclose(fp_data);
            if (fp_index != NULL)
                fclose(fp_index);
            break;
        }

        funcionalidade2(fp_csv, fp_data, fp_index, nomeArquivoDados, nomeArquivoIndice);

        fclose(fp_csv);
        fclose(fp_data);
        fclose(fp_index);

        binarioNaTela(nomeArquivoDados);
        binarioNaTela(nomeArquivoIndice);

        break;
    }
    case 3:
    {
        char nomeArquivoDeRegistro[100];
        scanf("%s", nomeArquivoDeRegistro);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        funcionalidade3(fp);
        fclose(fp);
        break;
    }
    case 4:
    {
        char nomeArquivoDeRegistro[100];
        char nomeArquivoIndice[100];
        scanf("%s", nomeArquivoDeRegistro);
        scanf("%s", nomeArquivoIndice);

        int buscas;
        scanf("%d", &buscas);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        FILE *fpIndice = fopen(nomeArquivoIndice, "rb");
        if (fpIndice == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            fclose(fp);
            break;
        }

        int nRegsEncontrados = 0;
        ResultadoBuscaPessoa *resultados = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 0, 0);

        
        if (resultados != NULL)
        {
            for (int i = 0; i < buscas; i++)
            {
                for (int j = 0; j < resultados[i].nRegistros; j++)
                {
                    imprime_registro_pessoa(resultados[i].registrosBusca[j]->registro);
                    
                    destroi_registro_pessoa(resultados[i].registrosBusca[j]->registro);
                }
                free(resultados[i].registrosBusca);
            }
            free(resultados); 
        }

        fclose(fp);
        fclose(fpIndice);
        break;
    }
    case 5:
    {
        char nomeArquivoDeRegistro[100];
        char nomeArquivoIndice[100];
        int buscas;

        scanf("%s %s %d", nomeArquivoDeRegistro, nomeArquivoIndice, &buscas);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb+");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        FILE *fpIndice = fopen(nomeArquivoIndice, "rb+");
        if (fpIndice == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            fclose(fp);
            break;
        }

        funcionalidade5(fp, fpIndice, nomeArquivoIndice, buscas);

        fclose(fp);
        fclose(fpIndice);

        binarioNaTela(nomeArquivoDeRegistro);
        binarioNaTela(nomeArquivoIndice);
        break;
    }
    case 6:
    {
        char nomeArquivoDeRegistro[100];
        char nomeArquivoIndice[100];
        int buscas;

        scanf("%s %s %d", nomeArquivoDeRegistro, nomeArquivoIndice, &buscas);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb+");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        FILE *fpIndice = fopen(nomeArquivoIndice, "rb+");
        if (fpIndice == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            fclose(fp);
            break;
        }

        funcionalidade6(fp, fpIndice, nomeArquivoIndice, buscas);

        fclose(fp);
        fclose(fpIndice);

        binarioNaTela(nomeArquivoDeRegistro);
        binarioNaTela(nomeArquivoIndice);
        break;
    }
    case 7:
    {
        char nomeArquivoDeRegistro[100];
        char nomeArquivoIndice[100];
        int buscas;

        scanf("%s %s %d", nomeArquivoDeRegistro, nomeArquivoIndice, &buscas);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb+");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        FILE *fpIndice = fopen(nomeArquivoIndice, "rb+");
        if (fpIndice == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            fclose(fp);
            break;
        }

        funcionalidade7(fp, fpIndice, nomeArquivoIndice, buscas);

        fclose(fp);
        fclose(fpIndice);

        binarioNaTela(nomeArquivoDeRegistro);
        binarioNaTela(nomeArquivoIndice);
        break;
    }

    case 8:
    {
        char nomeArquivoCSV[100];
        char nomeArquivoBinario[100];

        scanf("%s %s", nomeArquivoCSV, nomeArquivoBinario);

        FILE *fp_csv = fopen(nomeArquivoCSV, "r");
        if (fp_csv == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        FILE *fp_bin = fopen(nomeArquivoBinario, "wb");
        if (fp_bin == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            fclose(fp_csv);
            break;
        }

        int result = funcionalidade8(fp_csv, fp_bin);
        if (result != 0)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
        }

        fclose(fp_csv);
        fclose(fp_bin);

        binarioNaTela(nomeArquivoBinario);
        break;
    }
    case 9:
    {
        char nomeArquivoDeRegistro[100];
        char nomeArquivoDestino[100];

        scanf("%s %s", nomeArquivoDeRegistro, nomeArquivoDestino);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb");
        if (fp == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            break;
        }

        FILE *fpDestino = fopen(nomeArquivoDestino, "wb");
        if (fpDestino == NULL)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
            fclose(fp);
            break;
        }

        if (funcionalidade9(fp, fpDestino) != 0)
        {
            printf(FALHA_AO_PROCESSAR_ARQUIVO);
        }

        fclose(fp);
        fclose(fpDestino);

        binarioNaTela(nomeArquivoDestino);
        break;
    }
    case 10:
    {
        char nomeArquivoDeRegistro[100];
        char nomeArquivoIndice[100];
        char nomeArquivoSegue[100];
        int buscas;

        scanf("%s %s %s %d", nomeArquivoDeRegistro, nomeArquivoIndice, nomeArquivoSegue, &buscas);

        FILE *fp = fopen(nomeArquivoDeRegistro, "rb+");
        if (fp == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        FILE *fpIndice = fopen(nomeArquivoIndice, "rb+");
        if (fpIndice == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            fclose(fp);
            break;
        }

        FILE *fpSegue = fopen(nomeArquivoSegue, "rb");
        if (fpSegue == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            fclose(fp);
            fclose(fpIndice);
            break;
        }

        int result = funcionalidade10(fp, fpIndice, fpSegue, buscas);

        

        if(result != 0) {
            printf("Falha no processamento do arquivo. Erro: %d\n", result);
        }

        fclose(fp);
        fclose(fpIndice);
        fclose(fpSegue);
        break;
    }
    default:
    {
        printf("Funcionalidade inválida!\n");
        exit(0);
    }
    }

    return 0;
}
