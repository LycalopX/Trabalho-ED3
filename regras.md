## Funções
1. FOpen/FClose: Custosos, usar de forma condizente (senso comum kkkk)
2. FSeek: ler e gravar cabeçalho (ignorar cabeçalho antes de realizar leitura)
3. FTell: Custoso, serve para identificar posição no arquivo
4. FWrite: Gravar Struct direto no FWrite dá problemas (realizar campo a campo essa instrução)

## Óbvio
- Código precisa estar modularizado (obviamente)
- Documentar o código (NÃOOOOOO)
- Não usar bibliotecas com as funcionalidades (biblioteca AVL kkkkkkk)

## Não tão óbvio
- Hash é interessante
- int - atoi
- not atof, use strtof
- criar índice - gera com 1 e um monte de lixo
- Organização é nossa, (usar pastas por favor kkkkk)
- wb+ - não usar quando só escrevendo o arquivo

- entupir de alerta de erro para coisas que não existem (lidar com exceções)
- fechar arquivo e finalizar programa com erro
- se não existir o índice, fechar o arquivo original também
- mapa AVL talvez possa ser o melhor
- o disco **tem** que ser ordenado na memória primária
- funcionalidade 3: criar função diferente para printar registro (serve para lidar com exceções)

## Necessidades
- Modularidade
- MakeFile

## Detalhes
- erros no meio do loop de análise dos índices podem ser ignorados
- criar função para salvar char por char no registro

## Vídeos
- 5 min no mxm
- Explicar como foi feito, e não o que faz
- plmns 2m30s

### **1. Manipulação de Arquivos**

 - **Abertura e Fechamento (`FOpen`/`FClose`):** Evite abrir e fechar arquivos repetidamente. A abordagem correta é abrir o arquivo uma vez e passar o ponteiro `FILE*` como parâmetro para as funções que precisam acessá-lo.

 - **Leitura para Processamento:** Para analisar ou filtrar dados, eles devem ser lidos do disco para a memória RAM (usando `fread`). No entanto, **não carregue o arquivo inteiro de uma vez**. A prática recomendada é ler registro por registro, processá-lo e então ler o próximo.

 - **Condição de Parada da Leitura:** A única regra estrita é **não usar `eof()`**. Utilizar o valor de retorno de funções de leitura (como `fgets != NULL`) é uma abordagem válida e aceita.

### **2. Implementação das Funcionalidades**

 - **Busca (Funcionalidades 3 e 4):** A busca deve ser feita percorrendo o arquivo de dados. É permitido ler o registro inteiro para a memória e, então, comparar os campos necessários para verificar se ele corresponde ao critério de busca (filtro).

 - **Saída do `binarioNaTela`:** Conforme a correção nos casos de teste, a saída desta função deve ser exibida **tanto para o arquivo de dados quanto para o arquivo de índice**, quando aplicável.    

### **3. Conversão de Tipos de Dados**

 - Para converter uma string para um **inteiro (`int`)**, use `atoi()`.

 - Para converter uma string para um **inteiro longo (`long`)**, use `strtol()`. Esta é a função recomendada para ler o `byteoffset` do arquivo de índice.

### **4. Organização e Entrega do Projeto**

 - **Comentários de Autoria:** É suficiente colocar as informações dos desenvolvedores (nome, NUSP) no início do arquivo principal (`main`). Colocar em todos os arquivos é uma precaução, mas não é obrigatório.

 - **Envio do Vídeo:**
	- O vídeo **NÃO** deve ser enviado pelo run.codes.
	
	- O envio deve ser feito através da atividade criada no Moodle (e-disciplinas).
	
	- Você deve postar apenas o **link** para o vídeo (hospedado no YouTube, Google Drive, etc.) no campo de texto disponível na tarefa.