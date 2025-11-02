#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f9.h"

// Compara a e b e retorna um valor negativo se a < b, zero se a == b, e positivo se a > b
int comparar_registros(const void *a, const void *b) {
    const RegistroSegue *registroA = (const RegistroSegue *)a;
    const RegistroSegue *registroB = (const RegistroSegue *)b;

    // idPessoaQueSegue
    if (registroA->idPessoaQueSegue != registroB->idPessoaQueSegue) {
        return registroA->idPessoaQueSegue - registroB->idPessoaQueSegue;
    }
    
    // idPessoaQueESeguida
    if (registroA->idPessoaQueESeguida != registroB->idPessoaQueESeguida) {
        return registroA->idPessoaQueESeguida - registroB->idPessoaQueESeguida;
    }

    // dataInicioQueSegue
    int dataInicioComp = strncmp(registroA->dataInicioQueSegue, registroB->dataInicioQueSegue, 10);
    if (dataInicioComp != 0) {
        if(registroA->dataInicioQueSegue[0] == NULO_CARACTERE) return 1;
        else if(registroB->dataInicioQueSegue[0] == NULO_CARACTERE) return -1;
        else return dataInicioComp;
    }
    
    // dataFimQueSegue
    int dataFimComp = strncmp(registroA->dataFimQueSegue, registroB->dataFimQueSegue, 10);
    if (dataFimComp != 0) {
        if(registroA->dataFimQueSegue[0] == NULO_CARACTERE) return 1;
        else if(registroB->dataFimQueSegue[0] == NULO_CARACTERE) return -1;
        else return dataFimComp;
    }

    // grauAmizade
    if(registroA->grauAmizade == NULO_CARACTERE) return 1;
    else if(registroB->grauAmizade == NULO_CARACTERE) return -1;
    return registroA->grauAmizade - registroB->grauAmizade;
}

// Ordena os registros do arquivo de dados segue e grava no arquivo de destino
int funcionalidade9(FILE *fp, FILE *fpDestino) {
    // Move o ponteiro para o início do arquivo
    fseek(fp, 0, SEEK_SET);

    // Lê o cabeçalho
    CabecalhoSegue *cabecalho = malloc(sizeof(CabecalhoSegue));
    fread(&cabecalho->status, sizeof(char), 1, fp);
    fread(&cabecalho->quantidadePessoas, sizeof(int), 1, fp);
    fread(&cabecalho->proxRRN, sizeof(int), 1, fp);

    // DEBUG: printa o cabeçalho lido
    //printf("Cabeçalho lido:\n - Status: %c\n - Quantidade Pessoas: %u\n - Próximo RRN: %u\n\n", cabecalho->status, cabecalho->quantidadePessoas, cabecalho->proxRRN);

    // Aloca memória para os registros
    RegistroSegue *registros = malloc(cabecalho->quantidadePessoas * sizeof(RegistroSegue));
    if (registros == NULL) {
        return -1; // Falha na alocação de memória
    }

    // Lê os registros
    for (int i = 0; i < cabecalho->quantidadePessoas; i++) {
        int result = le_registro_segue(fp, &registros[i]);
        if(result == 1) // Registro removido
        {
            i--; // Decrementa i para compensar o registro removido
            continue;
        }
        
        // DEBUG: printa o registro lido
        //imprime_registro_segue(&registros[i]);
        //printf("\n");
    }

    // Ordena os registros usando qsort (biblioteca C) e a função de comparação
    qsort(registros, cabecalho->quantidadePessoas, sizeof(RegistroSegue), comparar_registros);

    // Escreve o cabeçalho no arquivo de destino
    fwrite(cabecalho, sizeof(CabecalhoSegue), 1, fpDestino);

    // Escreve os registros ordenados no arquivo de destino
    for (int i = 0; i < cabecalho->quantidadePessoas; i++) 
    {
        //imprime_registro_segue(&registros[i]); // DEBUG: printa o registro escrito
        escreve_registro_segue(fpDestino, &registros[i]);
    }

    // Libera a memória alocada
    free(registros);
    free(cabecalho);

    return 0; // Sucesso
}