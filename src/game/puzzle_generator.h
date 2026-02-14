#ifndef PUZZLE_GENERATOR_H
#define PUZZLE_GENERATOR_H

#include "../data/structs.h"
#include <stdbool.h>

// Inicializa generador de recursos (alimenta random)
void Generator_Init(void);

// Gera um novo nivel aleatorio no grid providenciado
// Gera um novo nivel aleatorio no grid providenciado
// Usa hardcoded templates para estrutura e Solver para preencher conteudo
bool Generator_GenerateLevel(Grid* grid, int levelIndex);

// Reseta o rastreamento de palavras usadas na sessao
void Generator_ResetSession(void);

#endif // PUZZLE_GENERATOR_H
