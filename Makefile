# Makefile para compilação e execução do trabalho
# Esta versão encontra automaticamente todos os arquivos .c necessários.

# Encontra todos os arquivos .c dentro da pasta src e de suas subpastas
SRCS = $(shell find src -name "*.c")

# A regra 'all' compila todos os arquivos .c encontrados.
# -lmd é a flag necessária para a função binarioNaTela.
all:
	gcc -o programaTrab $(SRCS) -lmd

# A regra 'run' executa o programa.
run:
	./programaTrab

# A regra 'clean' remove o programa executável.
clean:
	rm -f programaTrab

