@echo off
echo Building Crossword Puzzle...

set RAYLIB_PATH=C:\raylib\w64devkit
set CC=%RAYLIB_PATH%\bin\gcc.exe
:: Add multiple common usage paths for Raylib includes to ensure we find raymath.h
set CFLAGS=-g -std=c99 -Wall -Wno-missing-braces -I%RAYLIB_PATH%\include -IC:\raylib\raylib\src -IC:\raylib\include -Isrc
set LDFLAGS=-L%RAYLIB_PATH%\lib -Lsrc -lraylib -lopengl32 -lgdi32 -lwinmm

if not exist "%CC%" (
    echo Error: GCC not found at %CC%
    echo Please ensure Raylib w64devkit is installed at C:\raylib\w64devkit
    echo Or update this script with the correct path.
    pause
    exit /b 1
)

if not exist "bin" mkdir bin

echo Compiling...
"%CC%" ^
  src\app\main.c ^
  src\ui\interface.c ^
  src\ui\ui_components.c ^
  src\data\dictionary.c ^
  src\data\grid.c ^
  src\external\api_client.c ^
  src\game\solver.c ^
  src\game\validation.c ^
  src\game\puzzle_generator.c ^
  src\game\game_mode.c ^
  -o bin\main.exe ^
  %CFLAGS% ^
  %LDFLAGS%

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

echo Build successful! Executable is in bin\main.exe
pause
