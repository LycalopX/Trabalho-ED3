#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f1.h"

// Implementa a funcionalidade 1, responsável pela criação de um arquivo de índice primário.
void funcionalidade1(FILE *fp)
{

    CabecalhoIndice cab;
    cab.status = '1';
    memset(cab.lixo, '$', sizeof(cab.lixo));

    fseek(fp, 0, SEEK_SET);
    if (fwrite(&cab.status, sizeof(char), 1, fp) < 1 || fwrite(&cab.lixo, sizeof(char), 11, fp) < 1)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    fflush(fp);
}
