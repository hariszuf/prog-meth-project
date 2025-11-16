@echo off
REM train.bat - Unified training launcher

echo ========================================
echo Q-LEARNING UNIFIED TRAINER
echo ========================================
echo.

REM Check GCC
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: GCC not found!
    pause
    exit /b 1
)

REM Compile
echo Compiling unified trainer...
gcc -o unified_q_trainer.exe unified_q_trainer.c -lm -Wall -O2

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo SELECT TRAINING MODE
echo ========================================
echo.
echo 1. From Scratch      - Pure RL (zero knowledge)
echo 2. Dataset-Init      - Start with dataset knowledge
echo 3. Resume Training   - Continue from your existing model
echo.
set /p choice="Enter choice (1-3): "

if "%choice%"=="1" goto scratch
if "%choice%"=="2" goto dataset
if "%choice%"=="3" goto resume
echo Invalid choice!
pause
exit /b 1

:scratch
echo.
echo FROM-SCRATCH TRAINING
echo ---------------------
set /p episodes="Enter episodes (0=unlimited): "
echo.
echo Starting from zero knowledge...
echo Model: ..\..\models\q learning\q_learning_scratch.txt
echo.
unified_q_trainer.exe scratch "..\..\models\q learning\q_learning_scratch.txt" %episodes%
goto end

:dataset
echo.
echo DATASET-INITIALIZED TRAINING
echo ----------------------------
set /p episodes="Enter episodes (0=unlimited): "
echo.
echo Loading dataset and initializing with Minimax...
echo Dataset: ..\..\dataset\tic-tac-toe-minimax-non-terminal.data (4,520 positions)
echo Model: ..\..\models\q learning\q_learning_dataset.txt
echo Expected ~16,000 Q-values after initialization
echo.
unified_q_trainer.exe dataset "..\..\dataset\tic-tac-toe-minimax-non-terminal.data" "..\..\models\q learning\q_learning_dataset.txt" %episodes%
goto end

:resume
echo.
echo RESUME TRAINING
echo ---------------
echo.
echo Available models:
echo 1. q_learning_from_scratch.txt      (4 KB - 120 entries)
echo 2. q_learning_non_terminal.txt      (527 KB - 16,167 entries)
echo 3. q_learning_o_dataset_continuous.txt (72 KB - 2,182 entries) [YOUR NEW MODEL]
echo.
set /p model_choice="Select model to resume (1-3): "
set /p episodes="Enter additional episodes (0=unlimited): "

if "%model_choice%"=="1" (
    set MODEL=..\..\models\q learning\q_learning_from_scratch.txt
) else if "%model_choice%"=="2" (
    set MODEL=..\..\models\q learning\q_learning_non_terminal.txt
) else if "%model_choice%"=="3" (
    set MODEL=..\..\models\q learning\q_learning_o_dataset_continuous.txt
) else (
    echo Invalid model choice!
    pause
    exit /b 1
)

echo.
echo Resuming training from: %MODEL%
echo.
unified_q_trainer.exe resume "%MODEL%" %episodes%
goto end

:end
echo.
echo ========================================
echo Training session complete!
echo ========================================
pause
