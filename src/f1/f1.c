#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f1.h"

void funcionalidade1(char *nomeArquivoIndice)
{
    
    char caminhoCompleto[200];
    sprintf(caminhoCompleto, "%s", nomeArquivoIndice);

    // Usa o caminho completo para abrir/criar o arquivo.
    FILE *fp = fopen(caminhoCompleto, "wb");

    if (fp == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Preenche a struct do cabeçalho
    CabecalhoIndice cab;
    cab.status = '1';
    memset(cab.lixo, '$', sizeof(cab.lixo));

    if (fwrite(&cab, sizeof(CabecalhoIndice), 1, fp) < 1)
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    fclose(fp);

    binarioNaTela(caminhoCompleto);
}
