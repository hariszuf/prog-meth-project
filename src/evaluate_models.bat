@echo off
REM ========================================
REM Model Evaluation Script
REM Tests all trained models against Minimax
REM ========================================

echo ========================================
echo TIC-TAC-TOE MODEL EVALUATION
echo ========================================
echo.
echo This script will:
echo   1. Compile the evaluation program
echo   2. Test all models against perfect Minimax play
echo   3. Generate a comprehensive comparison report
echo.
echo Each model will play 100 games against Minimax (as X)
echo Best possible result: 100%% draws (perfect play)
echo.
pause

REM ========================================
REM Compile the evaluation program
REM ========================================
echo.
echo Compiling evaluation program...
gcc evaluate_against_minimax.c -o evaluate_against_minimax.exe -lm

if errorlevel 1 (
    echo ERROR: Failed to compile evaluation program!
    echo Make sure gcc is in your PATH
    pause
    exit /b 1
)

echo Compilation successful!
echo.

REM ========================================
REM Check for trained models
REM ========================================
echo Checking for trained models...
echo.

set MODEL_FOUND=0

if exist "..\models\naive_bayes_non_terminal\model_non_terminal.txt" (
    echo [FOUND] Naive Bayes ^(Non-Terminal^)
    set MODEL_FOUND=1
) else (
    echo [MISSING] Naive Bayes ^(Non-Terminal^)
)

if exist "..\models\naive_bayes_combined\model_combined.txt" (
    echo [FOUND] Naive Bayes ^(Combined^)
    set MODEL_FOUND=1
) else (
    echo [MISSING] Naive Bayes ^(Combined^)
)

if exist "..\models\linear_regression_non_terminal\model_non_terminal.txt" (
    echo [FOUND] Linear Regression ^(Non-Terminal^)
    set MODEL_FOUND=1
) else (
    echo [MISSING] Linear Regression ^(Non-Terminal^)
)

if exist "..\models\linear_regression_combined\model_combined.txt" (
    echo [FOUND] Linear Regression ^(Combined^)
    set MODEL_FOUND=1
) else (
    echo [MISSING] Linear Regression ^(Combined^)
)

if exist "..\models\q learning\q_learning_non_terminal.txt" (
    echo [FOUND] Q-Learning ^(Non-Terminal^)
    set MODEL_FOUND=1
) else (
    echo [MISSING] Q-Learning ^(Non-Terminal^)
)

if exist "..\models\q learning\q_learning_combined.txt" (
    echo [FOUND] Q-Learning ^(Combined^)
    set MODEL_FOUND=1
) else (
    echo [MISSING] Q-Learning ^(Combined^)
)

echo.

if %MODEL_FOUND%==0 (
    echo ERROR: No trained models found!
    echo Please run train_models_compare.bat first to train the models.
    pause
    exit /b 1
)

REM ========================================
REM Run the evaluation
REM ========================================
echo ========================================
echo RUNNING EVALUATION
echo ========================================
echo.
echo This may take a minute or two...
echo.

evaluate_against_minimax.exe

if errorlevel 1 (
    echo.
    echo ERROR: Evaluation failed!
    pause
    exit /b 1
)

REM ========================================
REM Display results
REM ========================================
echo.
echo ========================================
echo RESULTS
echo ========================================
echo.

if exist "model_evaluation_report.txt" (
    echo Full report saved to: model_evaluation_report.txt
    echo.
    echo Opening report...
    echo.
    type model_evaluation_report.txt
    echo.
    echo.
    echo The report has been saved to: model_evaluation_report.txt
    echo You can open it in any text editor for detailed analysis.
) else (
    echo Warning: Report file not created!
)

echo.
echo ========================================
echo EVALUATION COMPLETE
echo ========================================
echo.
echo Next steps:
echo   1. Review the report to see which model performed best
echo   2. The best model should have the highest draw rate
echo   3. Copy the best model to your game directory
echo   4. Update your game code to use the winning model
echo.
pause
