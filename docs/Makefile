# Makefile para Palavras Cruzadas (Raylib)

# Compilador e Caminhos
CC = gcc
RAYLIB_PATH = C:/raylib/w64devkit
CFLAGS = -g -std=c99 -Wall -Wno-missing-braces -I$(RAYLIB_PATH)/include -IC:/raylib/raylib/src -IC:/raylib/include -Isrc
LDFLAGS = -L$(RAYLIB_PATH)/lib -Lsrc -lraylib -lopengl32 -lgdi32 -lwinmm

# Diretorios
SRC_DIR = src
BIN_DIR = bin

# Arquivos
SOURCES = $(SRC_DIR)/app/main.c \
          $(SRC_DIR)/ui/interface.c \
          $(SRC_DIR)/ui/ui_components.c \
          $(SRC_DIR)/data/dictionary.c \
          $(SRC_DIR)/data/grid.c \
          $(SRC_DIR)/external/api_client.c \
          $(SRC_DIR)/game/solver.c \
          $(SRC_DIR)/game/validation.c \
          $(SRC_DIR)/game/game_mode.c

# Output
TARGET = $(BIN_DIR)/main.exe

# Regras
all: $(TARGET)

$(TARGET): $(SOURCES)
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LDFLAGS)
	@echo Build successful! Executable is in $(TARGET)

run: all
	$(TARGET)

clean:
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"
	@echo Cleaned up bin directory.

.PHONY: all run clean
