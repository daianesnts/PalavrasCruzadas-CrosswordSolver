#ifndef SOLVER_H
#define SOLVER_H

#include "..\data\structs.h"
#include <stdbool.h>

/*
 * MÓDULO SOLVER - BACKTRACKING
 * Este módulo implementa o algoritmo de backtracking para resolver
 * puzzles de palavras cruzadas automaticamente.
 * Funcionalidades:
 * - Resolução completa automática
 * - Resolução passo-a-passo (para animação na UI)
 * - Estatísticas de execução (tentativas, backtracks)
 */

// ESTRUTURAS

/*
 * Estado do solver durante a execução
 * Mantém informações sobre o progresso da resolução,
 * permitindo execução passo-a-passo e coleta de estatísticas.
 */
typedef struct {
  int indicePalavraAtual; // Índice da palavra sendo processada
  int tentativas;         // Número total de tentativas
  int retrocessos;        // Número de backtracks realizados
  bool concluido;         // Se a resolução foi concluída
  bool encontrouSolucao;  // Se encontrou uma solução válida
  int profundidade;       // Profundidade atual da recursão
  char ultimaPalavra[TAMANHO_MAX_PALAVRA + 1]; // Última palavra tentada
} EstadoSolver;

// FUNÇÕES PRINCIPAIS

/*
 * Resolve o puzzle completo de uma vez
 * Executa o algoritmo de backtracking até encontrar uma solução
 * ou determinar que não há solução possível.
 * Parâmetros:
 *   grid - Ponteiro para o grid a ser resolvido
 *   estado - Ponteiro para estrutura que receberá estatísticas
 * Retorna:
 *   true se encontrou solução, false caso contrário
 * Exemplo de uso:
 *   EstadoSolver estado;
 *   if (resolverPuzzle(&grid, &estado))
 *       printf("Resolvido em %d tentativas!\n", estado.tentativas);
 */
bool resolverPuzzle(Grid *grid, EstadoSolver *estado);

/*
 * Inicializa o estado do solver para execução passo-a-passo
 * Prepara as estruturas necessárias para resolver o puzzle
 * de forma incremental, permitindo animação na interface.
 * Parâmetros:
 *   grid - Ponteiro para o grid a ser resolvido
 *   estado - Ponteiro para estrutura de estado a ser inicializada
 */
void inicializarSolver(Grid *grid, EstadoSolver *estado);

/*
 * Executa um passo do algoritmo de backtracking
 * Avança a resolução em um passo, permitindo que a UI
 * atualize a visualização entre cada tentativa.
 * Parâmetros:
 *   grid - Ponteiro para o grid
 *   estado - Ponteiro para o estado do solver
 * Retorna:
 *   true se ainda há trabalho a fazer, false se concluiu
 * Exemplo de uso:
 *   EstadoSolver estado;
 *   inicializarSolver(&grid, &estado);
 *   // Atualizar UI
 *   while (proximoPasso(&grid, &estado))
 *       Sleep(100);
 */
bool proximoPasso(Grid *grid, EstadoSolver *estado);

// FUNÇÕES AUXILIARES

/*
 * Obtém estatísticas da execução do solver
 * Parâmetros:
 *   estado - Ponteiro para o estado do solver
 *   tentativas - Ponteiro para receber número de tentativas (pode ser NULL)
 *   retrocessos - Ponteiro para receber número de backtracks (pode ser NULL)
 */
void obterEstatisticas(EstadoSolver *estado, int *tentativas, int *retrocessos);

/*
 * Reseta o estado do solver
 * Limpa todas as estatísticas e reinicia o estado para uma nova execução.
 * Parâmetros:
 *   estado - Ponteiro para o estado a ser resetado
 */
void resetarSolver(EstadoSolver *estado);

/*
 * Verifica se o puzzle está resolvido corretamente
 * Valida se todas as palavras foram preenchidas e se
 * todas as restrições foram satisfeitas.
 * Parâmetros:
 *   grid - Ponteiro para o grid
 * Retorna:
 *   true se o puzzle está completamente resolvido
 */
bool verificarSolucao(Grid *grid);

#endif // SOLVER_H