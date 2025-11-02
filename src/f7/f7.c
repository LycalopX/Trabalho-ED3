#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arquivos.h"
#include "../utils/utils.h"
#include "f7.h"
#include "../f4/f4.h"

int funcionalidade7(FILE *fp, FILE *fpIndice, int buscas)
{
    /*
    Permita  a  atualização  dos  registros  do  arquivo  de  dados  pessoa
    */
    
    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 1);

    int index = 0;
    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));

    // Imprime os resultados e libera a memória
    if (resultadosEmBuscas != NULL)
    {
        for (int i = 0; i < buscas; i++)
        {
            for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
            {
                resultados[index] = resultadosEmBuscas[i].registrosBusca[j];
                index++;
            }
            free(resultadosEmBuscas[i].registrosBusca);
        }
        free(resultadosEmBuscas); // Libera o array de ponteiros
    }

    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_offset);



    // Ignorar casos onde a busca não retorna registros

    /*
    Campos a serem atualizados com valores nulos devem ser identificados, na entrada da 
    funcionalidade,  com  NULO.  Os  valores  dos  campos  do  tipo  string  devem  ser 
    especificados entre aspas duplas ("). Para a manipulação de strings com aspas duplas, 
    pode-se  usar  a  função  scan_quote_string  disponibilizada  na  página  do  projeto  da 
    disciplina. Não será solicitada a atualização dos campos idPessoa e nomeUsuario de 
    com  valores  nulos  ou  repetidos.
    */
    // Mesma especificação do exercício 6 para leitura dos campos a serem atualizados

    /*
    Quando o tamanho do registro atualizado for maior do que o tamanho do
    registro atual, o registro atual deve ser logicamente removido e o registro atualizado
    deve ser inserido como um novo registro

    Caso contrário, o  lixo  que  porventura  permanecer  no  registro  atualizado  deve  ser 
    identificado pelo caractere ‘$’
    */
    // Fazer remoção/inserção, ou só inserção conforme o caso

    return 0;
}