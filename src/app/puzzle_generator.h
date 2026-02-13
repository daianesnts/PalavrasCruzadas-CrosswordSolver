#ifndef PUZZLE_GENERATOR_H
#define PUZZLE_GENERATOR_H

#include "../data/structs.h"
#include <stdbool.h>

// Gera puzzle aleatório
bool gerarPuzzleAleatorio(Grid* grid, int linhas, int colunas);

// Gera puzzle por dificuldade (1-fácil, 2-médio, 3-difícil)
bool gerarPuzzlePorDificuldade(Grid* grid, int dificuldade);

#endif // PUZZLE_GENERATOR_H
