#include "solver.h"
#include "../data/dictionary.h"
#include <stdlib.h>
#include "../data/grid.h"
#include "validation.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FUNÇÕES INTERNAS (PRIVADAS)

static bool backtrack(Grid *grid, int indicePalavra, EstadoSolver *estado);

static void obterCandidatos(int tamanho, char ***candidatos,
                            int *numCandidatos);


// Libera memória alocada para lista de candidatos
static void liberarCandidatos(char **candidatos, int numCandidatos);

// FUNÇÕES PÚBLICAS

bool resolverPuzzle(Grid *grid, EstadoSolver *estado) {
  if (grid == NULL || estado == NULL) {
    return false;
  }

  // Inicializar estado
  inicializarSolver(grid, estado);

  // Executar backtracking a partir da primeira palavra
  bool sucesso = backtrack(grid, 0, estado);

  // Marcar como concluído
  estado->concluido = true;
  estado->encontrouSolucao = sucesso;
  return sucesso;
}

void inicializarSolver(Grid *grid, EstadoSolver *estado) {
  if (estado == NULL) {
    return;
  }

  // Resetar todas as estatísticas
  estado->indicePalavraAtual = 0;
  estado->tentativas = 0;
  estado->retrocessos = 0;
  estado->concluido = false;
  estado->encontrouSolucao = false;
  estado->profundidade = 0;
  estado->ultimaPalavra[0] = '\0';

  // Limpar grid (remover letras não fixas)
  if (grid != NULL) {
    limparGrid(grid);
  }
}

bool proximoPasso(Grid *grid, EstadoSolver *estado) {
  // Esta é uma versão simplificada para demonstração
  // Uma implementação completa precisaria manter pilha de estados
  // Por enquanto, executa a resolução completa

  if (estado->concluido) {
    return false; // Já terminou
  }

  // Executar resolução completa
  // TODO: Implementar versão incremental para animação
  bool sucesso = backtrack(grid, 0, estado);

  estado->concluido = true;
  estado->encontrouSolucao = sucesso;

  return false; // Não há mais passos
}

void obterEstatisticas(EstadoSolver *estado, int *tentativas,
                       int *retrocessos) {
  if (estado == NULL) {
    return;
  }

  if (tentativas != NULL) {
    *tentativas = estado->tentativas;
  }

  if (retrocessos != NULL) {
    *retrocessos = estado->retrocessos;
  }
}

void resetarSolver(EstadoSolver *estado) {
  if (estado == NULL) {
    return;
  }

  estado->indicePalavraAtual = 0;
  estado->tentativas = 0;
  estado->retrocessos = 0;
  estado->concluido = false;
  estado->encontrouSolucao = false;
  estado->profundidade = 0;
  estado->ultimaPalavra[0] = '\0';
}

bool verificarSolucao(Grid *grid) {
  if (grid == NULL) {
    return false;
  }

  // Verificar se todas as palavras estão completas
  for (int i = 0; i < grid->numPalavras; i++) {
    Palavra *palavra = obterPalavra(grid, i);

    if (palavra == NULL || !palavra->estaCompleta) {
      return false;
    }
  }

  // Verificar se o grid está completo (sem células vazias)
  return gridEstaCompleto(grid);
}

// FUNÇÕES INTERNAS

static bool backtrack(Grid *grid, int indicePalavra, EstadoSolver *estado) {
  // Atualizar profundidade
  estado->profundidade = indicePalavra;

  // CASO BASE: Todas as palavras foram preenchidas
  if (indicePalavra >= grid->numPalavras) {
    // Verificar se a solução é válida
    return verificarSolucao(grid);
  }

  // Obter a palavra atual
  Palavra *palavra = obterPalavra(grid, indicePalavra);

  if (palavra == NULL) {
    return false;
  }

  // Se a palavra já está completa, pular para a próxima
  if (palavra->estaCompleta) {
    return backtrack(grid, indicePalavra + 1, estado);
  }

  // Obter candidatos do dicionário com o tamanho correto
  char **candidatos = NULL;
  int numCandidatos = 0;

  obterCandidatos(palavra->tamanho, &candidatos, &numCandidatos);

  // Tentar cada candidato
  for (int i = 0; i < numCandidatos; i++) {
    char *candidato = candidatos[i];

    // Incrementar contador de tentativas
    estado->tentativas++;

    // Copiar para ultimaPalavra
    strncpy(estado->ultimaPalavra, candidato, TAMANHO_MAX_PALAVRA);
    estado->ultimaPalavra[TAMANHO_MAX_PALAVRA] = '\0';

    // VALIDAR: Verificar se o candidato pode ser colocado
    if (!validarPalavra(grid, palavra, candidato)) {
      continue; // Candidato inválido, tentar próximo
    }

    // COLOCAR: Tentar colocar a palavra no grid
    if (!colocarPalavra(grid, palavra, candidato)) {
      continue; // Não conseguiu colocar, tentar próximo
    }

    // RECURSÃO: Tentar resolver o resto do puzzle
    if (backtrack(grid, indicePalavra + 1, estado)) {
      // SUCESSO! Encontrou solução
      liberarCandidatos(candidatos, numCandidatos);
      return true;
    }

    // BACKTRACK: Não encontrou solução com este candidato
    // Desfazer a colocação da palavra
    removerPalavra(grid, palavra);
    estado->retrocessos++;
  }

  // Liberar memória dos candidatos
  liberarCandidatos(candidatos, numCandidatos);

  // Nenhum candidato funcionou
  return false;
}

static void obterCandidatos(int tamanho, char ***candidatos,
                            int *numCandidatos) {
  // Usar a função do dicionário para buscar palavras por tamanho
  dict_search_by_size(tamanho, candidatos, numCandidatos);

  // Nota: A função dict_search_by_size já aloca memória
  // O chamador é responsável por liberar usando liberarCandidatos
}

static void liberarCandidatos(char **candidatos, int numCandidatos) {
  if (candidatos == NULL) {
    return;
  }

  // Liberar cada string
  for (int i = 0; i < numCandidatos; i++) {
    if (candidatos[i] != NULL) {
      free(candidatos[i]);
    }
  }

  // Liberar array de ponteiros
  free(candidatos);
}