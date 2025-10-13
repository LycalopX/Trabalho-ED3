#!/bin/bash

# Limpa os arquivos objeto e o executável antes de compilar
make clean

# Compila o projeto
make

echo "--- Executando Testes Runcodes ---"

# Teste 1: Cria um arquivo de índice vazio.
echo ">> Teste 1: Funcionalidade 1 (indice.bin)"
echo "1 indice.bin" | ./programaTrab

# Teste 2: Popula os arquivos a partir de um arquivo CSV (dados2.csv).
# Nota: Este teste sobrescreverá os arquivos criados no teste anterior.
echo ">> Teste 2: Funcionalidade 2 (dados2.csv -> entrada.bin, indice.bin)"
echo "2 arquivoEntrada.csv entrada.bin indice.bin" | ./programaTrab

# Teste 3: Tenta popular a partir de um arquivo CSV que não existe.
echo ">> Teste 3: Funcionalidade 2 (Arquivo nao_existe.csv)"
echo "2 nao_existe.csv entrada.bin indice.bin" | ./programaTrab

# Teste 4: Popula os arquivos com o CSV principal (dados.csv).
# Nota: Este passo é fundamental, pois os testes de busca seguintes usarão estes dados.
echo ">> Teste 4: Funcionalidade 2 (dados.csv -> entrada.bin, indice.bin)"
echo "2 arquivoEntrada.csv entrada.bin indice.bin" | ./programaTrab

# Teste 5: Tenta listar registros de um arquivo de dados que não existe.
echo ">> Teste 5: Funcionalidade 3 (Arquivo pessoas5.bin)"
echo "3 entrada.bin" | ./programaTrab

# Teste 6: Tenta listar registros de um arquivo de dados que não existe.
echo ">> Teste 6: Funcionalidade 3 (Arquivo nao_existe.bin)"
echo "3 nao_existe.bin" | ./programaTrab

# Teste 7: Realiza uma busca simples por 'idPessoa'.
echo ">> Teste 7: Funcionalidade 4 (Busca por idPessoa=96)"
echo "4 entrada.bin indice.bin 1 1 idPessoa=96" | ./programaTrab

# Teste 8: Realiza múltiplas buscas com diferentes campos e valores.
echo ">> Teste 8: Funcionalidade 4 (Múltiplas buscas)"
echo "4 entrada.bin indice.bin 6 1 idadePessoa=19 2 idadePessoa=81 3 idadePessoa=333 4 nomePessoa=\"AGATHA ALBUQUERQUE\" 5 nomePessoa=\"JOHN LENNON\" 6 nomePessoa=\"MATHEUS PACHECO\"" | ./programaTrab

echo "--- Fim dos Testes ---"