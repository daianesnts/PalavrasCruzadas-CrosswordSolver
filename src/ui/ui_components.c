#include "ui_components.h"
#include <string.h>
#include <stdio.h>

bool GuiButton(Rectangle rect, const char* text) {
    Vector2 mousePoint = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mousePoint, rect);
    bool isClicked = false;
    bool isPressed = false;

    if (isHover) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            isPressed = true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isClicked = true;
        }
    }

    Color color = isHover ? UI_COLOR_BUTTON_HOVER : UI_COLOR_BUTTON;
    if (isPressed) color = UI_COLOR_PRIMARY;

    Rectangle drawRect = rect;
    if (isPressed) {
        drawRect.y += 2;
    }

    // Sombra
    if (!isPressed) {
        DrawRectangleRounded((Rectangle){rect.x, rect.y + 4, rect.width, rect.height}, 0.2f, 10, Fade(GRAY, 0.5f));
    }

    DrawRectangleRounded(drawRect, 0.2f, 10, color);
    DrawRectangleRoundedLines(drawRect, 0.2f, 10, WHITE);
    
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    
    Color textColor = isPressed ? WHITE : UI_COLOR_TEXT;
    DrawText(text, (int)(drawRect.x + (drawRect.width - textWidth) / 2), (int)(drawRect.y + (drawRect.height - fontSize) / 2), fontSize, textColor);

    return isClicked;
}

void GuiPanel(Rectangle rect) {
    // Sombra
    DrawRectangleRounded((Rectangle){rect.x + 4, rect.y + 4, rect.width, rect.height}, 0.05f, 10, Fade(GRAY, 0.3f));
    // Corpo principal
    DrawRectangleRounded(rect, 0.05f, 10, WHITE);
}

void DrawTextCentered(const char* text, int cx, int cy, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, cx - textWidth / 2, cy - fontSize / 2, fontSize, color);
}

void DrawTextWrapped(const char *text, Rectangle rec, int fontSize, Color color) {
    if (!text || strlen(text) == 0) return;

    int currentLineY = (int)rec.y;
    int currentLineX = (int)rec.x;
    int lineHeight = fontSize + 4; // Adiciona um espaco
    
    // Copia texto para modificar ou passar
    // Nos vamos passar caracter por caracter para lidar com \n e espacos
    const char* ptr = text;
    char wordBuffer[256];
    int wordLen = 0;
    
    char lineBuffer[1024] = "";
    
    while (*ptr != '\0') {
        if (*ptr == '\n') {
            if (strlen(lineBuffer) > 0) {
                 DrawText(lineBuffer, currentLineX, currentLineY, fontSize, color);
            }
            currentLineY += lineHeight;
            strcpy(lineBuffer, ""); // Reseta linha
            ptr++; // Pula \n
            continue;
        }
        
        // Le palavra
        wordLen = 0;
        while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0') {
             wordBuffer[wordLen++] = *ptr++;
        }
        wordBuffer[wordLen] = '\0';
        
        // Checa se palavra encaixa
        const char* space = (strlen(lineBuffer) > 0) ? " " : "";
        int currentW = MeasureText(lineBuffer, fontSize);
        int wordW = MeasureText(wordBuffer, fontSize);
        int spaceW = MeasureText(" ", fontSize);
        
        int totalW = currentW + (strlen(space) > 0 ? spaceW : 0) + wordW;
        
        if (totalW > rec.width) {
            DrawText(lineBuffer, currentLineX, currentLineY, fontSize, color);
            currentLineY += lineHeight;
            strcpy(lineBuffer, wordBuffer);
        } else {
            if (strlen(lineBuffer) > 0) strcat(lineBuffer, " ");
            strcat(lineBuffer, wordBuffer);
        }
        
        if (*ptr == ' ') ptr++; // Pula espaco
    }
    
    // Desenha ultima linha
    if (strlen(lineBuffer) > 0) {
        DrawText(lineBuffer, currentLineX, currentLineY, fontSize, color);
    }
}
