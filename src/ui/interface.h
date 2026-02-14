#ifndef INTERFACE_H
#define INTERFACE_H

#include "../data/structs.h"

// Inicializa Interface
void InitInterface(void);

// Funcao principal de renderizar para o grid
void DrawCrosswordGrid(Grid* grid, int offsetX, int offsetY, int cellSize);

// Desenha solver status
void DrawSolverStatus(bool is_running, int steps);

// Atualiza logica para interacoes UI
void UpdateInterface(Grid* grid, EstadoJogo* estado);

// Revela a proxima palavra nao resolvida
void RevealNextUnsolvedWord(Grid* grid);

#endif // INTERFACE_H