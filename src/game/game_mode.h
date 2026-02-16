#ifndef GAME_MODE_H
#define GAME_MODE_H

#include "..\data\structs.h"
#include <stdbool.h>

/*
 * MÓDULO DE CONTROLE DE MODO DE JOGO
 * Este módulo gerencia o modo de jogo atual e o estado
 * de execução do solver.
 * Modos disponíveis:
 * - MODO_SOLVER: Solver automático resolve o puzzle
 * - MODO_USUARIO: Usuário joga manualmente
 */

typedef enum {
  SOLVER_PARADO = 0,     // Solver não está executando
  SOLVER_EXECUTANDO = 1, // Solver está resolvendo
  SOLVER_PAUSADO = 2,    // Solver pausado (para step-by-step)
  SOLVER_CONCLUIDO = 3   // Solver terminou (com ou sem sucesso)
} EstadoExecucaoSolver;

// FUNÇÕES DE MODO DE JOGO

// Obtém o modo de jogo atual
// Retorna: O modo de jogo atual (MODO_SOLVER ou MODO_USUARIO)
ModoJogo obterModoAtual(void);

/*
 * Define o modo de jogo
 * Parâmetros: modo - Novo modo de jogo a ser definido
 * Exemplo de uso:
 *   definirModo(MODO_SOLVER);  // Ativar solver automático
 *   definirModo(MODO_USUARIO); // Permitir usuário jogar
 */
void definirModo(ModoJogo modo);

// Verifica se está no modo solver
// Retorna: true se o modo atual é MODO_SOLVER, false caso contrário
bool estaModoSolver(void);

// Verifica se está no modo usuário
// Retorna: true se o modo atual é MODO_USUARIO, false caso contrário
bool estaModoUsuario(void);

// ESTADO DO SOLVER

// Obtém o estado de execução do solver
// Retorna: Estado atual do solver (parado, executando, pausado, concluído)
EstadoExecucaoSolver obterEstadoSolver(void);

// Define o estado de execução do solver
// Parâmetros: estado - Novo estado do solver
void definirEstadoSolver(EstadoExecucaoSolver estado);

// Verifica se o solver está executando
// Retorna: true se o solver está em execução, false caso contrário
bool solverEstaExecutando(void);

// Verifica se o solver está pausado
// Retorna: true se o solver está pausado, false caso contrário
bool solverEstaPausado(void);

// Verifica se o solver concluiu a execução
// Retorna: true se o solver terminou, false caso contrário
bool solverConcluiu(void);

// FUNÇÕES UTILITÁRIAS

// Reseta o estado do módulo
// Volta ao modo padrão (MODO_USUARIO) e marca solver como parado.
void resetarModoJogo(void);

// Obtém string descritiva do modo atual
// Retorna: String com nome do modo (para debug/UI)
const char *obterNomeModo(void);

// Obtém string descritiva do estado do solver
// Retorna: String com nome do estado (para debug/UI)
const char *obterNomeEstadoSolver(void);

#endif // GAME_MODE_H