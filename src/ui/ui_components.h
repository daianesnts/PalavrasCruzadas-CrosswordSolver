#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <raylib.h>
#include <stdbool.h>

// CORES - Blue & Yellow Theme
#define UI_COLOR_PRIMARY      (Color){ 70, 160, 240, 255 }   // Azul Claro Ceu (Buttons, Headers)
#define UI_COLOR_SECONDARY    (Color){ 255, 204, 0, 255 }    // Amarelo (Acessado)
#define UI_COLOR_ACCENT       (Color){ 255, 240, 100, 255 }  // Amarelo Claro (Selection)
#define UI_COLOR_BG           (Color){ 245, 250, 255, 255 }  // Azul/Branco Muito Palido (Background)
#define UI_COLOR_TEXT         (Color){ 20, 30, 60, 255 }     // Azul/Preto Escuro (Texto)
#define UI_COLOR_BUTTON       (Color){ 70, 160, 240, 255 }   // Azul Mais Claro Ceu
#define UI_COLOR_BUTTON_HOVER (Color){ 100, 180, 250, 255 }  // Azul Ainda Mais Claro
#define UI_COLOR_GRID_LINE    (Color){ 40, 100, 180, 255 }   // Azul Medio (Linhas)
#define UI_COLOR_BLOCK        (Color){ 70, 160, 240, 255 }   // Azul Mais Claro Ceu (Blocos)
#define UI_COLOR_CORRECT      (Color){ 46, 204, 113, 255 }   // Verde (Correto)

// Desenhe um botão estilizado com cantos arredondados e efeito de foco ao passar o mouse
bool GuiButton(Rectangle rect, const char* text);

// Desenhe um painel com sombra e cantos arredondados
void GuiPanel(Rectangle rect);

// Auxiliar para desenhar texto centralizado
void DrawTextCentered(const char* text, int cx, int cy, int fontSize, Color color);

// Funcao auxiliar para desenhar texto dentro de um retangulo
void DrawTextWrapped(const char *text, Rectangle rec, int fontSize, Color color);

#endif // UI_COMPONENTS_H
