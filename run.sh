#!/bin/bash

# Detecta o sistema operacional
OS=$(uname -s)

# Define o comando de execução com ou sem Valgrind
if [ "$OS" = "Linux" ]; then
    echo "Detectado Linux. Executando com Valgrind."
    RUN_CMD="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./programaTrab"
else
    echo "Detectado $OS. Executando sem Valgrind."
    RUN_CMD="./programaTrab"
fi

# Limpa os arquivos objeto e o executável antes de compilar
make clean

# Compila o projeto
make

echo "--- Executando Novos Testes ---"

# Teste 1: Funcionalidade 11, caso 1
echo ">> Teste 1: Funcionalidade 11 (p.bin, i.bin, so.bin)"
cp testes/p.bin .
cp testes/i.bin .
cp testes/so.bin .
echo -e "11 p.bin i.bin so.bin" | $RUN_CMD


echo "--- Fim dos Novos Testes ---"

# silenciosamente
make clean &> /dev/null
make zip &> /dev/null