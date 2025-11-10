# Trabalho Prático - Organização de Arquivos e Estruturas de Dados

Este projeto consiste na implementação de um sistema gerenciador de banco de dados simplificado em linguagem C, desenvolvido para as disciplinas de Organização de Arquivos (SCC0215) e Estrutura de Dados III (SCC0607) do ICMC-USP. O sistema manipula arquivos binários para simular operações de um banco de dados relacional, como criação de tabelas, inserção, busca, atualização, remoção e junção de dados.

##  autores

*   **Alexandre Brenner Weber** 
*   **André Luis Santos Messias** 

---

## Funcionalidades Implementadas

O programa implementa 10 funcionalidades que simulam comandos SQL, operando sobre dois tipos de "tabelas": `pessoa` e `segue`.

| Funcionalidade | Comando SQL Equivalente | Descrição |
| :--- | :--- | :--- |
| **1** | `CREATE INDEX` | Cria um arquivo de índice primário (`indexaPessoa.bin`) vazio e consistente. |
| **2** | `CREATE TABLE` | Lê dados de um arquivo `.csv` e popula os arquivos de dados (`pessoa.bin`) e de índice (`indexaPessoa.bin`). |
| **3** | `SELECT FROM` | Exibe na saída padrão todos os registros válidos do arquivo de dados `pessoa.bin`. |
| **4** | `SELECT ... WHERE` | Realiza buscas no arquivo `pessoa.bin` com base em um critério de campo/valor. Utiliza o índice para buscas por `idPessoa`. |
| **5** | `DELETE FROM` | Realiza a remoção lógica de registros no arquivo `pessoa.bin` que correspondem a um critério de busca. |
| **6** | `INSERT INTO` | Insere novos registros no arquivo `pessoa.bin`, com reaproveitamento de espaço de registros removidos. |
| **7** | `UPDATE` | Atualiza um ou mais campos de registros que correspondem a um critério de busca. |
| **8** | `CREATE TABLE` | Lê dados de um `.csv` e cria o arquivo de relacionamentos `segue.bin`. |
| **9** | `ORDER BY` | Ordena o arquivo `segue.bin` com base em múltiplos critérios e salva o resultado em um novo arquivo (`segueOrdenado.bin`). |
| **10**| `JOIN ON` | Realiza a operação de junção entre os arquivos `pessoa.bin` e `segueOrdenado.bin`, exibindo os resultados combinados. |

---

## Estrutura do Projeto

O código-fonte está organizado de forma modular para facilitar a manutenção e o entendimento.

```
/
├── src/
│   ├── data_manip/   # Módulos para ler, escrever e manipular as structs (pessoa, segue, indice).
│   ├── f1/ a f10/    # Implementação de cada uma das 10 funcionalidades.
│   ├── utils/        # Funções utilitárias (ex: binarioNaTela, scan_quote_string).
│   └── main.c        # Ponto de entrada, com o switch-case principal.
├── data/             # Arquivos de entrada .csv.
├── bin/              # Destino para os arquivos binários gerados.
└── Makefile          # Arquivo para compilação e gerenciamento do projeto.
```

---

## Como Compilar e Executar

O projeto utiliza um `Makefile` para simplificar a compilação e execução.

### Comandos Principais

*   **Compilar o programa:**
    ```bash
    make all
    ```
    Este comando compila todos os arquivos-fonte e gera o executável `programaTrab` na raiz do projeto.

*   **Executar o programa:**
    O programa lê a funcionalidade e seus parâmetros da entrada padrão.
    ```bash
    ./programaTrab
    ```

*   **Limpar arquivos gerados:**
    ```bash
    make clean
    ```
    Este comando remove o executável e todos os arquivos de objeto (`.o`).

### Exemplos de Uso

Para executar uma funcionalidade, forneça o número da funcionalidade seguido dos parâmetros necessários, separados por espaço.

*   **Funcionalidade 2 (Criar `pessoa.bin` a partir de um CSV):**
    ```bash
    ./programaTrab
    2 data/arquivoEntrada.csv bin/pessoa.bin bin/indice.bin
    ```

*   **Funcionalidade 4 (Buscar pessoa com `idPessoa` = 25):**
    ```bash
    ./programaTrab
    4 bin/pessoa.bin bin/indice.bin 1 "idPessoa=25"
    ```

*   **Funcionalidade 9 (Ordenar o arquivo `segue.bin`):**
    ```bash
    ./programaTrab
    9 bin/segue.bin bin/segueOrdenado.bin
    ```