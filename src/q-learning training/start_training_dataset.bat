@echo off
REM start_training_dataset.bat - Start dataset-initialized continuous training

echo ========================================
echo DATASET-INITIALIZED CONTINUOUS TRAINING
echo ========================================
echo.

REM Check if GCC is available
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: GCC not found in PATH!
    echo Please install MinGW or MSYS2 and add to PATH.
    pause
    exit /b 1
)

REM Compile the dataset-initialized training program
echo Compiling dataset-initialized training program...
gcc -o q_learning_continuous_dataset.exe q_learning_continuous_dataset.c -lm -Wall -O2

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo Compilation successful!
echo.

REM Check if dataset exists
set DATASET=..\..\dataset\tic-tac-toe.data
if not exist "%DATASET%" (
    echo WARNING: Dataset not found at %DATASET%
    echo Will start with zero initialization instead.
    echo.
)

REM Ask user for training mode
echo Training modes:
echo   1. Unlimited episodes (train until Ctrl+C)
echo   2. Limited episodes (specify number)
echo.
set /p mode="Select mode (1 or 2): "

if "%mode%"=="2" (
    set /p episodes="Enter number of episodes: "
    echo.
    echo Starting training for %episodes% episodes...
    echo - Initial knowledge: Dataset (Minimax-evaluated)
    echo - Continuous learning: Self-play vs Minimax
    echo - Press Ctrl+C to stop early and save progress.
    echo.
    q_learning_continuous_dataset.exe "%DATASET%" "..\..\models\q learning\q_learning_o_dataset_continuous.txt" %episodes%
) else (
    echo.
    echo Starting unlimited training...
    echo - Initial knowledge: Dataset (Minimax-evaluated)
    echo - Continuous learning: Self-play vs Minimax
    echo - Press Ctrl+C to stop and save progress.
    echo.
    q_learning_continuous_dataset.exe "%DATASET%" "..\..\models\q learning\q_learning_o_dataset_continuous.txt"
)

echo.
echo Training session ended.
echo Model saved to ../../models/q learning/q_learning_o_dataset_continuous.txt
pause
