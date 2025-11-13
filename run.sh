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

#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose 

# Limpa os arquivos objeto e o executável antes de compilar
make clean

# Compila o projeto
make

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

# Teste 5: Tenta listar registros de um arquivo de dados que não existe.
# echo ">> Teste 5: Funcionalidade 3 (Arquivo pessoas5.bin)"
# echo -e "3 entrada.bin" | $RUN_CMD

# Teste 6: Tenta listar registros de um arquivo de dados que não existe.
# echo ">> Teste 6: Funcionalidade 3 (Arquivo nao_existe.bin)"
# echo -e "3 nao_existe.bin" | $RUN_CMD

# Teste 7: Realiza uma busca simples por 'idPessoa'.
# echo ">> Teste 7: Funcionalidade 4 (Busca por idPessoa=96)"
# echo -e "4 entrada.bin indice.bin 1\n1 idPessoa=96" | $RUN_CMD

# Teste 8: Realiza múltiplas buscas com diferentes campos e valores.
# echo ">> Teste 8: Funcionalidade 4 (Múltiplas buscas)"
# echo -e "4 entrada.bin indice.bin 6\n1 idadePessoa=19\n2 idadePessoa=81\n3 idadePessoa=333\n4 nomePessoa=\"AGATHA ALBUQUERQUE\"\n5 nomePessoa=\"JOHN LENNON\"\n6 nomePessoa=\"MATHEUS PACHECO\"" | $RUN_CMD

# Teste 9: Tenta remover um registro de um arquivo que não existe.
 echo ">> Teste 9: Funcionalidade 5 (Remoção em arquivo inexistente)"
 echo -e "5 nao_existe.bin indice.bin 1\n1 idPessoa=245" | $RUN_CMD

# Teste 10: Realiza múltiplas remoções.
 echo ">> Teste 10: Funcionalidade 5 (Múltiplas remoções)"
 echo -e "5 entrada.bin indice.bin 5\n1 idPessoa=69\n2 idPessoa=520\n3 idPessoa=2000\n4 nomeUsuario=\"MATEUS90\"\n5 idadePessoa=NULO" | $RUN_CMD

# Teste 11: Realiza múltiplas inserções.
 echo ">> Teste 11: Funcionalidade 6 (Múltiplas inserções)"
 echo -e "6 entrada.bin indice.bin 2\n1 625, \"SAMANTHA PEREIRA SANTOS\", 13, \"SAMANTHAPS\"\n2 645, \"VITORIA PRADO CAMPOS\", NULO, \"VIVICA\"" | $RUN_CMD

# Teste 12: Realiza múltiplas atualizações.
 echo ">> Teste 12: Funcionalidade 7 (Múltiplas atualizações)"
 echo -e "7 entrada.bin indice.bin 3 \n1 idPessoa=23 nomePessoa=\"PINTUDO DA SILVA\" \n2 idadePessoa=89 idadePessoa=20 \n3 nomePessoa=\"HEITOR DA MOTA\" nomePessoa=\"HIHI PINTUDO DA SILVA DE NOVO HIHI\"" | $RUN_CMD

# Teste 13: Realiza múltiplas atualizações com casos de borda, usando dados válidos.
 echo ">> Teste 13: Funcionalidade 7 (Atualizações com dados válidos)"
 echo -e "7 entrada.bin indice.bin 5 \n1 idPessoa=268 idPessoa=1234 \n2 idadePessoa=NULO idadePessoa=18 \n3 nomePessoa=NULO nomePessoa=\"FANTASMA\" \n4 nomePessoa=\"NAOEXISTE\" idPessoa=1 \n5 nomeUsuario=\"HMACHADO\" nomeUsuario=\"ANAFER\"" | $RUN_CMD

# Teste 14: Tenta criar um arquivo de seguidores a partir de um CSV que não existe.
# echo ">> Teste 14: Funcionalidade 8 (Arquivo CSV de seguidores inexistente)"
# echo -e "8 nao_existe.csv segue.bin" | $RUN_CMD

# Teste 15: Cria um arquivo de seguidores a partir de um CSV.
# echo ">> Teste 15: Funcionalidade 8 (Criação de arquivo de seguidores)"
# echo -e "8 segue.csv segue.bin" | $RUN_CMD

# Teste 16: Ordena o arquivo de seguidores.
# echo ">> Teste 16: Funcionalidade 9 (Ordenação do arquivo de seguidores)"
# echo -e "9 segue.bin segueOrdenado.bin" | $RUN_CMD

# Teste 17: Realiza uma junção simples.
# echo ">> Teste 17: Funcionalidade 10 (Junção simples)"
# echo -e "10 entrada.bin indice.bin segueOrdenado.bin 1\n1 idPessoa=19" | $RUN_CMD

# Teste 18: Realiza múltiplas junções.
# echo ">> Teste 18: Funcionalidade 10 (Múltiplas junções)"
# echo -e "10 entrada.bin indice.bin segueOrdenado.bin 2\n1 idPessoa=333\n2 idadePessoa=21" | $RUN_CMD

echo "--- Fim dos Testes ---"

# silenciosamente
make clean &> /dev/null
make zip &> /dev/null