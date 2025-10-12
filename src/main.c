// Alexandre Brenner Weber - 15436911
// André Luis Santos Messias - 

// Bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes locais
#include "arquivos.h"
#include "utils/utils.h"
#include "f1/f1.h"
#include "f2/f2.h"

void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

int main() {
    int funcionalidade;
    scanf("%d", &funcionalidade);
    
    switch (funcionalidade)
    {
        case 1:
        {
            char nomeArquivoIndice[100];
            scanf("%s", nomeArquivoIndice);
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            break;
        }
        case 4:
        {
            break;
        }
    
        default:
            break;
    }

    return 0;
}
