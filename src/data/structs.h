#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

// CONSTANTES
#define TAMANHO_MAX_GRID 15        // Tamanho máximo do grid (15x15)
#define MAX_PALAVRAS 50            // Número máximo de palavras no puzzle
#define TAMANHO_MAX_PALAVRA 15     // Tamanho máximo de uma palavra
#define TAMANHO_MAX_DICA 200       // Tamanho máximo da dica

//Tipo de célula no grid
typedef enum {
    CELULA_VAZIA = 0,        // Célula vazia (pode ser preenchida)
    CELULA_BLOQUEADA = 1,    // Célula bloqueada (preta)
    CELULA_PREENCHIDA = 2    // Célula com letra
} TipoCelula;

//Direção da palavra no grid
typedef enum { 
    DIRECAO_HORIZONTAL = 0,
    DIRECAO_VERTICAL = 1
} Direcao;

//Modo de jogo
typedef enum {
    MODO_SOLVER = 0,
    MODO_USUARIO = 1
} ModoJogo;

//Célula no grid
typedef struct {
    TipoCelula tipo;    // Tipo da célula (vazia, bloqueada, preenchida)
    char letra;         // Letra atual ('\0' se vazia)
    bool eFixa;         // true = letra fixa (não pode ser alterada)
    int numero;         // Número da célula, 0 se não numerada
} Celula;

//Posição no grid
typedef struct {
    int linha;    // Linha: 0 a TAMANHO_MAX_GRID-1 (de cima para baixo)
    int coluna;   // Coluna 0 a TAMANHO_MAX_GRID-1 (da esquerda para direita)
} Posicao;

//Palavra no puzzle
typedef struct {
    Posicao inicio;                           // Posição inicial
    Direcao direcao;                          // Direção da palavra
    int tamanho;                              // Tamanho da palavra
    char dica[TAMANHO_MAX_DICA];             // Significado
    char resposta[TAMANHO_MAX_PALAVRA + 1];  // Resposta correta
    char textoAtual[TAMANHO_MAX_PALAVRA + 1]; // Texto atual preenchido
    bool estaCompleta;                        // Se está completamente preenchida
} Palavra;

//Grid (tabuleiro) de palavras cruzadas
typedef struct {
    Celula celulas[TAMANHO_MAX_GRID][TAMANHO_MAX_GRID];  // Matriz de células
    int linhas;                                           // Linhas usadas
    int colunas;                                          // Colunas usadas
    Palavra palavras[MAX_PALAVRAS];                       // Array de palavras
    int numPalavras;                                      // Número de palavras
} Grid;

//Estado completo do jogo
typedef struct {
    Grid grid;              // Grid atual
    ModoJogo modo;          // Modo de jogo
    int tentativas;         // Número de tentativas
    int retrocessos;        // Número de backtracks
    bool estaCompleto;      // Se o puzzle está completo
    bool estaResolvido;     // Se está resolvido corretamente
} EstadoJogo;

#endif 
