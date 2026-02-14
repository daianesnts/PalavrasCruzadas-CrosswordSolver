#ifndef SOLVER_H
#define SOLVER_H

#include "..\data\structs.h"
#include <stdbool.h>

// ESTRUTURAS

typedef struct {
  int indicePalavraAtual;
  int tentativas;
  int retrocessos; // Número de backtracks realizados
  bool concluido;
  bool encontrouSolucao;
  int profundidade; // Profundidade atual da recursão
  char ultimaPalavra[TAMANHO_MAX_PALAVRA + 1]; // Última palavra tentada
} EstadoSolver;

// FUNÇÕES PRINCIPAIS

bool resolverPuzzle(Grid *grid, EstadoSolver *estado);

void inicializarSolver(Grid *grid, EstadoSolver *estado);

bool proximoPasso(Grid *grid, EstadoSolver *estado);

// FUNÇÕES AUXILIARES

void obterEstatisticas(EstadoSolver *estado, int *tentativas, int *retrocessos);

void resetarSolver(EstadoSolver *estado);

bool verificarSolucao(Grid *grid);

#endif // SOLVER_H