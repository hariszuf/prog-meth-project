@echo off
REM Compilation script for Tic-Tac-Toe with Naive Bayes AI

echo Compiling Tic-Tac-Toe GUI with AI...

REM Adjust the raylib path if necessary
REM This assumes raylib is installed in a standard location
REM You may need to modify -I and -L paths to match your raylib installation

gcc -o ttt_gui.exe ^
    gui_ai.c ^
    game.c ^
    minimax.c ^
    naive_bayes_ai.c ^
    stats.c ^
    -I"C:\raylib\raylib\src" ^
    -L"C:\raylib\raylib\src" ^
    -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lshell32 -lws2_32 ^
    -Wall

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful! Run ttt_gui.exe to play.
) else (
    echo.
    echo Compilation failed. Please check the errors above.
)

pause
