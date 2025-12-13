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


echo "--- Executando Testes Runcodes ---"

# Teste 1: Cria um arquivo de índice vazio.
 echo ">> Teste 1: Funcionalidade 1 (indice.bin)"
 echo -e "1 indice.bin" | $RUN_CMD

# Teste 2: Popula os arquivos a partir de um arquivo CSV (dados2.csv).
# Nota: Este teste sobrescreverá os arquivos criados no teste anterior.
 echo ">> Teste 2: Funcionalidade 2 (dados.csv -> entrada.bin, indice.bin)"
 echo -e "2 arquivoEntrada.csv entrada.bin indice.bin" | $RUN_CMD

# Teste 4: Popula os arquivos com o CSV principal (dados.csv).
# Nota: Este passo é fundamental, pois os testes de busca seguintes usarão estes dados.
 echo ">> Teste 4: Funcionalidade 2 (dados.csv -> entrada.bin, indice.bin)"
 echo -e "2 arquivoEntrada.csv entrada.bin indice.bin" | $RUN_CMD

# Teste 9: Tenta remover um registro de um arquivo que não existe.
 echo ">> Teste 9: Funcionalidade 5 (Remoção em arquivo inexistente)"
 echo -e "5 nao_existe.bin indice.bin 1\n1 idPessoa=245" | $RUN_CMD

# Teste 10: Realiza múltiplas remoções.
 echo ">> Teste 10: Funcionalidade 5 (Múltiplas remoções)"
 echo -e "5 entrada.bin indice.bin 5\n1 idPessoa=69\n2 idPessoa=520\n3 idPessoa=2000\n4 nomeUsuario=\"MATEUS90\"\n5 idadePessoa=NULO" | $RUN_CMD

echo "--- Fim dos Novos Testes ---"

# silenciosamente
make clean &> /dev/null
make zip &> /dev/null