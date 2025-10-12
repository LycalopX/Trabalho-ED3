#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f2.h"

void funcionalidade2(char* nomeArquivoCSV, char* nomeArquivoDados, char* nomeArquivoIndice) {

    char caminhoCompleto[200];

    sprintf(caminhoCompleto, "data/%s", nomeArquivoCSV);
    FILE *fp_csv = fopen(caminhoCompleto, "r");
    

    sprintf(caminhoCompleto, "data/%s", nomeArquivoDados);
    FILE *fp_data = fopen(caminhoCompleto, "r");


    sprintf(caminhoCompleto, "data/%s", nomeArquivoIndice);
    FILE *fp_index = fopen(caminhoCompleto, "r");


    if (fp_csv == NULL || fp_data == NULL || fp_index == NULL) {
        printf("Falha no processamento do arquivo.\n");
        
        // Fecha os arquivos que possam ter sido abertos
        if (fp_csv != NULL) fclose(fp_csv);
        if (fp_data != NULL) fclose(fp_data);
        if (fp_index != NULL) fclose(fp_index);
        
        return;
    }

    CabecalhoPessoa *data_header = malloc(sizeof(CabecalhoPessoa));
    CabecalhoIndice *index_header = malloc(sizeof(CabecalhoIndice));

    data_header->status = '0';
    index_header->status = '0';

    fread(data_header, sizeof(CabecalhoPessoa), 1, fp_data);
    fread(index_header, sizeof(CabecalhoIndice), 1, fp_index);


}