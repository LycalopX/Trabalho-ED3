#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para ftruncate

#include "../arquivos.h"
#include "../utils/utils.h"
#include "../data_manip/indice.h"
#include "../data_manip/pessoa.h"

#include "../f4/f4.h"
#include "tarefas.h"

// Atualizar campos como ID, idade
static int atualizar_campo_inteiro(RegistroBuscaPessoa *registroBusca, TarefaDeAtualizacao *tarefa, Parametro *u, int *campo_registro)
{
    printf("  [DEBUG] atualizar_campo_inteiro para ID %d. Novo valor: %s\n", registroBusca->registro->idPessoa, u->valor);
    if (atoi(u->valor) != *campo_registro)
    {
        tarefa->idPessoaAntigo = registroBusca->registro->idPessoa;
        *campo_registro = atoi(u->valor);
        printf("    Campo atualizado.\n");
    }
    else
    {
        registroBusca->ByteOffset = -1;
        printf("    Campo não precisou de atualização.\n");
    }
    return 0;
}

// Atualizar campos como nome, usuario
static int atualizar_campo_string(RegistroBuscaPessoa *registroBusca, TarefaDeAtualizacao *tarefa, Parametro *u, char **campo_registro, int *tamanho_campo, long long *proxByteOffset)
{
    printf("  [DEBUG] atualizar_campo_string para ID %d. Novo valor: %s\n", registroBusca->registro->idPessoa, u->valor);
    char *current_string = *campo_registro;
    char *new_string = u->valor;

    int update_needed = 0;
    if (current_string == NULL)
    {
        if (strlen(new_string) > 0)
        {
            update_needed = 1;
        }
    }
    else
    {
        if (strcmp(new_string, current_string) != 0)
        {
            update_needed = 1;
        }
    }

    if (update_needed)
    {
        int new_size = strlen(new_string);
        int old_size = *tamanho_campo;
        printf("    Campo atualizado. Tamanho antigo: %d, Tamanho novo: %d\n", old_size, new_size);

        if (current_string != NULL)
        {
            free(*campo_registro);
        }
        *campo_registro = strdup(new_string);
        *tamanho_campo = new_size;
    }
    else
    {
        registroBusca->ByteOffset = -1;
        printf("    Campo não precisou de atualização.\n");
    }

    return 0;
}

// Processa as atualizações para cada busca, preenchendo o array de tarefas
static void processar_atualizacoes_de_busca(int buscas, ResultadoBuscaPessoa *resultadosEmBuscas, TarefaDeAtualizacao *tarefas, long long *proxByteOffset)
{
    printf("[DEBUG] Entrando em processar_atualizacoes_de_busca para %d buscas.\n", buscas);
    int nTarefas = 0;

    for (int i = 0; i < buscas; i++)
    {
        printf("[DEBUG] Processando busca %d...\n", i);
        if (resultadosEmBuscas[i].nRegistros == 0 || resultadosEmBuscas[i].update.valor == NULL)
        {
            printf("  Busca %d não retornou registros ou não tem update. Pulando.\n", i);
            continue;
        }

        RegistroBuscaPessoa **registrosBusca = resultadosEmBuscas[i].registrosBusca;
        Parametro *u = &resultadosEmBuscas[i].update;
        printf("  Busca %d: %d registros encontrados. Atualizar campo '%s' para '%s'.\n", i, resultadosEmBuscas[i].nRegistros, u->campo, u->valor);

        int j = 0;
        int nRegistros = resultadosEmBuscas[i].nRegistros;

        if (strcmp(u->campo, CAMPO_ID) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, registrosBusca[j]->ByteOffset, &indexTarefa);
                atualizar_campo_inteiro(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->idPessoa);
            }
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_IDADE) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, registrosBusca[j]->ByteOffset, &indexTarefa);
                atualizar_campo_inteiro(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->idadePessoa);
            }
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_NOME) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, registrosBusca[j]->ByteOffset, &indexTarefa);
                atualizar_campo_string(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->nomePessoa, &registrosBusca[j]->registro->tamanhoNomePessoa, proxByteOffset);
            }
            nTarefas += j;
        }
        else if (strcmp(u->campo, CAMPO_USUARIO) == 0)
        {
            for (j = 0; j < nRegistros; j++)
            {
                int indexTarefa = j + nTarefas;
                inicializa_tarefa(&tarefas[indexTarefa], registrosBusca[j]->registro->idPessoa, registrosBusca[j]->ByteOffset, &indexTarefa);
                atualizar_campo_string(registrosBusca[j], &tarefas[indexTarefa], u, &registrosBusca[j]->registro->nomeUsuario, &registrosBusca[j]->registro->tamanhoNomeUsuario, proxByteOffset);
            }
            nTarefas += j;
        }
    }
    printf("[DEBUG] Saindo de processar_atualizacoes_de_busca.\n");
}

static void unificarResultados(RegistroBuscaPessoa **resultados, TarefaDeAtualizacao *tarefas, int *nRegsEncontrados)
{
    printf("[DEBUG] Entrando em unificarResultados com %d registros.\n", *nRegsEncontrados);
    if (*nRegsEncontrados <= 1)
    {
        printf("[DEBUG] Menos de 2 registros, não há nada para unificar.\n");
        return;
    }

    int write_idx = 0;
    for (int read_idx = 1; read_idx < (*nRegsEncontrados); read_idx++)
    {
        printf("  [DEBUG] Unificando: read_idx=%d (ID %d), write_idx=%d (ID %d)\n", read_idx, resultados[read_idx]->registro->idPessoa, write_idx, resultados[write_idx]->registro->idPessoa);
        if (resultados[read_idx]->ByteOffset == -1)
        {
            printf("    Registro %d (ID %d) marcado como não-atualizável. Ignorando.\n", read_idx, resultados[read_idx]->registro->idPessoa);
            destroi_registro_pessoa(resultados[read_idx]->registro);
            free(resultados[read_idx]);
            continue;
        }

        if (resultados[read_idx]->registro->idPessoa != resultados[write_idx]->registro->idPessoa)
        {
            printf("    Registro %d (ID %d) é único. Movendo para write_idx %d.\n", read_idx, resultados[read_idx]->registro->idPessoa, write_idx + 1);
            write_idx++;
            resultados[write_idx] = resultados[read_idx];
            tarefas[write_idx] = tarefas[read_idx];
        }
        else
        {
            printf("    Registro %d (ID %d) é duplicata de %d. Fundindo.\n", read_idx, resultados[read_idx]->registro->idPessoa, write_idx);
            switch (tarefas[read_idx].indiceDaRegra)
            {
            case 0: // ID
                resultados[write_idx]->registro->idPessoa = resultados[read_idx]->registro->idPessoa;
                break;
            case 1: // Idade
                resultados[write_idx]->registro->idadePessoa = resultados[read_idx]->registro->idadePessoa;
                break;
            case 2: // Nome
                free(resultados[write_idx]->registro->nomePessoa);
                resultados[write_idx]->registro->nomePessoa = strdup(resultados[read_idx]->registro->nomePessoa);
                resultados[write_idx]->registro->tamanhoNomePessoa = resultados[read_idx]->registro->tamanhoNomePessoa;
                break;
            case 3: // Usuario
                free(resultados[write_idx]->registro->nomeUsuario);
                resultados[write_idx]->registro->nomeUsuario = strdup(resultados[read_idx]->registro->nomeUsuario);
                resultados[write_idx]->registro->tamanhoNomeUsuario = resultados[read_idx]->registro->tamanhoNomeUsuario;
                break;
            }

            resultados[write_idx]->ByteOffset = (resultados[write_idx]->ByteOffset > resultados[read_idx]->ByteOffset) ? resultados[write_idx]->ByteOffset : resultados[read_idx]->ByteOffset;
            printf("      ByteOffset final da duplicata: %lld\n", resultados[write_idx]->ByteOffset);

            destroi_registro_pessoa(resultados[read_idx]->registro);
            free(resultados[read_idx]);
        }
    }
    *nRegsEncontrados = write_idx + 1;
    printf("[DEBUG] Saindo de unificarResultados. %d registros únicos.\n", *nRegsEncontrados);
}

static int aplicar_atualizacoes_de_busca(FILE *fp, RegistroIndice **indice_em_memoria, TarefaDeAtualizacao *tarefas,
                                         int nRegsEncontrados, int nRemovidosInseridos, CabecalhoPessoa cabPessoa, RegistroBuscaPessoa **resultados)
{
    printf("[DEBUG] Entrando em aplicar_atualizacoes_de_busca com %d registros para aplicar.\n", nRegsEncontrados);
    fseek(fp, 17, SEEK_SET);
    long long cursor_position = 17;

    RegistroBuscaPessoa **removidos_inseridos = malloc(nRemovidosInseridos * sizeof(RegistroBuscaPessoa *));
    int removidos_inseridos_idx = 0;

    for (int i = 0; i < nRegsEncontrados; i++)
    {
        RegistroPessoa *reg_atualizado = resultados[i]->registro;
        printf("  [DEBUG] Aplicando atualização para ID %d no ByteOffset %lld\n", reg_atualizado->idPessoa, resultados[i]->ByteOffset);

        long long byteOffset = resultados[i]->ByteOffset;

        int tamanho_novo_dados = sizeof(int) * 4 + reg_atualizado->tamanhoNomePessoa + reg_atualizado->tamanhoNomeUsuario;
        int tamanho_antigo_dados = reg_atualizado->tamanhoRegistro;
        
        printf("    Tamanho antigo: %d, Tamanho novo: %d\n", tamanho_antigo_dados, tamanho_novo_dados);

        long long tamanho_real_escrito = sizeof(char) + sizeof(int) + tamanho_novo_dados;
        long long diffByteOffset = byteOffset - cursor_position;

        if (tamanho_novo_dados <= tamanho_antigo_dados)
        {
            printf("    Atualização IN-PLACE.\n");
            fseek(fp, diffByteOffset, SEEK_CUR);
            reg_atualizado->tamanhoRegistro = tamanho_antigo_dados;
            escreve_registro_pessoa(fp, reg_atualizado);

            int diff = tamanho_antigo_dados - tamanho_novo_dados;
            if (diff > 0)
            {
                printf("      Preenchendo %d bytes de lixo com '.\n", diff);
                char *lixo = malloc(diff);
                memset(lixo, '$', diff);
                fwrite(lixo, 1, diff, fp);
                free(lixo);
            }
        }
        else
        {
            printf("    Atualização OUT-OF-PLACE (remover e inserir).\n");
            removidos_inseridos[removidos_inseridos_idx] = malloc(sizeof(RegistroBuscaPessoa));
            removidos_inseridos[removidos_inseridos_idx]->registro = reg_atualizado;
            removidos_inseridos[removidos_inseridos_idx]->ByteOffset = byteOffset;

            cabPessoa.proxByteOffset += tamanho_real_escrito;
            removidos_inseridos_idx++;
        }

        if (tarefas[i].indiceDaRegra == 0)
        {
            printf("    Atualizando índice para ID %d.\n", reg_atualizado->idPessoa);
            int id_busca;

            if (tarefas[i].idPessoaAntigo == 0)
            {
                id_busca = reg_atualizado->idPessoa;
            }
            else
            {
                id_busca = tarefas[i].idPessoaAntigo;
            }

            RegistroIndice **p_encontrado_ptr = bsearch(&id_busca,
                                                        indice_em_memoria,
                                                        cabPessoa.quantidadePessoas,
                                                        sizeof(RegistroIndice *),
                                                        comparar_bsearch_indice);

            if (p_encontrado_ptr == NULL)
            {
                printf("      [ERRO] ID %d não encontrado no índice para atualização!\n", id_busca);
                free(indice_em_memoria);
                return -1;
            }

            if (tarefas[i].indiceDaRegra == 0)
            {
                (*p_encontrado_ptr)->idPessoa = reg_atualizado->idPessoa;
                printf("      ID no índice atualizado para %d.\n", reg_atualizado->idPessoa);
            }
        }

        byteOffset += diffByteOffset + tamanho_real_escrito;
    }

    remover_pessoas_e_indices(removidos_inseridos, indice_em_memoria, cabPessoa, nRemovidosInseridos, fp, 1);
    inserir_pessoas(fp, removidos_inseridos, nRemovidosInseridos);

    return 0;
}

void copiar_resultados_busca_para_array(ResultadoBuscaPessoa *resultadosEmBuscas, int buscas, RegistroBuscaPessoa **resultados, int nRegsEncontrados)
{
    int current_idx = 0;
    for (int i = 0; i < buscas; i++)
    {
        for (int j = 0; j < resultadosEmBuscas[i].nRegistros; j++)
        {
            if (current_idx >= nRegsEncontrados) {
                printf("[DEBUG] ERRO FATAL em copiar_resultados: current_idx (%d) estourou nRegsEncontrados (%d)\n", current_idx, nRegsEncontrados);
                fflush(stdout);
                // Não continue, apenas para depurar
            }

            resultados[current_idx++] = resultadosEmBuscas[i].registrosBusca[j];
        }
        free(resultadosEmBuscas[i].registrosBusca);
    }
    free(resultadosEmBuscas);
}

int funcionalidade7(FILE *fp, FILE *fpIndice, int buscas)
{
    imprimir_registros_raw(fp);

    printf("\n--- [DEBUG] INICIANDO FUNCIONALIDADE 7 ---\n");
    printf("[DEBUG] %d buscas para processar.\n", buscas);

    int nRegsEncontrados = 0;
    ResultadoBuscaPessoa *resultadosEmBuscas = funcionalidade4(fp, fpIndice, buscas, &nRegsEncontrados, 1, 1);
    printf("[DEBUG] funcionalidade4 encontrou um total de %d registros.\n", nRegsEncontrados);

    if (nRegsEncontrados == 0)
    {
        printf("[DEBUG] Nenhum registro encontrado, encerrando.\n");
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        return 0;
    }

    TarefaDeAtualizacao *tarefas = malloc(sizeof(TarefaDeAtualizacao) * nRegsEncontrados);
    if (tarefas == NULL)
    {
        printf(FALHA_AO_ALOCAR);
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);

    CabecalhoPessoa cabPessoa;
    le_cabecalho_pessoa(fp, &cabPessoa);
    toggle_cabecalho_pessoa(fp, &cabPessoa);
    printf("[DEBUG] Cabeçalho do arquivo de dados lido. proxByteOffset inicial: %lld\n", cabPessoa.proxByteOffset);

    long long proxByteOffset = cabPessoa.proxByteOffset;

    processar_atualizacoes_de_busca(buscas, resultadosEmBuscas, tarefas, &proxByteOffset);
    printf("[DEBUG] proxByteOffset após processar atualizações: %lld\n", proxByteOffset);

    RegistroBuscaPessoa **resultados = malloc(nRegsEncontrados * sizeof(RegistroBuscaPessoa *));
    if (resultados == NULL && nRegsEncontrados > 0)
    {
        printf(FALHA_AO_ALOCAR);
        liberar_resultados_busca(resultadosEmBuscas, buscas);
        free(tarefas);
        return -1;
    }

    copiar_resultados_busca_para_array(resultadosEmBuscas, buscas, resultados, nRegsEncontrados);

    qsort(tarefas, nRegsEncontrados, sizeof(TarefaDeAtualizacao), comparar_tarefas_por_id);
    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_id);

    printf("[DEBUG] Resultados e tarefas ordenados por ID.\n");

    unificarResultados(resultados, tarefas, &nRegsEncontrados);

    int nRemovidosInseridos = 0;
    for (int i = 0; i < nRegsEncontrados; i++)
    {
        int tamanho_novo_dados = sizeof(int) * 4 + resultados[i]->registro->tamanhoNomePessoa + resultados[i]->registro->tamanhoNomeUsuario;
        int tamanho_antigo_dados = resultados[i]->registro->tamanhoRegistro;
        if (tamanho_antigo_dados < tamanho_novo_dados)
        {
            nRemovidosInseridos++;
        }
    }

    printf("[DEBUG] %d registros serão removidos/inseridos.\n", nRemovidosInseridos);
    rewind(fpIndice);

    RegistroIndice **indice_em_memoria = carregar_indice_inteiro(fpIndice, cabPessoa.quantidadePessoas);

    if (indice_em_memoria == NULL && cabPessoa.quantidadePessoas > 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO); // Agora deve imprimir isso
        free(tarefas);
        free(resultados);
        return -1; // E terminar de forma controlada
    }
    printf("[DEBUG] Índice carregado na memória.\n");
    fflush(stdout);

    qsort(tarefas, nRegsEncontrados, sizeof(TarefaDeAtualizacao), comparar_tarefas_por_byteoffset);
    qsort(resultados, nRegsEncontrados, sizeof(RegistroBuscaPessoa *), comparar_registros_busca_offset);
    printf("[DEBUG] Resultados e tarefas ordenados por ByteOffset.\n");

    if (aplicar_atualizacoes_de_busca(fp, indice_em_memoria, tarefas, nRegsEncontrados, nRemovidosInseridos, cabPessoa, resultados) < 0)
    {
        printf(FALHA_AO_PROCESSAR_ARQUIVO);
        free(tarefas);
        free(resultados);
        return -1;
    };

    qsort(indice_em_memoria, cabPessoa.quantidadePessoas, sizeof(RegistroIndice *), comparar_indices_id);
    printf("[DEBUG] Índice em memória re-ordenado por ID.\n");

    reescrever_arquivo_indice(fpIndice, indice_em_memoria, cabPessoa.quantidadePessoas);
    printf("[DEBUG] Arquivo de índice reescrito.\n");

    fflush(fp);
    fseek(fp, 0, SEEK_SET);

    toggle_cabecalho_pessoa(fp, &cabPessoa);
    fflush(fp);
    fflush(fpIndice);

    printf("[DEBUG] Liberando memória...\n");
    if (tarefas != NULL)
    {
        free(tarefas);
    }

    if (resultados != NULL)
    {
        for (int i = 0; i < nRegsEncontrados; i++)
        {
            if (resultados[i] != NULL)
            {
                destroi_registro_pessoa(resultados[i]->registro);
                free(resultados[i]);
            }
        }
        free(resultados);
    }

    printf("--- [DEBUG] FINALIZANDO FUNCIONALIDADE 7 ---\n");
    return 0;
}