#ifndef VALIDATION_H
#define VALIDATION_H

#include "../data/structs.h"
#include <stdbool.h>

/*
MÓDULO DE VALIDAÇÃO
Este módulo é responsável por validar se uma palavra pode ser
colocada em uma posição específica do grid de palavras cruzadas.
Validações realizadas:
- Tamanho da palavra corresponde ao espaço disponível
- Palavra cabe dentro dos limites do grid
- Não há conflitos com células bloqueadas
- Letras em interseções são compatíveis
 */

// FUNÇÕES PRINCIPAIS

/*
Valida se uma palavra pode ser colocada em uma posição específica
Esta é a função principal de validação que executa todas as verificações
necessárias antes de colocar uma palavra no grid.
 */
bool validarPalavra(Grid *grid, Palavra *palavra, const char *texto);

// FUNÇÕES AUXILIARES DE VALIDAÇÃO

//Valida se o tamanho do texto corresponde ao tamanho da palavra
bool validarTamanho(Palavra *palavra, const char *texto);

/*
Valida se a palavra cabe dentro dos limites do grid
Verifica se, dada a posição inicial e direção, a palavra
não ultrapassa os limites do grid.
 */
bool validarLimites(Grid *grid, Palavra *palavra);

/*
Valida interseções com outras palavras já colocadas
Verifica se as letras da palavra são compatíveis com letras
já existentes no grid (em pontos de cruzamento).
 */
bool validarIntersecoes(Grid *grid, Palavra *palavra, const char *texto);

/*
Valida se não há conflitos com células bloqueadas
Verifica se a palavra não tenta ocupar células bloqueadas
(células pretas do tabuleiro).
 */
bool validarConflitos(Grid *grid, Palavra *palavra);

// FUNÇÕES UTILITÁRIAS

/*
Conta quantas letras da palavra já estão corretas no grid
Útil para heurísticas de ordenação de palavras no solver.
 */
int contarLetrasCorretas(Grid *grid, Palavra *palavra, const char *texto);



#endif // VALIDATION_H
