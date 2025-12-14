
/**
 * Entrada do programa para a funcionalidade [14]: 
14 arquivoPessoa.bin arquivoIndexaPessoa.bin arquivoSegueOrdenado.bin 
"nomeUsuarioQueGerouFofoca"  
onde: 
-  arquivoPessoa.bin  é  o  arquivo  de  dados  pessoa  no  formato  binário,  o  qual  é 
gerado de acordo com as especificações da Descrição do Arquivo Pessoa definidas 
no trabalho prático introdutório. 
- arquivoIndexaPessoa.bin é o arquivo de índice primário que indexa o arquivo de 
dados  pessoa,  o  qual  é  gerado  de  acordo  com  as  especificações  da  Descrição  do 
Arquivo de Índice Primário definidas no trabalho prático introdutório. 
- arquivoSegueOrdenado.bin é o arquivo de dados segue no formato binário gerado 
pela  funcionalidade  [9]  conforme  as  especificações  descritas  no  primeiro  trabalho 
prático, o qual encontra-se ordenado de acordo com o campo idPessoaQueSegue. 
- nomeUsuarioQueGerouFofoca é o nome de usuário da pessoa que gerou a fofoca e 
que é passado como parâmetro. Ele é representado entre aspas duplas (") por ser do 
tipo string. 
 
Saída caso o programa seja executado com sucesso: 
A saída deve ser exibida na saída padrão da seguinte forma. Deve ser 
exibido o comprimento do caminho caso a fofoca retorne para a pessoa 
ou "A FOFOCA NAO RETORNOU".   
 
Mensagem de saída caso algum erro seja encontrado: 
Falha na execução da funcionalidade. 
 
Exemplo de execução (são mostrados apenas alguns elementos): 
./programaTrab  
14 arquivoPessoa.bin arquivoIndexaPessoa.bin arquivoSegueOrdenado.bin 
"ARTHUROLIVEIRA" 
5 
 
./programaTrab  
14 arquivoPessoa.bin arquivoIndexaPessoa.bin arquivoSegueOrdenado.bin 
"STEPHANY6" 
A FOFOCA NAO RETORNOU 

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f14.h"
#include "../grafo/grafo.h"

int funcionalidade14(FILE *fpPessoa, FILE *fpIndice, FILE *fpSegue, char *nomeUsuarioQueGerouFofoca)
{
    Grafo *grafo = criar_grafo(fpPessoa, fpIndice, fpSegue);
    if (!grafo)
    {
        printf(FALHA_AO_EXECUTAR_FUNCIONALIDADE);
        return 1; // Indica falha na criação do grafo
    }

    Grafo *grafoTransposto = transpor_grafo(grafo);
    if (!grafoTransposto)
    {
        destruir_grafo(grafo);
        printf(FALHA_AO_EXECUTAR_FUNCIONALIDADE);
        return 1; // Indica falha na criação do grafo transposto
    }

    encontrar_caminhos_curtos_bfs(grafoTransposto, nomeUsuarioQueGerouFofoca, grafo);

    destruir_grafo(grafo);
    destruir_grafo(grafoTransposto);
    return 0; // Sucesso
}