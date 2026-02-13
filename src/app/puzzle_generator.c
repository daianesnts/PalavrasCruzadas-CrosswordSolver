#include "puzzle_generator.h"
#include "puzzle_generator.h"
#include "../data/grid.h"
#include <stdlib.h>
#include <time.h>

static void initRandom() {
    static int seeded = 0;

    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
}

bool gerarPuzzleAleatorio(Grid* grid, int linhas, int colunas) {
    initRandom();

    // Valida tamanho
    if (linhas <= 0 || linhas > 15 || colunas <= 0 || colunas > 15)
        return false;

    // Inicializa grid (TUDO BLOQUEADO por padrão)
    inicializarGrid(grid, linhas, colunas);

    // Decide quantas células serão VAZIAS (30-50% do grid)
    int totalCelulas = linhas * colunas;
    int numVazias = totalCelulas * (0.3 + (rand() % 20) / 100.0);

    // Abre células aleatórias (transforma BLOQUEADA em VAZIA)
    for (int i = 0; i < numVazias; i++) {
        int l = rand() % linhas;
        int c = rand() % colunas;
        Celula* cel = obterCelula(grid, l, c);
        cel->tipo = CELULA_VAZIA;  // Agora pode ser preenchida
    }

    return true;
}

bool gerarPuzzlePorDificuldade(Grid* grid, int dificuldade) {
    // Fácil: mais células vazias, grid menor
    // Difícil: menos células vazias, grid maior
    int tamanho;
    switch(dificuldade) {
        case 1: // Fácil - 8x8, muitas vazias
            tamanho = 8;
            break;
        case 2: // Médio - 10x10
            tamanho = 10;
            break;
        case 3: // Difícil - 12x12, mais bloqueadas
            tamanho = 12;
            break;
        default:
            tamanho = 8;
    }

    return gerarPuzzleAleatorio(grid, tamanho, tamanho);
}
