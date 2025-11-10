#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f1.h"

// Implementa a funcionalidade 1: Criação de um arquivo de índice primário.
void funcionalidade1(FILE *fp)
{
    // Inicializa a estrutura do cabeçalho do índice.
    // O status é definido como '1' (consistente) pois a criação do cabeçalho é uma operação atômica.
    // O restante do cabeçalho é preenchido com caracteres '$' conforme especificado.
    CabecalhoIndice cab;
    cab.status = '1';
    memset(cab.lixo, '$', sizeof(cab.lixo));

    // Posiciona o cursor no início do arquivo e escreve o cabeçalho.
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&cab, sizeof(CabecalhoIndice), 1, fp) < 1)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    // Garante que o cabeçalho seja gravado no disco imediatamente.
    fflush(fp);
}
