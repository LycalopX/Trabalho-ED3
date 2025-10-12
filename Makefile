# Makefile para compilação e execução do trabalho

all:
    gcc -Wall -Wextra -std=c11 -o programaTrab *.c -I. -Ibin -lm

    # A flag -lm é para a biblioteca de matemática, pode ser útil. 
    # O PDF indica -Imd, verifique qual é a correta para a função binarioNaTela.

run:
    ./programaTrab

clean:
    rm -f programaTrab bin/*