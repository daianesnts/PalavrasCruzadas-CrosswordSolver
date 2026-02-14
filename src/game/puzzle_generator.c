#include "puzzle_generator.h"
#include "solver.h"
#include "../data/grid.h"
#include "../data/dictionary.h" // Necessário para busca de palavras aleatorias
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

static bool seeded = false;

// Session Word Tracking
#define MAX_SESSION_WORDS 500
static char sessionUsedWords[MAX_SESSION_WORDS][TAMANHO_MAX_PALAVRA + 1];
static int sessionUsedCount = 0;

void Generator_Init(void) {
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
}

void Generator_ResetSession(void) {
    sessionUsedCount = 0;
}

static bool IsWordUsedInSession(const char* word) {
    for (int i = 0; i < sessionUsedCount; i++) {
        if (strcmp(sessionUsedWords[i], word) == 0) return true;
    }
    return false;
}

static void MarkWordUsedInSession(const char* word) {
    if (sessionUsedCount < MAX_SESSION_WORDS) {
        strcpy(sessionUsedWords[sessionUsedCount++], word);
    }
}

// --- FUNCOES AUXILIARES ---

// Verifica se a colocacao de uma palavra valida (limites e vizinhos)
// Retorna true se valida.
static bool IsPlacementValid(Grid* g, int row, int col, int len, Direcao dir, const char* word) {
    int r = row;
    int c = col;
    
    // 1. Limites Basicos
    if (r < 0 || c < 0) return false;
    if (dir == DIRECAO_HORIZONTAL) {
        if (c + len > g->colunas) return false;
    } else {
        if (r + len > g->linhas) return false;
    }

    // 2. Colisao e Vizinhos
    // Iteramos sobre as celulas da nova palavra
    for (int i = 0; i < len; i++) {
        // Posicao atual da celula
        int curR = (dir == DIRECAO_HORIZONTAL) ? r : r + i;
        int curC = (dir == DIRECAO_HORIZONTAL) ? c + i : c;
        char letter = word[i];

        // Verifica conteudo desta celula
        Celula* cell = &g->celulas[curR][curC];
        
        // Se a celula ja esta preenchida
        if (cell->tipo == CELULA_PREENCHIDA) {
            // Deve coincidir a letra exata
            if (cell->letra != letter) return false;
        } 
        else if (cell->tipo == CELULA_BLOQUEADA || cell->tipo == CELULA_VAZIA) {
             // Se vazia/bloqueada, devemos garantir que nao estamos tocando outras palavras "de lado" 
             // (criando nao-palavras de 2 letras), A MENOS que seja o ponto de cruzamento.
             // Apenas permitir colocacao se a celula for VAZIA/BLOQUEADA.
        }
    }
    
    // 3. Verifica Limites da Palavra (Inicio/Fim)
    // Celula "antes" da palavra deve ser vazia/bloqueada/limite
    int preR = (dir == DIRECAO_HORIZONTAL) ? r : r - 1;
    int preC = (dir == DIRECAO_HORIZONTAL) ? c - 1 : c;
    if (preR >= 0 && preC >= 0 && preR < g->linhas && preC < g->colunas) {
        if (g->celulas[preR][preC].tipo == CELULA_PREENCHIDA) return false;
    }
    
    // Celula "depois" da palavra deve ser vazia/bloqueada/limite
    int postR = (dir == DIRECAO_HORIZONTAL) ? r : r + len;
    int postC = (dir == DIRECAO_HORIZONTAL) ? c + len : c;
    if (postR < g->linhas && postC < g->colunas) {
        if (g->celulas[postR][postC].tipo == CELULA_PREENCHIDA) return false;
    }

    // 4. Checagem "Paralela" (evitar palavras lado a lado)
    // Iterar novamente, verificando estritamente vizinhos perpendiculares
    for (int i = 0; i < len; i++) {
        int curR = (dir == DIRECAO_HORIZONTAL) ? r : r + i;
        int curC = (dir == DIRECAO_HORIZONTAL) ? c + i : c;
        
        // Se esta celula ja estava preenchida, e um cruzamento, entao vizinhos sao OK (pertencem a palavra cruzada)
        if (g->celulas[curR][curC].tipo == CELULA_PREENCHIDA) continue; 

        // Verifica Vizinhos Perpendiculares
        int n1R = curR + ((dir == DIRECAO_HORIZONTAL) ? -1 : 0);
        int n1C = curC + ((dir == DIRECAO_HORIZONTAL) ? 0 : -1);
        int n2R = curR + ((dir == DIRECAO_HORIZONTAL) ? 1 : 0);
        int n2C = curC + ((dir == DIRECAO_HORIZONTAL) ? 0 : 1);

        if (n1R >= 0 && n1C >= 0 && n1R < g->linhas && n1C < g->colunas) {
            if (g->celulas[n1R][n1C].tipo == CELULA_PREENCHIDA) return false;
        }
        if (n2R >= 0 && n2C >= 0 && n2R < g->linhas && n2C < g->colunas) {
            if (g->celulas[n2R][n2C].tipo == CELULA_PREENCHIDA) return false;
        }
    }

    return true;
}


// Retorna true se conseguiu preencher ate o alvo
static bool BacktrackGenerate(Grid* grid, int targetWords) {
    // Base Case: Alvo atingido
    if (grid->numPalavras >= targetWords) return true;

    // Tenta encontrar uma nova palavra para colocar
    // Estrategia: Iterar sobre palavras ja colocadas, escolher uma interseccao, tentar palavras do dicionario
    
    // Para garantir variedade, criamos indices aleatorios para iterar
    int* wordIndices = (int*)malloc(grid->numPalavras * sizeof(int));
    for(int i=0; i<grid->numPalavras; i++) wordIndices[i] = i;
    // Shuffle indices
    for(int i=grid->numPalavras-1; i>0; i--) {
        int j = rand() % (i+1);
        int temp = wordIndices[i];
        wordIndices[i] = wordIndices[j];
        wordIndices[j] = temp;
    }

    for (int idx = 0; idx < grid->numPalavras; idx++) {
        int i = wordIndices[idx];
        Palavra* srcP = &grid->palavras[i];
        
        // Tenta todas as posicoes desta palavra para cruzar
        int len = srcP->tamanho;
        int* charIndices = (int*)malloc(len * sizeof(int));
        for(int k=0; k<len; k++) charIndices[k] = k;
        // Embaralha char indices
        for(int k=len-1; k>0; k--) {
            int j = rand() % (k+1);
            int temp = charIndices[k];
            charIndices[k] = charIndices[j];
            charIndices[j] = temp;
        }

        for(int cIdx = 0; cIdx < len; cIdx++) {
            int interOffset = charIndices[cIdx];
            char pivotChar = srcP->resposta[interOffset];
            int pivotR = srcP->inicio.linha + ((srcP->direcao == DIRECAO_VERTICAL) ? interOffset : 0);
            int pivotC = srcP->inicio.coluna + ((srcP->direcao == DIRECAO_HORIZONTAL) ? interOffset : 0);
            
            Direcao newDir = (srcP->direcao == DIRECAO_HORIZONTAL) ? DIRECAO_VERTICAL : DIRECAO_HORIZONTAL;
            
            // Tentar tamanhos variados
            // Embaralha tamanhos 4-7
            int sizes[] = {4, 5, 6, 7};
            // Embaralha tamanhos
             for(int s=3; s>0; s--) {
                int j = rand() % (s+1);
                int temp = sizes[s];
                sizes[s] = sizes[j];
                sizes[j] = temp;
            }

            for(int szIdx=0; szIdx<4; szIdx++) {
                int newLen = sizes[szIdx];
                
                char** searchList = NULL;
                int count = 0;
                dict_search_by_size(newLen, &searchList, &count);
                
                if (count > 0) {
                     // Tentar candidatos (limite para nao demorar muito no backtrack)
                     // Shuffle candidates ou pegar random subset
                     int tries = (count > 15) ? 15 : count;
                     int* candIndices = (int*)malloc(count * sizeof(int));
                     for(int ci=0; ci<count; ci++) candIndices[ci] = ci;
                    for(int ci=0; ci<tries; ci++) {
                        int rnd = rand() % count; // Simples escolha aleatória é melhor para velocidade do que uma embaralhada completa em uma lista grande
                        int temp = candIndices[ci];
                        candIndices[ci] = candIndices[rnd];
                        candIndices[rnd] = temp;
                    }
                     
                     for(int attempt=0; attempt<tries; attempt++) {
                         char* cand = searchList[candIndices[attempt]];
                         
                         // Verifica se cand tem pivotChar
                         for(int j=0; j<newLen; j++) {
                             if (cand[j] == pivotChar) {
                                  // Candidato match!
                                  int startR = pivotR - ((newDir == DIRECAO_VERTICAL) ? j : 0);
                                  int startC = pivotC - ((newDir == DIRECAO_HORIZONTAL) ? j : 0);
                                  
                                  // Check Dup in Grid OR Session
                                  bool dup = false;
                                  
                                  // Check Grid
                                  for(int w=0; w<grid->numPalavras; w++) {
                                      if (strcmp(grid->palavras[w].resposta, cand) == 0) dup = true;
                                  }
                                  
                                  // Check Session (Global)
                                  if (!dup && IsWordUsedInSession(cand)) dup = true;
                                  
                                  if (dup) continue;

                                  if (IsPlacementValid(grid, startR, startC, newLen, newDir, cand)) {
                                      // FAZ: coloca palavra
                                      int currentIdx = grid->numPalavras;
                                      Palavra* newP = &grid->palavras[currentIdx];
                                      newP->direcao = newDir;
                                      newP->tamanho = newLen;
                                      newP->inicio.linha = startR;
                                      newP->inicio.coluna = startC;
                                      strcpy(newP->resposta, cand);
                                      
                                      // Preenche celulas
                                       for(int k=0; k<newLen; k++) {
                                         int rr = (newDir==DIRECAO_VERTICAL) ? startR+k : startR;
                                         int cc = (newDir==DIRECAO_HORIZONTAL) ? startC+k : startC;
                                         grid->celulas[rr][cc].letra = cand[k];
                                         grid->celulas[rr][cc].tipo = CELULA_PREENCHIDA;
                                      }
                                      grid->numPalavras++;

                                      // RECURSIVO
                                      if (BacktrackGenerate(grid, targetWords)) {
                                          free(candIndices);
                                          if (searchList) { for(int xx=0; xx<count; xx++) free(searchList[xx]); free(searchList); }
                                          free(charIndices);
                                          free(wordIndices);
                                          return true;
                                      }
                                      
                                      // DESFAZ (Backtrack)
                                      grid->numPalavras--;
                                      // Restaurar celulas... Cuidado!
                                      // Se removermos cells, podemos apagar interseccoes de outras palavras?
                                      // NAO, porque so adicionamos. Mas precisamos restaurar o estado ANTERIOR.
                                      // Celulas que ERAM vazias voltam a ser vazias/bloqueadas.
                                      // Celulas que ERAM preenchidas (cruzamentos) devem manter.
                                      // IsPlacementValid garante que so tocou em VAZIA, BLOQUEADA ou PREENCHIDA-IGUAL.
                                      // Entao:
                                      for(int k=0; k<newLen; k++) {
                                         int rr = (newDir==DIRECAO_VERTICAL) ? startR+k : startR;
                                         int cc = (newDir==DIRECAO_HORIZONTAL) ? startC+k : startC;
                                         
                                         // Verificar se pertence a OUTRA palavra antes de limpar
                                         bool belongsToOther = false;
                                         for(int ow=0; ow<grid->numPalavras; ow++) {
                                             Palavra* other = &grid->palavras[ow];
                                             // Verifica se (rr,cc) esta em 'other'
                                             bool inOther = false;
                                             if (other->direcao == DIRECAO_HORIZONTAL) {
                                                 if (rr == other->inicio.linha && cc >= other->inicio.coluna && cc < other->inicio.coluna + other->tamanho) inOther = true;
                                             } else {
                                                  if (cc == other->inicio.coluna && rr >= other->inicio.linha && rr < other->inicio.linha + other->tamanho) inOther = true;
                                             }
                                             if (inOther) { belongsToOther = true; break; }
                                         }
                                         
                                         if (!belongsToOther) {
                                             grid->celulas[rr][cc].tipo = CELULA_BLOQUEADA; // Default do Init
                                             grid->celulas[rr][cc].letra = '\0';
                                         }
                                      }
                                  }
                             }
                         }
                     }
                     free(candIndices);
                }
                if (searchList) {
                    for(int i=0; i<count; i++) free(searchList[i]);
                    free(searchList);
                }
            }
        }
        free(charIndices);
    }
    free(wordIndices);
    
    return false; // Falhou em ramificar daqui
}

bool Generator_GenerateLevel(Grid* grid, int levelIndex) {
    if (!grid) return false;
    Generator_Init();

    // 1. Resetar Grid (Tudo Bloqueado/Azul)
    inicializarGrid(grid, TAMANHO_MAX_GRID, TAMANHO_MAX_GRID);
    grid->numPalavras = 0;

    // 2. Determinar Dificuldade
    int targetWords = 6 + (levelIndex * 2); 
    if (targetWords > MAX_PALAVRAS) targetWords = MAX_PALAVRAS;
    
    // 3. Colocar Primeira Palavra (Base da Arvore)
    int len = 5 + (rand() % 4);
    char** candidates = NULL;
    int numCandidates = 0;
    dict_search_by_size(len, &candidates, &numCandidates);
    
    if (numCandidates > 0) {
        char* startWord = candidates[rand() % numCandidates];
        
        // Ensure start word is unique too!
        // But hard to retry here without loop. If collision, just accept for start or pick another?
        // Let's try up to 10 times for a unique start word
        for(int try=0; try<10; try++) {
            if (!IsWordUsedInSession(startWord)) break;
            startWord = candidates[rand() % numCandidates];
        }
        
        Palavra* p = &grid->palavras[grid->numPalavras];
        p->direcao = DIRECAO_HORIZONTAL;
        p->tamanho = len;
        p->inicio.linha = 7; 
        p->inicio.coluna = 7 - (len/2); 
        strcpy(p->resposta, startWord);
        strcpy(p->textoAtual, "");
        p->estaCompleta = false;
        
         for(int k=0; k<len; k++) {
            grid->celulas[p->inicio.linha][p->inicio.coluna + k].letra = p->resposta[k];
            grid->celulas[p->inicio.linha][p->inicio.coluna + k].tipo = CELULA_PREENCHIDA;
        }
        grid->numPalavras++;
    
        // 4. Iniciar Backtracking
        if (!BacktrackGenerate(grid, targetWords)) {
            // Se falhou, tenta pelo menos retornar o que conseguiu se for "decente"
            if (grid->numPalavras < 3) {
                 // Vamos aceitar para nao travar.
            }
        }
    }
    
    // Limpeza Dict
     if (candidates) {
        for(int i=0; i<numCandidates; i++) free(candidates[i]);
        free(candidates);
    }

    // 5. Finalizar (Converter para Jogavel) e Marcar como Usadas
    // Preencher Dicas e limpar celulas para jogabilidade
    for(int i=0; i<grid->numPalavras; i++) {
        MarkWordUsedInSession(grid->palavras[i].resposta); // Mark as used
        
        dict_check_word(grid->palavras[i].resposta, grid->palavras[i].dica, TAMANHO_MAX_DICA);
        if (strlen(grid->palavras[i].dica) == 0) strcpy(grid->palavras[i].dica, "Sem Dica");
         grid->palavras[i].estaCompleta = false;
         strcpy(grid->palavras[i].textoAtual, "");
         
         // Numeros
         int r = grid->palavras[i].inicio.linha;
         int c = grid->palavras[i].inicio.coluna;
         if(grid->celulas[r][c].numero == 0) grid->celulas[r][c].numero = i+1; // Temporario, main reordena
    }

    for(int r=0; r<grid->linhas; r++) {
        for(int c=0; c<grid->colunas; c++) {
            if (grid->celulas[r][c].tipo == CELULA_PREENCHIDA) {
                grid->celulas[r][c].tipo = CELULA_VAZIA; 
                grid->celulas[r][c].letra = '\0';        
            }
        }
    }
    return true;
}