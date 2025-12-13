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

# Teste 2: Funcionalidade 11, caso 2
echo ">> Teste 2: Funcionalidade 11 (pessoas1.bin, indice1.bin, segueOrd.bin)"
cp testes/pessoas1.bin .
cp testes/indice1.bin .
cp testes/segueOrd.bin .
echo -e "11 pessoas1.bin indice1.bin segueOrd.bin" | $RUN_CMD

# Teste 3: Funcionalidade 11, caso 3
echo ">> Teste 3: Funcionalidade 11 (pessoas2.bin, indice2.bin, segueOrd.bin)"
cp testes/pessoas2.bin .
cp testes/indice2.bin .
cp testes/segueOrd.bin .
echo -e "11 pessoas2.bin indice2.bin segueOrd.bin" | $RUN_CMD

# Teste 4: Funcionalidade 12, caso 1
echo ">> Teste 4: Funcionalidade 12 (p.bin, i.bin, so.bin)"
cp testes/p.bin .
cp testes/i.bin .
cp testes/so.bin .
echo -e "12 p.bin i.bin so.bin" | $RUN_CMD

# Teste 5: Funcionalidade 12, caso 2
echo ">> Teste 5: Funcionalidade 12 (pessoas1.bin, indice1.bin, segueOrd.bin)"
cp testes/pessoas1.bin .
cp testes/indice1.bin .
cp testes/segueOrd.bin .
echo -e "12 pessoas1.bin indice1.bin segueOrd.bin" | $RUN_CMD

# Teste 6: Funcionalidade 13, caso 1
echo ">> Teste 6: Funcionalidade 13 (pessoas1.bin, ... LUIZALVES1)"
cp testes/pessoas1.bin .
cp testes/indice1.bin .
cp testes/segueOrd.bin .
echo -e "13 pessoas1.bin indice1.bin segueOrd.bin LUIZALVES1" | $RUN_CMD

# Teste 7: Funcionalidade 13, caso 2
echo ">> Teste 7: Funcionalidade 13 (pessoas2.bin, ... JUANCUNHA1)"
cp testes/pessoas2.bin .
cp testes/indice2.bin .
cp testes/segueOrd.bin .
echo -e "13 pessoas2.bin indice2.bin segueOrd.bin JUANCUNHA1" | $RUN_CMD

# Teste 8: Funcionalidade 13, caso 3 (arquivo inexistente)
echo ">> Teste 8: Funcionalidade 13 (pessoas1.bin, ... nao_existe)"
cp testes/pessoas1.bin .
cp testes/indice1.bin .
echo -e "13 pessoas1.bin indice1.bin nao_existe. GNIPNIJIX" | $RUN_CMD

# Teste 9: Funcionalidade 14, caso 1
echo ">> Teste 9: Funcionalidade 14 (pessoas1.bin, ... MARIAARAGAO1)"
cp testes/pessoas1.bin .
cp testes/indice1.bin .
cp testes/segueOrd.bin .
echo -e "14 pessoas1.bin indice1.bin segueOrd.bin MARIAARAGAO1" | $RUN_CMD

# Teste 10: Funcionalidade 14, caso 2
echo ">> Teste 10: Funcionalidade 14 (pessoas2.bin, ... LIAMARAGAO)"
cp testes/pessoas2.bin .
cp testes/indice2.bin .
cp testes/segueOrd.bin .
echo -e "14 pessoas2.bin indice2.bin segueOrd.bin LIAMARAGAO" | $RUN_CMD


echo "--- Fim dos Novos Testes ---"

# silenciosamente
make clean &> /dev/null
make zip &> /dev/null