# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -g -I. -IAssets

# Busca recursiva por arquivos .c
SRCS = $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard *.c) 

# Arquivos objeto gerados a partir dos arquivos fonte
OBJS = $(SRCS:.c=.o)

# Executável final
TARGET = programaTrab

# Arquivo de teste
TEST = test.txt

# Nome do arquivo zip
ZIPFILE = projeto.zip

all: $(TARGET)
	
run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@if [ -f $(TEST) ]; then \
		./$(TARGET) < $(TEST); \
	else \
		echo "Arquivo de teste $(TEST) não encontrado!"; \
	fi

# Regra para criar o executável
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
	@if [ "$(MAKECMDGOALS)" != "buildO" ]; then \
		find . -name "*.o" -type f -delete; \
	fi

# buildO: compila e mantém os .o
buildO: $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Regra para compilar os arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos objeto e o executável
clean:
	find . -name "*.o" -type f -delete
	rm -f $(TARGET)

# Limpar apenas os arquivos .o
clean-obj:
	find . -name "*.o" -type f -delete

# Regra para criar um arquivo zip com todo o projeto, ignorando .o e o próprio .zip
zip:
	rm -f $(ZIPFILE)
	find . -name "*.c" -o -name "*.h" -o -name "Makefile" -o -name "run.sh" | zip $(ZIPFILE) -@

# Compilar com flag de debug
debug: CFLAGS += -g
debug: $(TARGET)

# Regra para executar o Valgrind no programa
val: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

# Regra para executar o Valgrind com o arquivo de teste
valtest: $(TARGET)
	@if [ -f $(TEST) ]; then \
		valgrind --leak-check=full ./$(TARGET) < $(TEST); \
	else \
		echo "Arquivo de teste $(TEST) não encontrado!"; \
	fi

.PHONY: all clean clean-obj debug zip val valtest buildO