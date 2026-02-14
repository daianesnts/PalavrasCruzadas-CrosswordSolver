#include <raylib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../ui/interface.h"
#include "../data/dictionary.h"
#include "../ui/ui_components.h"
#include "../data/structs.h"
#include "../data/grid.h"
#include "../external/api_client.h"
#include "../game/puzzle_generator.h"

// Cenas do Jogo
typedef enum { 
    CENA_MENU, 
    CENA_AJUDA, 
    CENA_IDIOMA, 
    CENA_JOGO,
    CENA_RELATORIO
} CenaJogo;

// Estado Global
static CenaJogo cenaAtual = CENA_MENU;
static char idiomaAtual[3] = "EN"; // EN ou PT

int grandTotalWords = 0;
int levelsCompleted = 0;

void DesenharMenuPrincipal() {
    DrawTextCentered("PALAVRAS CRUZADAS", GetScreenWidth()/2, 100, 50, BLACK);
    
    if (GuiButton((Rectangle){GetScreenWidth()/2 - 100, 250, 200, 50}, "JOGAR")) {
        cenaAtual = CENA_IDIOMA;
    }
    
    if (GuiButton((Rectangle){GetScreenWidth()/2 - 100, 320, 200, 50}, "AJUDA")) {
        cenaAtual = CENA_AJUDA;
    }
    
    if (GuiButton((Rectangle){GetScreenWidth()/2 - 100, 390, 200, 50}, "SAIR")) {
        CloseWindow();
    }
    
    DrawTextCentered("Trabalho EDII - Utilizando Backtracking", GetScreenWidth()/2, GetScreenHeight() - 30, 20, DARKGRAY);
}

void DesenharTelaAjuda() {
    DrawTextCentered("COMO JOGAR", GetScreenWidth()/2, 50, 40, BLACK);
    
    int panelW = 800;
    int panelH = 350;
    int panelX = (GetScreenWidth() - panelW) / 2;
    int panelY = 100;
    
    GuiPanel((Rectangle){panelX, panelY, panelW, panelH});
    
    const char* textoAjuda = 
        "- Use as SETAS ou MOUSE para selecionar os quadrados.\n\n"
        "- Digite as letras para preencher a grade.\n\n"
        "- Leia as dicas na lateral direita (Horizontais e Verticais).\n\n"
        "- Responda conforme o dicionário selecionado.\n\n"
        "- Divirta-se!";
        
    DrawTextWrapped(textoAjuda, (Rectangle){panelX + 30, panelY + 30, panelW - 60, panelH - 60}, 24, BLACK);
    
    if (GuiButton((Rectangle){GetScreenWidth()/2 - 100, 500, 200, 50}, "VOLTAR")) {
        cenaAtual = CENA_MENU;
    }
}

// Estados do jogo
double startTime;
int gameErrors = 0;
int currentLevel = 1;
int maxLevels = 3;

// Modo Infinito
double grandTotalTime = 0;
int grandTotalErrors = 0;

// Configuracoes globais
char globalLanguage[3] = "EN";

// Tela de Relatorio Final
void DesenharTelaRelatorio() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), UI_COLOR_BG);
    
    DrawTextCentered("RELATORIO FINAL", GetScreenWidth()/2, 100, 50, UI_COLOR_PRIMARY);
    
    char statsStr[128];
    
    // Niveis Completados
    snprintf(statsStr, 128, "Niveis Completados: %d", levelsCompleted);
    DrawTextCentered(statsStr, GetScreenWidth()/2, 250, 30, DARKGRAY);
    
    // Palavras Acertadas
    snprintf(statsStr, 128, "Palavras Acertadas: %d", grandTotalWords);
    DrawTextCentered(statsStr, GetScreenWidth()/2, 300, 30, DARKGRAY);

    // Tempo Total
    snprintf(statsStr, 128, "Tempo Total: %.0fs", grandTotalTime);
    DrawTextCentered(statsStr, GetScreenWidth()/2, 350, 30, DARKGRAY);
    
    // Erros Totais
    snprintf(statsStr, 128, "Erros Totais: %d", grandTotalErrors);
    DrawTextCentered(statsStr, GetScreenWidth()/2, 400, 30, DARKGRAY);
    
    if (GuiButton((Rectangle){GetScreenWidth()/2 - 150, 550, 300, 50}, "VOLTAR AO MENU")) {
        cenaAtual = CENA_MENU;
        // Reseta globais
        currentLevel = 1; 
        grandTotalTime = 0; 
        grandTotalErrors = 0;
        grandTotalWords = 0;
        levelsCompleted = 0;
    }
}

void FinalizeGrid(Grid* g);

// Auxiliar para marcar cedulas nao usadas como BLOQUEADAS para melhor UI/Nav
void FinalizeGrid(Grid* g) {
    bool used[TAMANHO_MAX_GRID][TAMANHO_MAX_GRID] = {0};
    
    // Marca celulas ativamente usadas
    for(int i=0; i<g->numPalavras; i++) {
        int r = g->palavras[i].inicio.linha;
        int c = g->palavras[i].inicio.coluna;
        for(int k=0; k<g->palavras[i].tamanho; k++) {
            if (g->palavras[i].direcao == DIRECAO_HORIZONTAL) {
                used[r][c+k] = true;
            } else {
                used[r+k][c] = true;
            }
        }
    }
    
    // Bloqueia nao usadas
    for(int r=0; r<TAMANHO_MAX_GRID; r++) {
        for(int c=0; c<TAMANHO_MAX_GRID; c++) {
            if (!used[r][c]) {
                g->celulas[r][c].tipo = CELULA_BLOQUEADA;
                g->celulas[r][c].letra = '\0';
            }
        }
    }
}

// Auxiliar para auto-numerar grid de forma sequencial
void RecalculateNumbers(Grid* g) {
    int nextNum = 1;
    // Limpar numeros antigos
    for(int y=0; y<g->linhas; y++) {
        for(int x=0; x<g->colunas; x++) {
            g->celulas[y][x].numero = 0;
        }
    }
    
    // Ordem crescente
    for(int y=0; y<g->linhas; y++) {
        for(int x=0; x<g->colunas; x++) {
            bool isStart = false;
            // Checa se a palavra comeca aqui
            for(int i=0; i<g->numPalavras; i++) {
                if (g->palavras[i].inicio.linha == y && g->palavras[i].inicio.coluna == x) {
                    isStart = true;
                    break;
                }
            }
            
            if (isStart) {
                g->celulas[y][x].numero = nextNum;
                // Atualiza a estrutura da palavra com o numero (e lista numeros na lista do grid)
                nextNum++;
            }
        }
    }
}

void LoadLevel(int level, Grid* g) {
    // Resetar Grid
    inicializarGrid(g, TAMANHO_MAX_GRID, TAMANHO_MAX_GRID);
    g->numPalavras = 0;
    
    // Sempre usar Gerador para Progressao Infinita
    if (!Generator_GenerateLevel(g, level)) {
        printf("Generator falhou para nivel %d. Tentando novamente...\n", level);
        if (!Generator_GenerateLevel(g, level)) {
             printf("Erro fatal no Generator.\n");
        }
    }
    
    gameErrors = 0;
    startTime = GetTime();
    RecalculateNumbers(g); // AUTO-AJUSTA NUMEROS
    FinalizeGrid(g);       // MARCA NAO USADOS
    
    // Sorteia palavras pelo numero do grid
    for(int i=0; i<g->numPalavras-1; i++) {
        for(int j=0; j<g->numPalavras-i-1; j++) {
            int numA = g->celulas[g->palavras[j].inicio.linha][g->palavras[j].inicio.coluna].numero;
            int numB = g->celulas[g->palavras[j+1].inicio.linha][g->palavras[j+1].inicio.coluna].numero;
            
            if (numA > numB) {
                Palavra temp = g->palavras[j];
                g->palavras[j] = g->palavras[j+1];
                g->palavras[j+1] = temp;
            }
        }
    }
}

int main() {
    dict_init();
    
    InitWindow(1024, 768, "Palavras Cruzadas - Backtracking");
    SetTargetFPS(60);
    
    InitInterface();
    
    Grid g_val;
    Grid* g = &g_val;
    inicializarGrid(g, 15, 15);
    
    EstadoJogo estado;
    int palavrasCorretas = 0;
    char statusMsg[64] = "";
    bool gameOver = false;
    
    // Inicia Estado
    currentLevel = 1;
    grandTotalTime = 0;
    grandTotalErrors = 0;
    strcpy(globalLanguage, "PT"); // Padrao
    dict_set_language("PT");      // Inicializa Linguagem Dicionario
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(UI_COLOR_BG);
        
        switch(cenaAtual) {
            case CENA_MENU:
                DesenharMenuPrincipal();
                break;
            case CENA_AJUDA:
                DesenharTelaAjuda();
                break;
            case CENA_IDIOMA:
                DrawTextCentered("ESCOLHA O IDIOMA", GetScreenWidth()/2, 100, 40, UI_COLOR_TEXT);
                
                if (GuiButton((Rectangle){GetScreenWidth()/2 - 120, 250, 100, 100}, "EN")) {
                    strcpy(idiomaAtual, "EN");
                    strcpy(globalLanguage, "EN");
                    dict_set_language("EN");
                    LoadLevel(1, g); 
                    cenaAtual = CENA_JOGO; gameOver = false; palavrasCorretas = 0;
                }
                DrawText("Ingles", GetScreenWidth()/2 - 100, 360, 20, UI_COLOR_TEXT);

                if (GuiButton((Rectangle){GetScreenWidth()/2 + 20, 250, 100, 100}, "PT")) {
                    strcpy(idiomaAtual, "PT");
                    strcpy(globalLanguage, "PT");
                    dict_set_language("PT");
                    LoadLevel(1, g); 
                    cenaAtual = CENA_JOGO; gameOver = false; palavrasCorretas = 0;
                }
                DrawText("Portugues", GetScreenWidth()/2 + 25, 360, 20, UI_COLOR_TEXT);
                
                if (GuiButton((Rectangle){GetScreenWidth()/2 - 100, 500, 200, 50}, "VOLTAR")) { cenaAtual = CENA_MENU; }
                break;
                
            case CENA_RELATORIO:
                DesenharTelaRelatorio();
                break;
                
            case CENA_JOGO:
                if (gameOver) {
                    // TELA DE VITORIA
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(UI_COLOR_BG, 0.9f));
                    
                    if (true) { // SEMPRE PERMITIR PROXIMO NIVEL (Modo Infinito)
                         // Tela de proximo nivel
                         if (currentLevel >= 3) {
                             DrawTextCentered("NIVEL CONCLUIDO! (MODO INFINITO)", GetScreenWidth()/2, 200, 40, UI_COLOR_PRIMARY);
                         } else {
                             DrawTextCentered("NIVEL CONCLUIDO!", GetScreenWidth()/2, 200, 40, UI_COLOR_PRIMARY);
                         }
                         
                         char scoreStr[64];
                         snprintf(scoreStr, 64, "Tempo: %.0fs  |  Erros: %d", GetTime() - startTime, gameErrors);
                         DrawTextCentered(scoreStr, GetScreenWidth()/2, 300, 30, UI_COLOR_TEXT);
                         
                         if (GuiButton((Rectangle){GetScreenWidth()/2 - 150, 400, 300, 50}, "PROXIMO NIVEL")) {
                             grandTotalTime += (GetTime() - startTime);
                             grandTotalErrors += gameErrors;
                             grandTotalWords += palavrasCorretas; // Acumula palavras
                             
                             currentLevel++;
                             LoadLevel(currentLevel, g);
                             gameOver = false; palavrasCorretas = 0;
                         }
                         
                         // Opcao de Parar
                         if (GuiButton((Rectangle){GetScreenWidth()/2 - 150, 460, 300, 50}, "PARAR E SAIR")) {
                             // Acumula stats finais antes de sair
                             grandTotalTime += (GetTime() - startTime);
                             grandTotalErrors += gameErrors;
                             grandTotalWords += palavrasCorretas;
                             levelsCompleted = currentLevel; // Completou este nivel
                             
                             cenaAtual = CENA_RELATORIO;
                         }
                    }
                } else {
                    UpdateInterface(g, &estado);
                    
                    // Cabecalho
                    DrawText("Palavras Cruzadas", 40, 20, 30, UI_COLOR_PRIMARY);
                    char infoStr[64];
                    snprintf(infoStr, 64, "Idioma: %s - Nivel %d | Acertos: %d/%d", idiomaAtual, currentLevel, palavrasCorretas, g->numPalavras);
                    DrawText(infoStr, 40, 55, 20, DARKGRAY);
                    
                    // Grid
                    DrawCrosswordGrid(g, 50, 100, 32); 
                    
                    // Dicas
                    int colDicas = 550;
                    int startY = 100;
                    int maxClueWidth = 430; // 1024 - 550 - padding
                    
                    // Horizontal
                    DrawText("HORIZONTAIS", colDicas, startY, 20, UI_COLOR_PRIMARY);
                    DrawLine(colDicas, startY + 25, 950, startY + 25, UI_COLOR_PRIMARY);
                    startY += 30;
                    
                    for(int i=0; i<g->numPalavras; i++) {
                        if (g->palavras[i].direcao == DIRECAO_HORIZONTAL) {
                             char clueLine[256];
                             snprintf(clueLine, 256, "%d. %s", g->celulas[g->palavras[i].inicio.linha][g->palavras[i].inicio.coluna].numero, g->palavras[i].dica);
                             
                             // Quebra de linha manual se necessario
                             char displayLine[256] = "";
                             int currentLineWidth = 0;
                             int lineCount = 1;
                             
                             char* word = strtok(clueLine, " ");
                             while (word != NULL) {
                                int wordW = MeasureText(word, 14);
                                int spaceW = MeasureText(" ", 14);
                                
                                if (currentLineWidth + wordW + spaceW > maxClueWidth) {
                                    strcat(displayLine, "\n   "); // Indentacao na nova linha
                                    currentLineWidth = MeasureText("   ", 14);
                                    lineCount++;
                                }
                                
                                strcat(displayLine, word);
                                strcat(displayLine, " ");
                                currentLineWidth += wordW + spaceW;
                                word = strtok(NULL, " ");
                             }

                             if (startY < 380) { 
                                 DrawText(displayLine, colDicas, startY, 14, UI_COLOR_TEXT);
                                 startY += (18 * lineCount);
                             }
                        }
                    }

                    startY = 400;
                    DrawText("VERTICAIS", colDicas, startY, 20, UI_COLOR_PRIMARY);
                    DrawLine(colDicas, startY + 25, 950, startY + 25, UI_COLOR_PRIMARY);
                    startY += 30;

                     for(int i=0; i<g->numPalavras; i++) {
                        if (g->palavras[i].direcao == DIRECAO_VERTICAL) {
                             char clueLine[256];
                             snprintf(clueLine, 256, "%d. %s", g->celulas[g->palavras[i].inicio.linha][g->palavras[i].inicio.coluna].numero, g->palavras[i].dica);
                             
                             // Quebra de linha manual
                             char displayLine[256] = ""; 
                             // Nota: strtok modifica a string original, entao precisamos de copias se quisessemos reusar clueLine, 
                             // mas aqui eh local loop var.
                             
                             int currentLineWidth = 0;
                             int lineCount = 1;
                             
                             char* word = strtok(clueLine, " ");
                             while (word != NULL) {
                                int wordW = MeasureText(word, 14);
                                int spaceW = MeasureText(" ", 14);
                                
                                if (currentLineWidth + wordW + spaceW > maxClueWidth) {
                                    strcat(displayLine, "\n   "); 
                                    currentLineWidth = MeasureText("   ", 14);
                                    lineCount++;
                                }
                                
                                strcat(displayLine, word);
                                strcat(displayLine, " ");
                                currentLineWidth += wordW + spaceW;
                                word = strtok(NULL, " ");
                             }

                             if (startY < 600) { // um pouco menos espaco para verticais nao baterem nos botoes
                                 DrawText(displayLine, colDicas, startY, 14, UI_COLOR_TEXT);
                                 startY += (18 * lineCount);
                             }
                        }
                    }

                    // Botão Pular Palavra
                    if (GuiButton((Rectangle){ (float)colDicas, 600, 220, 40}, "PULAR PALAVRA")) {
                        RevealNextUnsolvedWord(g);
                        RecalculateNumbers(g); // Apenas por precaucao
                    }

                    if (GuiButton((Rectangle){ (float)colDicas, 650, 220, 40}, "VERIFICAR")) {
                        int currentCorrect = 0;
                        for(int i=0; i<g->numPalavras; i++) {
                            Palavra* p = &g->palavras[i];
                            bool correct = true;
                            if (p->resposta[0] != '\0') {
                                 int r = p->inicio.linha;
                                 int c = p->inicio.coluna;
                                 for(int k=0; k<p->tamanho; k++) {
                                     if (toupper(g->celulas[r][c].letra) != toupper(p->resposta[k])) {
                                         correct = false; break;
                                     }
                                     if (p->direcao == DIRECAO_HORIZONTAL) c++; else r++;
                                 }
                            } else correct = false;
                            
                            if (correct) {
                                currentCorrect++;
                                p->estaCompleta = true; 
                                 int r = p->inicio.linha;
                                 int c = p->inicio.coluna;
                                 for(int k=0; k<p->tamanho; k++) {
                                     g->celulas[r][c].eFixa = true; 
                                     if (p->direcao == DIRECAO_HORIZONTAL) c++; else r++;
                                 }
                            }
                        }
                        
                        if (currentCorrect > palavrasCorretas) {
                             snprintf(statusMsg, 64, "Muito bem!");
                        } else {
                             gameErrors++;
                             snprintf(statusMsg, 64, "Tente novamente.");
                        }
                        palavrasCorretas = currentCorrect;
                        
                        if (palavrasCorretas == g->numPalavras) {
                            gameOver = true;
                        }
                    }
                    
                    DrawText(statusMsg, colDicas, 700, 20, UI_COLOR_CORRECT);
                    
                    if (GuiButton((Rectangle){40, 680, 120, 40}, "VOLTAR")) {
                        cenaAtual = CENA_MENU;
                    }
                }
                break;
        }

        EndDrawing();
    }
    
    api_cleanup();
    CloseWindow();
    return 0;
}