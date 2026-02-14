# 🧩 Palavras Cruzadas (Crossword Solver)

Projeto desenvolvido para a disciplina **Estrutura de Dados II**.

Um solucionador de palavras cruzadas com ajuda de dicas baseadas nos significados das palavras em **C puro** utilizando **Backtracking** com visualização gráfica em **Raylib**.

---

## 🎯 Objetivos

Aplicar na prática conceitos de:

* Backtracking
* Estruturas de dados em grid
* Validação de restrições
* Internacionalização (PT/EN)
* Integração com API de dicionário (Simulada/Local)
* Visualização de algoritmos

---

## 🛠️ Tecnologias

* Linguagem: **C**
* Interface Gráfica: **Raylib**
* Algoritmo principal: **Backtracking**
* Plataforma: **Windows** 

---

## 🚀 Como Executar

### Pré-requisitos
1.  **Raylib Instalado**: Este projeto depende da biblioteca Raylib. Recomenda-se usar o kit `w64devkit` da Raylib.
    *   Instale em: `C:\raylib\w64devkit` (padrão)
    *   Ou ajuste os caminhos no `Makefile` / `build.bat`.

### Compilando e Rodando

**Opção A: Usando build.bat (Windows)**
1.  Abra o terminal na pasta do projeto.
2.  Execute:
    ```cmd
    build.bat
    ```
3.  O executável será criado em `bin/main.exe`.

**Opção B: Usando Makefile (MinGW/Make)**
1.  Abra o terminal.
2.  Execute:
    ```bash
    make
    ```
3.  Execute o jogo:
    ```bash
    make run
    ```

---

## 📂 Estrutura do Projeto

```
├── src/
│   ├── app/
│   │   └── main.c          # Lógica principal e loop do jogo
│   │
│   ├── ui/
│   │   ├── interface.c     # Renderização do grid e inputs
│   │   ├── interface.h
│   │   ├── ui_components.c # Botões e estilos
│   │   └── ui_components.h
│   │
│   ├── game/
│   │   ├── solver.c        # Algoritmo de Backtracking
│   │   ├── solver.h
│   │   ├── validation.c    # Regras de validação
│   │   ├── validation.h
│   │   ├── game_mode.c     # Modos de jogo
│   │   └── game_mode.h
│   │
│   ├── data/
│   │   ├── grid.c          # Manipulação da matriz
│   │   ├── grid.h
│   │   ├── dictionary.c    # Banco de palavras
│   │   ├── dictionary.h
│   │   └── structs.h       # Definições de tipos
│   │
│   ├── external/
│   │   ├── api_client.c    # Cliente API (simulado)
│   │   └── api_client.h
│
├── data_files/
│   ├── dictionaries/       # Arquivos de textos dos dicionários para em casa de falha da API
│   │   ├── pt.txt
│   │   └── en.txt
│
├── bin/
│   │   └── main.exe        # Executável final
|
├── docs/
|   │   ├── Makefile        # Script de compilação
|__ |   └── PalavrasCruzadas.pdf #Slide de Apresentação
```

---

## 🚧 Status do Projeto

✅ **Concluído**
- [x] Interface Gráfica (Raylib)
- [x] Lógica de Jogo (3 Níveis PT / 3 Níveis EN)
- [x] Backtracking/Solver
- [x] Sistema de Dicas
- [x] Internacionalização
- [x] Navegação por Teclado e Mouse

---

## 👥 Equipe

* Daiane Santos ([@daianesnts](https://github.com/daianesnts/))
* Guilhereme Seixas ([@guilheeme1108-prog](https://github.com/guilheeme1108-prog/))
* Igor Dias ([@iidias](https://github.com/iidias/))
* Matheus de Jesus ([@Littlemonster22](https://github.com/Littlemonster22/))
* Yasmim Passos ([@yasmim-passos](https://github.com/yasmim-passos/))

Projeto realizado em grupo como atividade avaliativa da disciplina de Estrutura de Dados II, na Universidade Federal de Sergipe (UFS).

