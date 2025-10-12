// Alexandre Brenner Weber - 15436911
// André Luis Santos Messias - 

// Bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes locais
#include "arquivos.h"
#include "utils/utils.h"


int main() {
    printf("Hello, World!\n");

    return 0;
}




/*

## Descrição do Arquivo de Pessoa

Este trabalho tem como objetivo obter dados de um arquivo de entrada e gerar um arquivo binário com eles, 
bem como realizar operações de busca com e sem um índice primário. Ele é um trabalho introdutório, de forma 
que será usado como base para o desenvolvimento dos demais trabalhos da disciplina.

Os trabalhos práticos têm como objetivo armazenar e recuperar dados relacionados às pessoas que seguem pessoas em uma rede social.
São definidos dois arquivos de dados a serem utilizados nos trabalhos práticos: arquivo de dados pessoa e arquivo de dados segue.

Registro de Cabeçalho pessoa: 

status: 0- inconsistente, 1- consistente (1 byte),
quantidadePessoas: inicializado com 0, incrementado quando necessário (4 bytes), 
quantidadeRemovidos: inicializado com 0, incrementado quando necessário (4 bytes), 
proxByteOffset: iniciado com valor 17, incrementado quando necessário (17 bytes)

- Tamanho fixo (17 bytes);

Os campos de tamanho fixo são definidos da seguinte forma: 
• idPessoa: código sequencial que identifica univocamente cada registro do arquivo de dados – inteiro – tamanho: 4 bytes. 
• idadePessoa: idade da pessoa – inteiro – tamanho: 4 bytes. 

Os campos de tamanho variável são definidos da seguinte forma (usar método indicador de tamanho):
• nomePessoa: nome completo da pessoa – string 
• nomeUsuario: nome da pessoa na rede social – string

Adicionalmente, os seguintes campos de tamanho fixo também compõem cada registro:
- removido: indica se o registro está logicamente removido. – tamanho: string de 1 byte. 
- tamanhoRegistro: número de bytes do registro – inteiro – tamanho: 4 bytes. 
- tamanhoNomePessoa: número de bytes do campo nomePessoa – inteiro – tamanho: 4 bytes. 
- tamanhoNomeUsuario: número de bytes do campo nomeUsuario – inteiro – tamanho: 4 bytes

OBS:
- O campo nomeUsuario não pode assumir valores nulos ou valores repetidos. 
- O campo idPessoa não pode assumir valores nulos ou valores repetidos. 
- Os valores alfanuméricos dos campos de tamanhão variável não devem ser finalizados por '\0',
- No tamanho fixo, os valores nulos devem ser representados pelo valor -1 quando forem inteiros 
ou devem ser totalmente preenchidos pelo lixo ‘$’ quando forem do tipo string.
- Os valores nulos nos campos de tamanho variável devem ser manipulados da seguinte forma. 
Apenas o indicador de tamanho deve ser armazenado, com o valor zero.
- Nenhum byte do registro deve permanecer vazio, ou seja, cada byte deve armazenar um valor válido ou ‘$’.
- O conceito de página de disco não está sendo considerado.


## Descrição do Arquivo de Índice Primário

- O arquivo de dados pessoa é indexado por um índice primário, que é definido sobre o campo idPessoa. 
O índice primário também é um arquivo, sendo chamado de arquivo de índice indexaPessoa. A especificação 
de indexaPessoa é feita a seguir.

Registro de Cabeçalho pessoa: 

status: 0- inconsistente, 1- consistente (1 byte),

O tamanho do registro de cabeçalho deve ser de 12 bytes (1 byte de status, 11 de lixo).

- Para caber em páginas de disco (que são definidas em potência de 2), o registro de cabeçalho foi definido 
como uma potência de 2, no mesmo tamanho dos registros de dados). Portanto, o registro de cabeçalho tem o 
tamanho de 12 bytes, sendo 1 byte preenchido com dados necessários para o desenvolvimento do trabalho, e os 
bytes restantes preenchidos com lixo. O lixo é representado pelo caractere ‘$’.



Descrição Geral. 
- Implemente um programa em C por meio do qual o usuário possa obter dados de um arquivo de entrada e gerar 
arquivos binários com esses dados, bem como realizar operações de busca nesses arquivos binários com e sem 
o uso de um índice primário.

Importante. 
A definição da sintaxe de cada comando bem como sua saída devem seguir estritamente as especificações definidas 
em cada funcionalidade. Para especificar a sintaxe de execução, considere que o programa seja chamado de “programaTrab”. 
Essas orientações devem ser seguidas uma vez que a correção do funcionamento do programa se dará de forma automática. 
De forma geral, a primeira entrada da entrada padrão é sempre o identificador de suas funcionalidades, conforme 
especificado a seguir.

*/
