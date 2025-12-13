/*

   1. `criar_grafo()`
       * O que faz: Seria a função mais importante. Ela leria os dados dos arquivos (pessoa.bin e segueOrdenado.bin), usando as funções que já existem, e montaria a estrutura de dados do grafo (a lista de adjacências) em memória, já tratando a ordenação.
       * Usada por: Todas as funcionalidades (11, 12, 13 e 14), pois o grafo é o ponto de partida para todas elas.

   2. `imprimir_grafo()`
       * O que faz: Recebe um grafo e o imprime na saída padrão no formato exato exigido pelo trabalho (vértices e arestas em ordem alfabética).
       * Usada por: Funcionalidade 11 (para imprimir o grafo original) e Funcionalidade 12 (para imprimir o grafo transposto).

   3. `transpor_grafo()`
       * O que faz: Recebe um grafo G como entrada e retorna um novo grafo G_t que é o seu transposto (com todas as arestas invertidas).
       * Usada por: Funcionalidade 12.

   4. `encontrar_caminho_mais_curto_bfs()`
       * O que faz: Implementa o algoritmo de Busca em Largura (BFS) para encontrar o caminho mais curto entre dois nós em um grafo. Poderia retornar o caminho ou apenas seu comprimento.
       * Usada por: Funcionalidade 13 (para achar o caminho de um seguidor até a celebridade) e Funcionalidade 14 (para detectar o ciclo da fofoca).

   5. `destruir_grafo()`
       * O que faz: Libera toda a memória alocada para a estrutura do grafo (vértices, listas, strings, etc.) para evitar vazamentos de memória (memory leaks).
       * Usada por: Todas as funcionalidades (11, 12, 13 e 14) ao final de sua execução.

*/