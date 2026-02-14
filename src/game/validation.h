#ifndef VALIDATION_H
#define VALIDATION_H

#include "../data/structs.h"
#include <stdbool.h>

// FUNÇÕES PRINCIPAIS

bool validarPalavra(Grid *grid, Palavra *palavra, const char *texto);

// FUNÇÕES AUXILIARES DE VALIDAÇÃO

bool validarTamanho(Palavra *palavra, const char *texto);

bool validarLimites(Grid *grid, Palavra *palavra);

bool validarIntersecoes(Grid *grid, Palavra *palavra, const char *texto);

bool validarConflitos(Grid *grid, Palavra *palavra);

// FUNÇÕES UTILITÁRIAS

int contarLetrasCorretas(Grid *grid, Palavra *palavra, const char *texto);

#endif // VALIDATION_H