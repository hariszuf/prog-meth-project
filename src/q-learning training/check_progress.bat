@echo off
REM check_progress.bat - Check training progress and model statistics

echo ========================================
echo Q-LEARNING TRAINING PROGRESS
echo ========================================
echo.

set MODEL_DIR=..\..\models\q learning

echo Checking for continuous training models...
echo.

if exist "%MODEL_DIR%\q_learning_x_continuous.txt" (
    echo [X] Player X Model Found
    for %%A in ("%MODEL_DIR%\q_learning_x_continuous.txt") do (
        echo     Size: %%~zA bytes
        echo     Modified: %%~tA
    )
    findstr /C:"# Total entries:" "%MODEL_DIR%\q_learning_x_continuous.txt"
) else (
    echo [X] Player X Model: Not found
)

echo.

if exist "%MODEL_DIR%\q_learning_o_continuous.txt" (
    echo [O] Player O Model Found
    for %%A in ("%MODEL_DIR%\q_learning_o_continuous.txt") do (
        echo     Size: %%~zA bytes
        echo     Modified: %%~tA
    )
    findstr /C:"# Total entries:" "%MODEL_DIR%\q_learning_o_continuous.txt"
) else (
    echo [O] Player O Model: Not found
)

echo.
echo ========================================
echo EXISTING DEPLOYMENT MODELS
echo ========================================
echo.

if exist "%MODEL_DIR%\q_learning_non_terminal.txt" (
    echo [DEPLOY] Non-Terminal Model
    for %%A in ("%MODEL_DIR%\q_learning_non_terminal.txt") do (
        echo     Size: %%~zA bytes
    )
    findstr /C:"# Total entries:" "%MODEL_DIR%\q_learning_non_terminal.txt"
)

echo.

if exist "%MODEL_DIR%\q_learning_from_scratch.txt" (
    echo [DEPLOY] From-Scratch Model
    for %%A in ("%MODEL_DIR%\q_learning_from_scratch.txt") do (
        echo     Size: %%~zA bytes
    )
    findstr /C:"# Total entries:" "%MODEL_DIR%\q_learning_from_scratch.txt"
)

echo.
pause
