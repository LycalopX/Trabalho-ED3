#include <stdio.h>
#include <stdlib.h>

#include "f3.h"
#include "../arquivos.h"
#include "../utils/utils.h"

#include "../data_manip/pessoa.h"

void funcionalidade3(FILE *fp) 
{

    // Lê o cabeçalho para saber o número de registros
    CabecalhoPessoa cab;
    if (le_cabecalho_pessoa(fp, &cab) != 0 || cab.status == '0') 
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    // Se não houver registros, imprime a mensagem apropriada
    if (cab.quantidadePessoas == 0) {
        printf("Registro inexistente.\n\n");
        return;
    }

    // Posiciona o cursor no início dos registros de dados
    fseek(fp, 17, SEEK_SET);

    // Itera sobre os registros do arquivo
    int registrosEncontrados = 0;
    for (int i = 0; i < cab.quantidadePessoas + cab.quantidadeRemovidos; i++) 
    {
        RegistroPessoa *reg = NULL;

        if (le_registro_pessoa(fp, &reg) == 0) { // Se a leitura for bem sucedida
            if (reg->removido == '0') { // E o registro não estiver removido
                imprime_registro_pessoa(reg);
                registrosEncontrados++;
            }
            destroi_registro_pessoa(reg); // Destroi o registro temporário
        } else {
            // Se le_registro_pessoa falhar, pode ser o fim do arquivo
            break;
        }
    }

    if (registrosEncontrados == 0) {
        printf("Registro inexistente.\n\n");
    }
}
