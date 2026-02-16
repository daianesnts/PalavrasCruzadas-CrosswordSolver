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

// Cenas do Jogo
typedef enum { 
    CENA_MENU, 
    CENA_AJUDA, 
    CENA_IDIOMA, 
    CENA_JOGO 
} CenaJogo;

// Estado Global
static CenaJogo cenaAtual = CENA_MENU;
static char idiomaAtual[3] = "EN"; // EN ou PT

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
    
    DrawTextCentered("v2.0 - Edicao Jornal", GetScreenWidth()/2, GetScreenHeight() - 30, 20, DARKGRAY);
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
        "- Responda em INGLÊS conforme o dicionário selecionado.\n\n"
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
double grandTotalTime = 0;
int grandTotalErrors = 0;
char globalLanguage[4] = "PT";

void FinalizeGrid(Grid* g);

// Auxiliar para marcar cedulas nao usadas como BLOQUEADAS para melhor UI/Nav
void FinalizeGrid(Grid* g) {
    bool used[TAMANHO_MAX_GRID][TAMANHO_MAX_GRID] = {0};
    
    // Marca actively used cells
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
    // Limpar numeros velhos
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
    inicializarGrid(g, 15, 15);
    g->numPalavras = 0;
    
    if (strcmp(globalLanguage, "PT") == 0) {
        if (level == 1) {
            // NIVEL 1: (12 Palavras)
            g->palavras[0].inicio = (Posicao){4, 2}; g->palavras[0].direcao = DIRECAO_HORIZONTAL; g->palavras[0].tamanho = 8;
            colocarPalavra(g, &g->palavras[0], "FLORESTA"); strcpy(g->palavras[0].dica, "Grande area coberta de arvores."); g->numPalavras++;

            g->palavras[1].inicio = (Posicao){4, 2}; g->palavras[1].direcao = DIRECAO_VERTICAL; g->palavras[1].tamanho = 4;
            colocarPalavra(g, &g->palavras[1], "FOGO"); strcpy(g->palavras[1].dica, "Elemento que queima."); g->numPalavras++;

            g->palavras[2].inicio = (Posicao){4, 5}; g->palavras[2].direcao = DIRECAO_VERTICAL; g->palavras[2].tamanho = 3;
            colocarPalavra(g, &g->palavras[2], "RIO"); strcpy(g->palavras[2].dica, "Curso de agua doce."); g->numPalavras++;

            g->palavras[3].inicio = (Posicao){6, 4}; g->palavras[3].direcao = DIRECAO_HORIZONTAL; g->palavras[3].tamanho = 3;
            colocarPalavra(g, &g->palavras[3], "SOL"); strcpy(g->palavras[3].dica, "Estrela central."); g->numPalavras++;

            g->palavras[4].inicio = (Posicao){4, 3}; g->palavras[4].direcao = DIRECAO_VERTICAL; g->palavras[4].tamanho = 3;
            colocarPalavra(g, &g->palavras[4], "LUA"); strcpy(g->palavras[4].dica, "Satelite natural."); g->numPalavras++;

            g->palavras[5].inicio = (Posicao){7, 2}; g->palavras[5].direcao = DIRECAO_HORIZONTAL; g->palavras[5].tamanho = 4;
            colocarPalavra(g, &g->palavras[5], "ONDA"); strcpy(g->palavras[5].dica, "Movimento do mar."); g->numPalavras++;

            g->palavras[6].inicio = (Posicao){7, 5}; g->palavras[6].direcao = DIRECAO_VERTICAL; g->palavras[6].tamanho = 4;
            colocarPalavra(g, &g->palavras[6], "AGUA"); strcpy(g->palavras[6].dica, "Liquido vital."); g->numPalavras++;

            g->palavras[7].inicio = (Posicao){9, 3}; g->palavras[7].direcao = DIRECAO_HORIZONTAL; g->palavras[7].tamanho = 5;
            colocarPalavra(g, &g->palavras[7], "FAUNA"); strcpy(g->palavras[7].dica, "Animais de uma regiao."); g->numPalavras++;

            g->palavras[8].inicio = (Posicao){3, 6}; g->palavras[8].direcao = DIRECAO_VERTICAL; g->palavras[8].tamanho = 3;
            colocarPalavra(g, &g->palavras[8], "CEU"); strcpy(g->palavras[8].dica, "Espaco acima (azul)."); g->numPalavras++;

            g->palavras[9].inicio = (Posicao){0, 5}; g->palavras[9].direcao = DIRECAO_HORIZONTAL; g->palavras[9].tamanho = 5;
            colocarPalavra(g, &g->palavras[9], "VENTO"); strcpy(g->palavras[9].dica, "Ar em movimento."); g->numPalavras++;

            g->palavras[10].inicio = (Posicao){0, 5}; g->palavras[10].direcao = DIRECAO_VERTICAL; g->palavras[10].tamanho = 4;
            colocarPalavra(g, &g->palavras[10], "VIDA"); strcpy(g->palavras[10].dica, "Estado de seres organicos."); g->numPalavras++;

            g->palavras[11].inicio = (Posicao){11, 4}; g->palavras[11].direcao = DIRECAO_HORIZONTAL; g->palavras[11].tamanho = 5;
            colocarPalavra(g, &g->palavras[11], "PEDRA"); strcpy(g->palavras[11].dica, "Materia mineral dura."); g->numPalavras++;
        
        } else if (level == 2) {
            // NIVEL 2: (12 Palavras)
            g->palavras[0].inicio = (Posicao){7, 4}; g->palavras[0].direcao = DIRECAO_HORIZONTAL; g->palavras[0].tamanho = 6;
            colocarPalavra(g, &g->palavras[0], "CIDADE"); strcpy(g->palavras[0].dica, "Zona urbana."); g->numPalavras++;

            g->palavras[1].inicio = (Posicao){7, 4}; g->palavras[1].direcao = DIRECAO_VERTICAL; g->palavras[1].tamanho = 4;
            colocarPalavra(g, &g->palavras[1], "CASA"); strcpy(g->palavras[1].dica, "Lugar de moradia."); g->numPalavras++;

            g->palavras[2].inicio = (Posicao){8, 4}; g->palavras[2].direcao = DIRECAO_HORIZONTAL; g->palavras[2].tamanho = 4;
            colocarPalavra(g, &g->palavras[2], "ARTE"); strcpy(g->palavras[2].dica, "Expressao criativa."); g->numPalavras++;

            g->palavras[3].inicio = (Posicao){9, 3}; g->palavras[3].direcao = DIRECAO_HORIZONTAL; g->palavras[3].tamanho = 6;
            colocarPalavra(g, &g->palavras[3], "ESCOLA"); strcpy(g->palavras[3].dica, "Lugar de ensino."); g->numPalavras++;

            g->palavras[4].inicio = (Posicao){9, 7}; g->palavras[4].direcao = DIRECAO_VERTICAL; g->palavras[4].tamanho = 4;
            colocarPalavra(g, &g->palavras[4], "LOJA"); strcpy(g->palavras[4].dica, "Estabelecimento comercial."); g->numPalavras++;

            g->palavras[5].inicio = (Posicao){11, 6}; g->palavras[5].direcao = DIRECAO_HORIZONTAL; g->palavras[5].tamanho = 4;
            colocarPalavra(g, &g->palavras[5], "JOGO"); strcpy(g->palavras[5].dica, "Atividade ludica."); g->numPalavras++;

            g->palavras[6].inicio = (Posicao){5, 0}; g->palavras[6].direcao = DIRECAO_HORIZONTAL; g->palavras[6].tamanho = 5;
            colocarPalavra(g, &g->palavras[6], "CARRO"); strcpy(g->palavras[6].dica, "Veiculo automotor."); g->numPalavras++;

            g->palavras[7].inicio = (Posicao){5, 2}; g->palavras[7].direcao = DIRECAO_VERTICAL; g->palavras[7].tamanho = 3;
            colocarPalavra(g, &g->palavras[7], "RUA"); strcpy(g->palavras[7].dica, "Via publica."); g->numPalavras++;

            g->palavras[8].inicio = (Posicao){9, 8}; g->palavras[8].direcao = DIRECAO_VERTICAL; g->palavras[8].tamanho = 5;
            colocarPalavra(g, &g->palavras[8], "ALUNO"); strcpy(g->palavras[8].dica, "Estudante."); g->numPalavras++;

            g->palavras[9].inicio = (Posicao){12, 6}; g->palavras[9].direcao = DIRECAO_HORIZONTAL; g->palavras[9].tamanho = 5;
            colocarPalavra(g, &g->palavras[9], "BANCO"); strcpy(g->palavras[9].dica, "Instituicao financeira."); g->numPalavras++;
            
            g->palavras[10].inicio = (Posicao){5, 3}; g->palavras[10].direcao = DIRECAO_VERTICAL; g->palavras[10].tamanho = 4;
            colocarPalavra(g, &g->palavras[10], "REDE"); strcpy(g->palavras[10].dica, "Sistema de conexao."); g->numPalavras++;

            g->palavras[11].inicio = (Posicao){7, 1}; g->palavras[11].direcao = DIRECAO_HORIZONTAL; g->palavras[11].tamanho = 4;
            colocarPalavra(g, &g->palavras[11], "MEDO"); strcpy(g->palavras[11].dica, "Reacao ao perigo."); g->numPalavras++;

            // NOVAS PALAVRAS (16 Palavras)
            g->palavras[12].inicio = (Posicao){9, 10}; g->palavras[12].direcao = DIRECAO_VERTICAL; g->palavras[12].tamanho = 4;
            colocarPalavra(g, &g->palavras[12], "POVO"); strcpy(g->palavras[12].dica, "Conjunto de pessoas."); g->numPalavras++;

            g->palavras[13].inicio = (Posicao){0, 8}; g->palavras[13].direcao = DIRECAO_HORIZONTAL; g->palavras[13].tamanho = 5;
            colocarPalavra(g, &g->palavras[13], "CLUBE"); strcpy(g->palavras[13].dica, "Associacao recreativa."); g->numPalavras++;

            g->palavras[14].inicio = (Posicao){0, 0}; g->palavras[14].direcao = DIRECAO_HORIZONTAL; g->palavras[14].tamanho = 4;
            colocarPalavra(g, &g->palavras[14], "AREA"); strcpy(g->palavras[14].dica, "Espaco delimitado."); g->numPalavras++;

            g->palavras[15].inicio = (Posicao){0, 0}; g->palavras[15].direcao = DIRECAO_VERTICAL; g->palavras[15].tamanho = 2;
            colocarPalavra(g, &g->palavras[15], "AR"); strcpy(g->palavras[15].dica, "O que respiramos."); g->numPalavras++;

            g->palavras[5].inicio = (Posicao){11, 7}; // Atualiza posicao do jogo
            
        } else if (level == 3) {
            // LEVEL 3: (Dificil - 10 Palavras)
            g->palavras[0].inicio = (Posicao){7, 3}; g->palavras[0].direcao = DIRECAO_HORIZONTAL; g->palavras[0].tamanho = 8;
            colocarPalavra(g, &g->palavras[0], "UNIVERSO"); strcpy(g->palavras[0].dica, "Tudo o que existe."); g->numPalavras++;
            
            g->palavras[1].inicio = (Posicao){7, 6}; g->palavras[1].direcao = DIRECAO_VERTICAL; g->palavras[1].tamanho = 4;
            colocarPalavra(g, &g->palavras[1], "VIDA"); strcpy(g->palavras[1].dica, "Existencia."); g->numPalavras++;

            g->palavras[2].inicio = (Posicao){4, 4}; g->palavras[2].direcao = DIRECAO_VERTICAL; g->palavras[2].tamanho = 7;
            colocarPalavra(g, &g->palavras[2], "PLANETA"); strcpy(g->palavras[2].dica, "Orbita uma estrela."); g->numPalavras++;

            g->palavras[3].inicio = (Posicao){2, 8}; g->palavras[3].direcao = DIRECAO_VERTICAL; g->palavras[3].tamanho = 3;
            colocarPalavra(g, &g->palavras[3], "LUZ"); strcpy(g->palavras[3].dica, "Radiacao visivel."); g->numPalavras++;
            
            g->palavras[4].inicio = (Posicao){4, 4}; g->palavras[4].direcao = DIRECAO_HORIZONTAL; g->palavras[4].tamanho = 3;
            colocarPalavra(g, &g->palavras[4], "PAZ"); strcpy(g->palavras[4].dica, "Ausencia de guerra."); g->numPalavras++;
            
            g->palavras[5].inicio = (Posicao){10, 4}; g->palavras[5].direcao = DIRECAO_HORIZONTAL; g->palavras[5].tamanho = 5;
            colocarPalavra(g, &g->palavras[5], "ATOMO"); strcpy(g->palavras[5].dica, "Menor parte da materia."); g->numPalavras++;
             
            g->palavras[6].inicio = (Posicao){9, 5}; g->palavras[6].direcao = DIRECAO_VERTICAL; g->palavras[6].tamanho = 5;
            colocarPalavra(g, &g->palavras[6], "TEMPO"); strcpy(g->palavras[6].dica, "Dimensao da duracao."); g->numPalavras++;
            
             g->palavras[7].inicio = (Posicao){2, 0}; g->palavras[7].direcao = DIRECAO_HORIZONTAL; g->palavras[7].tamanho = 6;
             colocarPalavra(g, &g->palavras[7], "COMETA"); strcpy(g->palavras[7].dica, "Corpo celeste gelado."); g->numPalavras++;
             
             g->palavras[8].inicio = (Posicao){12, 3}; g->palavras[8].direcao = DIRECAO_HORIZONTAL; g->palavras[8].tamanho = 6;
             colocarPalavra(g, &g->palavras[8], "ESPACO"); strcpy(g->palavras[8].dica, "Onde estao os astros."); g->numPalavras++;
             
             g->palavras[9].inicio = (Posicao){7, 9}; g->palavras[9].direcao = DIRECAO_VERTICAL; g->palavras[9].tamanho = 6;
             colocarPalavra(g, &g->palavras[9], "SOMBRA"); strcpy(g->palavras[9].dica, "Bloqueio da luz."); g->numPalavras++;

             // NOVAS PALAVRAS (18+)
             g->palavras[10].inicio = (Posicao){0, 0}; g->palavras[10].direcao = DIRECAO_HORIZONTAL; g->palavras[10].tamanho = 3;
             colocarPalavra(g, &g->palavras[10], "CEU"); strcpy(g->palavras[10].dica, "A morada dos astros."); g->numPalavras++;

             g->palavras[11].inicio = (Posicao){14, 0}; g->palavras[11].direcao = DIRECAO_HORIZONTAL; g->palavras[11].tamanho = 3;
             colocarPalavra(g, &g->palavras[11], "GAS"); strcpy(g->palavras[11].dica, "Estado da materia das estrelas."); g->numPalavras++;

             g->palavras[12].inicio = (Posicao){0, 14}; g->palavras[12].direcao = DIRECAO_VERTICAL; g->palavras[12].tamanho = 5;
             colocarPalavra(g, &g->palavras[12], "MARTE"); strcpy(g->palavras[12].dica, "Planeta vermelho."); g->numPalavras++;

             g->palavras[13].inicio = (Posicao){3, 10}; g->palavras[13].direcao = DIRECAO_HORIZONTAL; g->palavras[13].tamanho = 5;
             colocarPalavra(g, &g->palavras[13], "VENUS"); strcpy(g->palavras[13].dica, "Planeta vizinho."); g->numPalavras++;
             
             g->palavras[14].inicio = (Posicao){10, 0}; g->palavras[14].direcao = DIRECAO_VERTICAL; g->palavras[14].tamanho = 5;
             colocarPalavra(g, &g->palavras[14], "ROCHA"); strcpy(g->palavras[14].dica, "Material de asteroides."); g->numPalavras++;

             g->palavras[15].inicio = (Posicao){14, 8}; g->palavras[15].direcao = DIRECAO_HORIZONTAL; g->palavras[15].tamanho = 5;
             colocarPalavra(g, &g->palavras[15], "TERRA"); strcpy(g->palavras[15].dica, "Nosso planeta."); g->numPalavras++;
             
             g->palavras[16].inicio = (Posicao){5, 0}; g->palavras[16].direcao = DIRECAO_HORIZONTAL; g->palavras[16].tamanho = 3;
             colocarPalavra(g, &g->palavras[16], "LUA"); strcpy(g->palavras[16].dica, "Satelite natural."); g->numPalavras++;

             g->palavras[17].inicio = (Posicao){5, 12}; g->palavras[17].direcao = DIRECAO_VERTICAL; g->palavras[17].tamanho = 3;
             colocarPalavra(g, &g->palavras[17], "SOL"); strcpy(g->palavras[17].dica, "Estrela do sistema."); g->numPalavras++;
        }
    } else {
        // EN Layout - 3 Levels
        if (level == 1) {
            // LEVEL 1: (12 Words)
            g->palavras[0].inicio = (Posicao){4, 2}; g->palavras[0].direcao = DIRECAO_HORIZONTAL; g->palavras[0].tamanho = 4;
            colocarPalavra(g, &g->palavras[0], "TREE"); strcpy(g->palavras[0].dica, "Tall plant with trunk and leaves."); g->numPalavras++;

            g->palavras[1].inicio = (Posicao){4, 2}; g->palavras[1].direcao = DIRECAO_VERTICAL; g->palavras[1].tamanho = 4;
            colocarPalavra(g, &g->palavras[1], "TIME"); strcpy(g->palavras[1].dica, "Measured in seconds."); g->numPalavras++;

            g->palavras[2].inicio = (Posicao){4, 5}; g->palavras[2].direcao = DIRECAO_VERTICAL; g->palavras[2].tamanho = 5;
            colocarPalavra(g, &g->palavras[2], "RIVER"); strcpy(g->palavras[2].dica, "Flowing water."); g->numPalavras++;

            g->palavras[3].inicio = (Posicao){7, 5}; g->palavras[3].direcao = DIRECAO_HORIZONTAL; g->palavras[3].tamanho = 4;
            colocarPalavra(g, &g->palavras[3], "RAIN"); strcpy(g->palavras[3].dica, "Water from sky."); g->numPalavras++;

            g->palavras[4].inicio = (Posicao){6, 8}; g->palavras[4].direcao = DIRECAO_VERTICAL; g->palavras[4].tamanho = 3;
            colocarPalavra(g, &g->palavras[4], "SUN"); strcpy(g->palavras[4].dica, "Star of our system."); g->numPalavras++;

            g->palavras[5].inicio = (Posicao){5, 7}; g->palavras[5].direcao = DIRECAO_HORIZONTAL; g->palavras[5].tamanho = 4;
            colocarPalavra(g, &g->palavras[5], "SNOW"); strcpy(g->palavras[5].dica, "Frozen rain."); g->numPalavras++;

            g->palavras[6].inicio = (Posicao){2, 1}; g->palavras[6].direcao = DIRECAO_HORIZONTAL; g->palavras[6].tamanho = 3;
            colocarPalavra(g, &g->palavras[6], "SEA"); strcpy(g->palavras[6].dica, "Large body of salt water."); g->numPalavras++;

            g->palavras[7].inicio = (Posicao){1, 3}; g->palavras[7].direcao = DIRECAO_VERTICAL; g->palavras[7].tamanho = 4;
            colocarPalavra(g, &g->palavras[7], "BIRD"); strcpy(g->palavras[7].dica, "Flying animal."); g->numPalavras++;

            g->palavras[8].inicio = (Posicao){9, 2}; g->palavras[8].direcao = DIRECAO_HORIZONTAL; g->palavras[8].tamanho = 4;
            colocarPalavra(g, &g->palavras[8], "WIND"); strcpy(g->palavras[8].dica, "Moving air."); g->numPalavras++;

            g->palavras[9].inicio = (Posicao){9, 5}; g->palavras[9].direcao = DIRECAO_VERTICAL; g->palavras[9].tamanho = 4;
            colocarPalavra(g, &g->palavras[9], "DUST"); strcpy(g->palavras[9].dica, "Fine powder of dirt."); g->numPalavras++;

            g->palavras[10].inicio = (Posicao){12, 1}; g->palavras[10].direcao = DIRECAO_HORIZONTAL; g->palavras[10].tamanho = 4;
            colocarPalavra(g, &g->palavras[10], "ROCK"); strcpy(g->palavras[10].dica, "Hard mineral material."); g->numPalavras++;
            
            g->palavras[11].inicio = (Posicao){12, 4}; g->palavras[11].direcao = DIRECAO_VERTICAL; g->palavras[11].tamanho = 3;
            colocarPalavra(g, &g->palavras[11], "KEY"); strcpy(g->palavras[11].dica, "Used to open locks."); g->numPalavras++; // Filler to reach 12

        } else if (level == 2) {
            // LEVEL 2: (16 Words)
            g->palavras[0].inicio = (Posicao){5, 5}; g->palavras[0].direcao = DIRECAO_HORIZONTAL; g->palavras[0].tamanho = 4;
            colocarPalavra(g, &g->palavras[0], "CITY"); strcpy(g->palavras[0].dica, "Large town."); g->numPalavras++;

            g->palavras[1].inicio = (Posicao){3, 8}; g->palavras[1].direcao = DIRECAO_VERTICAL; g->palavras[1].tamanho = 4;
            colocarPalavra(g, &g->palavras[1], "TOWN"); strcpy(g->palavras[1].dica, "Small city."); g->numPalavras++;

            g->palavras[2].inicio = (Posicao){5, 5}; g->palavras[2].direcao = DIRECAO_VERTICAL; g->palavras[2].tamanho = 3;
            colocarPalavra(g, &g->palavras[2], "CAR"); strcpy(g->palavras[2].dica, "Motor vehicle."); g->numPalavras++;

            g->palavras[3].inicio = (Posicao){7, 5}; g->palavras[3].direcao = DIRECAO_HORIZONTAL; g->palavras[3].tamanho = 4;
            colocarPalavra(g, &g->palavras[3], "ROAD"); strcpy(g->palavras[3].dica, "Street for cars."); g->numPalavras++;

            g->palavras[4].inicio = (Posicao){8, 3}; g->palavras[4].direcao = DIRECAO_HORIZONTAL; g->palavras[4].tamanho = 3;
            colocarPalavra(g, &g->palavras[4], "BUS"); strcpy(g->palavras[4].dica, "Public transport."); g->numPalavras++;

            g->palavras[5].inicio = (Posicao){2, 10}; g->palavras[5].direcao = DIRECAO_VERTICAL; g->palavras[5].tamanho = 6;
            colocarPalavra(g, &g->palavras[5], "STREET"); strcpy(g->palavras[5].dica, "Public road in a city."); g->numPalavras++;
            
            g->palavras[6].inicio = (Posicao){2, 10}; g->palavras[6].direcao = DIRECAO_HORIZONTAL; g->palavras[6].tamanho = 4;
            colocarPalavra(g, &g->palavras[6], "SHOP"); strcpy(g->palavras[6].dica, "Place to buy things."); g->numPalavras++;

            g->palavras[7].inicio = (Posicao){10, 2}; g->palavras[7].direcao = DIRECAO_HORIZONTAL; g->palavras[7].tamanho = 4;
            colocarPalavra(g, &g->palavras[7], "PARK"); strcpy(g->palavras[7].dica, "Green public area."); g->numPalavras++;

            g->palavras[8].inicio = (Posicao){10, 5}; g->palavras[8].direcao = DIRECAO_VERTICAL; g->palavras[8].tamanho = 4;
            colocarPalavra(g, &g->palavras[8], "BANK"); strcpy(g->palavras[8].dica, "Financial institution."); g->numPalavras++;

            g->palavras[9].inicio = (Posicao){12, 1}; g->palavras[9].direcao = DIRECAO_HORIZONTAL; g->palavras[9].tamanho = 4;
            colocarPalavra(g, &g->palavras[9], "WORK"); strcpy(g->palavras[9].dica, "Job or task."); g->numPalavras++;

            g->palavras[10].inicio = (Posicao){0, 0}; g->palavras[10].direcao = DIRECAO_HORIZONTAL; g->palavras[10].tamanho = 4;
            colocarPalavra(g, &g->palavras[10], "HOME"); strcpy(g->palavras[10].dica, "Where you live."); g->numPalavras++;

            g->palavras[11].inicio = (Posicao){0, 3}; g->palavras[11].direcao = DIRECAO_VERTICAL; g->palavras[11].tamanho = 4;
            colocarPalavra(g, &g->palavras[11], "EXIT"); strcpy(g->palavras[11].dica, "Way out."); g->numPalavras++;
            
            g->palavras[12].inicio = (Posicao){3, 0}; g->palavras[12].direcao = DIRECAO_VERTICAL; g->palavras[12].tamanho = 4;
            colocarPalavra(g, &g->palavras[12], "TAXI"); strcpy(g->palavras[12].dica, "Hired car."); g->numPalavras++;

            g->palavras[13].inicio = (Posicao){14, 8}; g->palavras[13].direcao = DIRECAO_HORIZONTAL; g->palavras[13].tamanho = 5;
            colocarPalavra(g, &g->palavras[13], "HOTEL"); strcpy(g->palavras[13].dica, "Place to stay."); g->numPalavras++;

            g->palavras[14].inicio = (Posicao){0, 14}; g->palavras[14].direcao = DIRECAO_HORIZONTAL; g->palavras[14].tamanho = 3;
            colocarPalavra(g, &g->palavras[14], "MAP"); strcpy(g->palavras[14].dica, "Guide to locations."); g->numPalavras++;
            
            g->palavras[15].inicio = (Posicao){1, 12}; g->palavras[15].direcao = DIRECAO_HORIZONTAL; g->palavras[15].tamanho = 3;
            colocarPalavra(g, &g->palavras[15], "ZOO"); strcpy(g->palavras[15].dica, "Place with animals."); g->numPalavras++;

        } else if (level == 3) {
            // LEVEL 3: (18 Words)
            g->palavras[0].inicio = (Posicao){7, 2}; g->palavras[0].direcao = DIRECAO_HORIZONTAL; g->palavras[0].tamanho = 8;
            colocarPalavra(g, &g->palavras[0], "UNIVERSE"); strcpy(g->palavras[0].dica, "All existence."); g->numPalavras++;

            g->palavras[1].inicio = (Posicao){7, 2}; g->palavras[1].direcao = DIRECAO_VERTICAL; g->palavras[1].tamanho = 3; // U..
            colocarPalavra(g, &g->palavras[1], "UFO"); strcpy(g->palavras[1].dica, "Flying object."); g->numPalavras++;

            g->palavras[2].inicio = (Posicao){5, 4}; g->palavras[2].direcao = DIRECAO_VERTICAL; g->palavras[2].tamanho = 6; // ..I..
            colocarPalavra(g, &g->palavras[2], "ORBITA"); strcpy(g->palavras[2].dica, "Path around planet."); g->numPalavras++; // wait, ORBIT in EN
            g->palavras[2].inicio = (Posicao){5, 4}; g->palavras[2].direcao = DIRECAO_VERTICAL; g->palavras[2].tamanho = 5;
            colocarPalavra(g, &g->palavras[2], "ORBIT"); strcpy(g->palavras[2].dica, "Path around planet."); 

            g->palavras[3].inicio = (Posicao){7, 5}; g->palavras[3].direcao = DIRECAO_VERTICAL; g->palavras[3].tamanho = 6; // E..
            colocarPalavra(g, &g->palavras[3], "ENERGY"); strcpy(g->palavras[3].dica, "Power/Capacity."); g->numPalavras++;

            g->palavras[4].inicio = (Posicao){7, 7}; g->palavras[4].direcao = DIRECAO_VERTICAL; g->palavras[4].tamanho = 4; // S..
            colocarPalavra(g, &g->palavras[4], "STAR"); strcpy(g->palavras[4].dica, "Burning gas ball."); g->numPalavras++;

            g->palavras[5].inicio = (Posicao){7, 9}; g->palavras[5].direcao = DIRECAO_VERTICAL; g->palavras[5].tamanho = 5; // E.. EARTH?
            colocarPalavra(g, &g->palavras[5], "EARTH"); strcpy(g->palavras[5].dica, "Our home."); g->numPalavras++;

            g->palavras[6].inicio = (Posicao){3, 0}; g->palavras[6].direcao = DIRECAO_HORIZONTAL; g->palavras[6].tamanho = 6;
            colocarPalavra(g, &g->palavras[6], "GALAXY"); strcpy(g->palavras[6].dica, "System of stars."); g->numPalavras++;
            
            g->palavras[7].inicio = (Posicao){10, 0}; g->palavras[7].direcao = DIRECAO_HORIZONTAL; g->palavras[7].tamanho = 4;
            colocarPalavra(g, &g->palavras[7], "MARS"); strcpy(g->palavras[7].dica, "Red planet."); g->numPalavras++;

            g->palavras[8].inicio = (Posicao){0, 5}; g->palavras[8].direcao = DIRECAO_HORIZONTAL; g->palavras[8].tamanho = 5;
            colocarPalavra(g, &g->palavras[8], "COMET"); strcpy(g->palavras[8].dica, "Icy space body."); g->numPalavras++;

            g->palavras[9].inicio = (Posicao){0, 7}; g->palavras[9].direcao = DIRECAO_VERTICAL; g->palavras[9].tamanho = 4; // T..
            colocarPalavra(g, &g->palavras[9], "TIME"); strcpy(g->palavras[9].dica, "The 4th dimension."); g->numPalavras++;

            g->palavras[10].inicio = (Posicao){0, 10}; g->palavras[10].direcao = DIRECAO_HORIZONTAL; g->palavras[10].tamanho = 5;
            colocarPalavra(g, &g->palavras[10], "VENUS"); strcpy(g->palavras[10].dica, "Hot planet."); g->numPalavras++;

            g->palavras[11].inicio = (Posicao){0, 10}; g->palavras[11].direcao = DIRECAO_VERTICAL; g->palavras[11].tamanho = 4; // V..
            colocarPalavra(g, &g->palavras[11], "VOID"); strcpy(g->palavras[11].dica, "Empty space."); g->numPalavras++;
            
            g->palavras[12].inicio = (Posicao){12, 10}; g->palavras[12].direcao = DIRECAO_HORIZONTAL; g->palavras[12].tamanho = 4;
            colocarPalavra(g, &g->palavras[12], "ATOM"); strcpy(g->palavras[12].dica, "Basic unit of matter."); g->numPalavras++;
            
            g->palavras[13].inicio = (Posicao){14, 0}; g->palavras[13].direcao = DIRECAO_HORIZONTAL; g->palavras[13].tamanho = 4;
            colocarPalavra(g, &g->palavras[13], "MOON"); strcpy(g->palavras[13].dica, "Earth's satellite."); g->numPalavras++;
            
            g->palavras[14].inicio = (Posicao){4, 13}; g->palavras[14].direcao = DIRECAO_HORIZONTAL; g->palavras[14].tamanho = 5;
            colocarPalavra(g, &g->palavras[14], "LIGHT"); strcpy(g->palavras[14].dica, "Fastest thing."); g->numPalavras++;
            
            g->palavras[15].inicio = (Posicao){2, 12}; g->palavras[15].direcao = DIRECAO_VERTICAL; g->palavras[15].tamanho = 6;
            colocarPalavra(g, &g->palavras[15], "NEBULA"); strcpy(g->palavras[15].dica, "Star nursery."); g->numPalavras++;

            g->palavras[16].inicio = (Posicao){10, 8}; g->palavras[16].direcao = DIRECAO_HORIZONTAL; g->palavras[16].tamanho = 6; // R..
            colocarPalavra(g, &g->palavras[16], "ROCKET"); strcpy(g->palavras[16].dica, "Space vehicle."); g->numPalavras++;
            
            g->palavras[17].inicio = (Posicao){12, 6}; g->palavras[17].direcao = DIRECAO_HORIZONTAL; g->palavras[17].tamanho = 5; // ..ACE
            colocarPalavra(g, &g->palavras[17], "SPACE"); strcpy(g->palavras[17].dica, "The final frontier."); g->numPalavras++;
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
    
    InitWindow(1024, 768, "Palavras Cruzadas - Expanded");
    SetTargetFPS(60);
    
    InitInterface();
    
    Grid g_val;
    Grid* g = &g_val;
    inicializarGrid(g, 15, 15);
    
    EstadoJogo estado;
    int palavrasCorretas = 0;
    char statusMsg[64] = "";
    bool gameOver = false;
    
    // Init state
    currentLevel = 1;
    grandTotalTime = 0;
    grandTotalErrors = 0;
    strcpy(globalLanguage, "PT"); // Default
    
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
                    LoadLevel(1, g); 
                    cenaAtual = CENA_JOGO; gameOver = false; palavrasCorretas = 0;
                }
                DrawText("Ingles", GetScreenWidth()/2 - 100, 360, 20, UI_COLOR_TEXT);

                if (GuiButton((Rectangle){GetScreenWidth()/2 + 20, 250, 100, 100}, "PT")) {
                    strcpy(idiomaAtual, "PT");
                    strcpy(globalLanguage, "PT");
                    LoadLevel(1, g); 
                    cenaAtual = CENA_JOGO; gameOver = false; palavrasCorretas = 0;
                }
                DrawText("Portugues", GetScreenWidth()/2 + 25, 360, 20, UI_COLOR_TEXT);
                
                if (GuiButton((Rectangle){GetScreenWidth()/2 - 100, 500, 200, 50}, "VOLTAR")) { cenaAtual = CENA_MENU; }
                break;
                
            case CENA_JOGO:
                if (gameOver) {
                    // TELA DE VITORIA
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(UI_COLOR_BG, 0.9f));
                    
                    if (currentLevel < maxLevels) {
                         // Tela de proximo nivel
                         DrawTextCentered("NIVEL CONCLUIDO!", GetScreenWidth()/2, 200, 40, UI_COLOR_PRIMARY);
                         
                         char scoreStr[64];
                         snprintf(scoreStr, 64, "Tempo: %.0fs  |  Erros: %d", GetTime() - startTime, gameErrors);
                         DrawTextCentered(scoreStr, GetScreenWidth()/2, 300, 30, UI_COLOR_TEXT);
                         
                         if (GuiButton((Rectangle){GetScreenWidth()/2 - 150, 400, 300, 50}, "PROXIMO NIVEL")) {
                             grandTotalTime += (GetTime() - startTime);
                             grandTotalErrors += gameErrors;
                             currentLevel++;
                             LoadLevel(currentLevel, g);
                             gameOver = false; palavrasCorretas = 0;
                         }
                    } else {
                         // VITORIA FINAL
                         DrawTextCentered("PARABENS! VOCE VENCEU!", GetScreenWidth()/2, 100, 40, UI_COLOR_PRIMARY);
                         
                         char totalStr[100];
                         snprintf(totalStr, 100, "Tempo Total: %.0fs | Erros Totais: %d", grandTotalTime + (GetTime()-startTime), grandTotalErrors + gameErrors);
                         DrawTextCentered(totalStr, GetScreenWidth()/2, 300, 30, UI_COLOR_TEXT);
                         
                         if (GuiButton((Rectangle){GetScreenWidth()/2 - 150, 450, 300, 50}, "MENU PRINCIPAL")) {
                             cenaAtual = CENA_MENU;
                             currentLevel = 1; grandTotalTime=0; grandTotalErrors=0;
                         }
                    }
                } else {
                    UpdateInterface(g, &estado);
                    
                    // Header
                    DrawText("Palavras Cruzadas", 40, 20, 30, UI_COLOR_PRIMARY);
                    char infoStr[64];
                    snprintf(infoStr, 64, "Idioma: %s - Nivel %d | Acertos: %d/%d", idiomaAtual, currentLevel, palavrasCorretas, g->numPalavras);
                    DrawText(infoStr, 40, 55, 20, DARKGRAY);
                    
                    // Grid
                    DrawCrosswordGrid(g, 50, 100, 32); 
                    
                    // Dicas
                    int colDicas = 550;
                    int startY = 100;
                    
                    // Horizontal
                    DrawText("HORIZONTAIS", colDicas, startY, 20, UI_COLOR_PRIMARY);
                    DrawLine(colDicas, startY + 25, 950, startY + 25, UI_COLOR_PRIMARY);
                    startY += 30;
                    
                    for(int i=0; i<g->numPalavras; i++) {
                        if (g->palavras[i].direcao == DIRECAO_HORIZONTAL) {
                             char clueLine[100];
                             snprintf(clueLine, 100, "%d. %s", g->celulas[g->palavras[i].inicio.linha][g->palavras[i].inicio.coluna].numero, g->palavras[i].dica);
                             if (startY < 380) { 
                                 DrawText(clueLine, colDicas, startY, 14, UI_COLOR_TEXT);
                                 startY += 18;
                             }
                        }
                    }

                    startY = 400;
                    DrawText("VERTICAIS", colDicas, startY, 20, UI_COLOR_PRIMARY);
                    DrawLine(colDicas, startY + 25, 950, startY + 25, UI_COLOR_PRIMARY);
                    startY += 30;

                     for(int i=0; i<g->numPalavras; i++) {
                        if (g->palavras[i].direcao == DIRECAO_VERTICAL) {
                             char clueLine[100];
                             snprintf(clueLine, 100, "%d. %s", g->celulas[g->palavras[i].inicio.linha][g->palavras[i].inicio.coluna].numero, g->palavras[i].dica);
                             if (startY < 640) {
                                 DrawText(clueLine, colDicas, startY, 14, UI_COLOR_TEXT);
                                 startY += 18;
                             }
                        }
                    }

                    // Botão Verificar
                    if (GuiButton((Rectangle){ (float)colDicas, 650, 150, 40}, "VERIFICAR")) {
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
                    
                    DrawText(statusMsg, colDicas + 170, 660, 20, UI_COLOR_CORRECT);
                    
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
