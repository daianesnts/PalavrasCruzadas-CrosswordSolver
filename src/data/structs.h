#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

// CONSTANTES
#define TAMANHO_MAX_GRID 15 // Tamanho máximo do grid (15x15)
#define MAX_PALAVRAS 50
#define TAMANHO_MAX_PALAVRA 15
#define TAMANHO_MAX_DICA 200

//Tipo de célula no grid
typedef enum {
    CELULA_VAZIA = 0,        // Célula vazia (pode ser preenchida)
    CELULA_BLOQUEADA = 1,    // Célula bloqueada (azul)
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
    Posicao inicio;
    Direcao direcao;
    int tamanho;
    char dica[TAMANHO_MAX_DICA];
    char resposta[TAMANHO_MAX_PALAVRA + 1];
    char textoAtual[TAMANHO_MAX_PALAVRA + 1];
    bool estaCompleta;
} Palavra;

//Grid (tabuleiro) de palavras cruzadas
typedef struct {
    Celula celulas[TAMANHO_MAX_GRID][TAMANHO_MAX_GRID];
    int linhas;
    int colunas;
    Palavra palavras[MAX_PALAVRAS];
    int numPalavras;
} Grid;

//Estado completo do jogo
typedef struct {
    Grid grid;
    ModoJogo modo;
    int tentativas;
    int retrocessos; // Número de backtracks
    bool estaCompleto;
    bool estaResolvido;
} EstadoJogo;

#endif 
