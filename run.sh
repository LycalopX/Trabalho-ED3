#!/bin/bash

# --- Script de Testes Automatizados (Versão Corrigida) ---
# Compila e executa os casos de teste, passando os argumentos
# via entrada padrão (stdin) para o programa.

# Cores para o output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Passo 1: Limpar e compilar o projeto
echo -e "${BLUE}Limpando e compilando o projeto...${NC}"
make clean > /dev/null 2>&1
make
if [ $? -ne 0 ]; then
    echo "Erro na compilação. Abortando script."
    exit 1
fi
echo -e "${GREEN}Projeto compilado com sucesso!${NC}"
echo ""


# Passo 2: Teste da Funcionalidade 1 (usando pipe para enviar input)
echo -e "${BLUE}--- Executando Teste da Funcionalidade 1 ---${NC}"
echo "Input para o programa: 1 arquivoIndicePrimario.bin"
# A string é "empurrada" para a entrada padrão do programa
echo "1 arquivoIndicePrimario.bin" | ./programaTrab
echo -e "${BLUE}--- Fim do Teste da Funcionalidade 1 ---${NC}"
echo ""
echo ""


# Passo 3: Teste da Funcionalidade 2 (usando pipe para enviar input)
echo -e "${BLUE}--- Executando Teste da Funcionalidade 2 ---${NC}"
echo "Input para o programa: 2 arquivoEntrada.csv arquivoSaida.bin arquivoIndicePrimario.bin"
# Novamente, a string de argumentos é enviada via pipe
echo "2 arquivoEntrada.csv arquivoSaida.bin arquivoIndicePrimario.bin" | ./programaTrab
echo -e "${BLUE}--- Fim do Teste da Funcionalidade 2 ---${NC}"
echo ""


echo -e "${GREEN}Script de testes finalizado.${NC}"