#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f1.h"

void funcionalidade1(FILE *fp)
{
    // Preenche a struct do cabeçalho
    CabecalhoIndice cab;
    cab.status = '1';
    memset(cab.lixo, '$', sizeof(cab.lixo));

    // Escreve o cabeçalho no início do arquivo
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&cab, sizeof(CabecalhoIndice), 1, fp) < 1)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    // Força o buffer a ser escrito no disco para que binarioNaTela possa ler
    fflush(fp);
}
