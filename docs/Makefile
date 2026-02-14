# Makefile for Palavras Cruzadas (Windows MinGW/w64devkit) - Updated

# Compiler and paths
CC = gcc
CFLAGS = -g -std=c99 -Wall -Wno-missing-braces -IC:/raylib/w64devkit/include -IC:/raylib/raylib/src -IC:/raylib/include -Isrc
LDFLAGS = -LC:/raylib/w64devkit/lib -Lsrc -lraylib -lopengl32 -lgdi32 -lwinmm

# Directories
SRC_DIR = src
BIN_DIR = bin

# Source files
SRCS = src/app/main.c \
       src/ui/interface.c \
       src/ui/ui_components.c \
       src/data/dictionary.c \
       src/data/grid.c \
       src/external/api_client.c \
       src/game/solver.c \
       src/game/validation.c \
       src/game/puzzle_generator.c \
       src/game/game_mode.c

# Output
TARGET = $(BIN_DIR)/main.exe

# Build rules
all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p $(BIN_DIR)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
