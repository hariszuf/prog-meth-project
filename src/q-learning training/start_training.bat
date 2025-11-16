@echo off
REM start_training.bat - Start continuous Q-Learning training

echo ========================================
echo CONTINUOUS Q-LEARNING TRAINING
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

REM Compile the continuous training program
echo Compiling continuous training program...
gcc -o q_learning_continuous.exe q_learning_continuous.c -lm -Wall -O2

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo Compilation successful!
echo.

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
    echo Press Ctrl+C to stop early and save progress.
    echo.
    q_learning_continuous.exe %episodes%
) else (
    echo.
    echo Starting unlimited training...
    echo Press Ctrl+C to stop and save progress.
    echo.
    q_learning_continuous.exe
)

echo.
echo Training session ended.
echo Models saved to ../../models/q learning/
pause
