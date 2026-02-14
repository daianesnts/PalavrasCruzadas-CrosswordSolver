#include "interface.h"
#include "ui_components.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>

static int selectedX = -1;
static int selectedY = -1;

// Estado de animacao
static float cellScales[TAMANHO_MAX_GRID][TAMANHO_MAX_GRID];

void InitInterface(void) {
    // Inicializa estados de animacao
    for (int y = 0; y < TAMANHO_MAX_GRID; y++) {
        for (int x = 0; x < TAMANHO_MAX_GRID; x++) {
            cellScales[y][x] = 1.0f;
        }
    }
}

// Desenha o grid
void DrawCrosswordGrid(Grid* grid, int offsetX, int offsetY, int cellSize) {
    if (!grid) return;

    int rows = (grid->linhas > 0) ? grid->linhas : TAMANHO_MAX_GRID;
    int cols = (grid->colunas > 0) ? grid->colunas : TAMANHO_MAX_GRID;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            Rectangle rect = {
                (float)(offsetX + x * cellSize),
                (float)(offsetY + y * cellSize),
                (float)cellSize,
                (float)cellSize
            };

            // Determina cor da cedula
            Color bgColor = UI_COLOR_BG; // Cedula vazia padrao
            
            if (grid->celulas[y][x].tipo == CELULA_BLOQUEADA) {
                bgColor = UI_COLOR_BLOCK; // Bloco azul
            } else if (x == selectedX && y == selectedY) {
                bgColor = UI_COLOR_ACCENT; // Selecao amarela
            } else {
                 bgColor = WHITE;
            }

            // Desenha fundo da cedula
            DrawRectangleRec(rect, bgColor);
            
            // Desenha bordas (Linhas Azuis)
            DrawRectangleLinesEx(rect, 1.0f, UI_COLOR_GRID_LINE);

            // Desenha numeros se existir
            if (grid->celulas[y][x].numero > 0) {
                DrawText(TextFormat("%d", grid->celulas[y][x].numero), 
                         (int)rect.x + 3, (int)rect.y + 3, 10, UI_COLOR_TEXT);
            }

            // Desenha letra
            char letra = grid->celulas[y][x].letra;
            if (letra != '\0') {
                char letterStr[2] = { letra, '\0' };
                int fontSize = (int)(cellSize * 0.6f);
                int textW = MeasureText(letterStr, fontSize);
                
                Color txtColor = UI_COLOR_TEXT;
                if (grid->celulas[y][x].eFixa) { // Usando eFixa para significar "Verificado/Correto"
                     txtColor = UI_COLOR_CORRECT;
                     DrawText(letterStr, 
                         (int)(rect.x + (cellSize - textW) / 2) + 2, 
                         (int)(rect.y + (cellSize - fontSize) / 2 + 2) + 2, 
                         fontSize, Fade(UI_COLOR_CORRECT, 0.5f));
                }
                
                // Centraliza letras
                DrawText(letterStr, 
                         (int)(rect.x + (cellSize - textW) / 2), 
                         (int)(rect.y + (cellSize - fontSize) / 2 + 2), 
                         fontSize, txtColor);
            }
        }
    }
}

void DrawSolverStatus(bool is_running, int steps) {
    if (is_running) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, 0.8f));
        DrawText("Solver Running...", GetScreenWidth()/2 - 100, GetScreenHeight()/2 - 20, 30, UI_COLOR_ACCENT);
        
        char stepStr[32];
        sprintf(stepStr, "Steps: %d", steps);
        DrawText(stepStr, GetScreenWidth()/2 - 50, GetScreenHeight()/2 + 20, 20, GRAY);
    }
}

void UpdateInterface(Grid* grid, EstadoJogo* estado) {
    if (!grid) return;
    
    // Navegacao pelo teclado
    if (IsKeyPressed(KEY_RIGHT)) {
        int nx = selectedX;
        do { nx = (nx + 1) % grid->colunas; } while(grid->celulas[selectedY][nx].tipo == CELULA_BLOQUEADA && nx != selectedX);
        selectedX = nx;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        int nx = selectedX;
        do { nx = (nx - 1 + grid->colunas) % grid->colunas; } while(grid->celulas[selectedY][nx].tipo == CELULA_BLOQUEADA && nx != selectedX);
        selectedX = nx;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        int ny = selectedY;
        do { ny = (ny + 1) % grid->linhas; } while(grid->celulas[ny][selectedX].tipo == CELULA_BLOQUEADA && ny != selectedY);
        selectedY = ny;
    }
    if (IsKeyPressed(KEY_UP)) {
        int ny = selectedY;
        do { ny = (ny - 1 + grid->linhas) % grid->linhas; } while(grid->celulas[ny][selectedX].tipo == CELULA_BLOQUEADA && ny != selectedY);
        selectedY = ny;
    }

    // Selecao com mouse
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        int offsetX = 50; 
        int offsetY = 100;
        int cellSize = 32;

        int gx = (int)(mouse.x - offsetX) / cellSize;
        int gy = (int)(mouse.y - offsetY) / cellSize;
        
        int rows = (grid->linhas > 0) ? grid->linhas : TAMANHO_MAX_GRID;
        int cols = (grid->colunas > 0) ? grid->colunas : TAMANHO_MAX_GRID;

        if (gx >= 0 && gx < cols && gy >= 0 && gy < rows) {
             // So seleciona se nao tiver bloqueado
             if (grid->celulas[gy][gx].tipo != CELULA_BLOQUEADA) {
                 selectedX = gx;
                 selectedY = gy;
                 cellScales[gy][gx] = 1.2f;   
             }
        }
    }

    // Interacao com o teclado (Digitacao)
    if (selectedX != -1 && selectedY != -1) {
        // Pula checagem para MODO_USUARIO se passando simple stade
        if (grid->celulas[selectedY][selectedX].eFixa || grid->celulas[selectedY][selectedX].tipo == CELULA_BLOQUEADA) {
             // Nao faz nada se fixado ou bloqueado
        } else {
            int key = GetKeyPressed();
            if ((key >= 65 && key <= 90) || (key >= 97 && key <= 122)) { // A-Z
                 if (key >= 97) key -= 32; // Maiuscula
                 grid->celulas[selectedY][selectedX].letra = (char)key;
                 grid->celulas[selectedY][selectedX].tipo = CELULA_PREENCHIDA;
                 
                 // Animacao Pop
                 cellScales[selectedY][selectedX] = 1.5f; 
            } else if (key == KEY_BACKSPACE || key == KEY_DELETE) {
                 grid->celulas[selectedY][selectedX].letra = '\0';
                 grid->celulas[selectedY][selectedX].tipo = CELULA_VAZIA;
                 cellScales[selectedY][selectedX] = 0.8f; 
            }
        }
    }
}

void RevealNextUnsolvedWord(Grid* grid) {
    if (!grid) return;

    // Itera sobre as palavras (assumindo que estao ordenadas por numero, como feito no main.c)
    for (int i = 0; i < grid->numPalavras; i++) {
        Palavra* p = &grid->palavras[i];
        bool isSolved = true;
        
        // Verifica se a palavra esta correta no grid
        int r = p->inicio.linha;
        int c = p->inicio.coluna;
        
        // Se ja marcada como completa, pula
        // Mas vamos verificar as celulas tambem para garantir
        for (int k = 0; k < p->tamanho; k++) {
            char cellLetra = grid->celulas[r][c].letra;
            // Se letra vazia ou diferente da resposta
            if (toupper(cellLetra) != toupper(p->resposta[k])) {
                isSolved = false;
                break;
            }
            if (p->direcao == DIRECAO_HORIZONTAL) c++; else r++;
        }

        if (!isSolved) {
            // Revelar esta palavra!
            r = p->inicio.linha;
            c = p->inicio.coluna;
            for (int k = 0; k < p->tamanho; k++) {
                grid->celulas[r][c].letra = p->resposta[k];
                grid->celulas[r][c].tipo = CELULA_PREENCHIDA;
                // Dado "Pular Palavra"
                // Vamos preencher.
                if (p->direcao == DIRECAO_HORIZONTAL) c++; else r++;
            }
            return; // Revela apenas uma
        }
    }
}