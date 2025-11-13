#include <stdio.h>
#include <stdlib.h>

#include "f3.h"
#include "../arquivos.h"
#include "../utils/utils.h"

#include "../data_manip/pessoa.h"

// Implementa a funcionalidade 3: Exibe todos os registros de pessoa não removidos.
void funcionalidade3(FILE *fp)
{

    CabecalhoPessoa cab;
    if (le_cabecalho_pessoa(fp, &cab) != 0 || cab.status == '0')
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        return;
    }

    if (cab.quantidadePessoas == 0)
    {
        printf("Registro inexistente.\n\n");
        return;
    }

    fseek(fp, 17, SEEK_SET);

    int registrosEncontrados = 0;
    for (int i = 0; i < cab.quantidadePessoas + cab.quantidadeRemovidos; i++)
    {
        RegistroPessoa *reg = NULL;

        if (le_registro_pessoa(fp, &reg) == 0)
        {
            if (reg->removido == '0')
            {
                imprime_registro_pessoa(reg);
                registrosEncontrados++;
            }
            destroi_registro_pessoa(reg);
        }
        else
        {

            break;
        }
    }

    if (registrosEncontrados == 0)
    {
        printf("Registro inexistente.\n\n");
    }
}
