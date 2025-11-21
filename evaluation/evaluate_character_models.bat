@echo off
REM ========================================
REM Evaluate Character-Format Models
REM Generates confusion matrices for non-matrix models
REM ========================================

setlocal enabledelayedexpansion

echo ========================================
echo  CONFUSION MATRIX EVALUATION
echo  (Character Format Models)
echo ========================================
echo.

REM ========================================
REM Step 1: Compile Confusion Matrix Program
REM ========================================
echo [1/4] Compiling confusion matrix evaluator...
gcc confusion-matrix.c -o confusion-matrix.exe -lm

if errorlevel 1 (
    echo Error: Compilation failed!
    pause
    exit /b 1
)
echo   Done: confusion-matrix.exe
echo.
pause

REM ========================================
REM Step 2: Evaluate Linear Regression (Character)
REM ========================================
echo [2/4] Evaluating Linear Regression Model (Character Format)...
echo.

if exist "..\models\linear_regression_non_terminal\model_non_terminal.txt" (
    echo ----------------------------------------
    confusion-matrix.exe ^
        ..\models\linear_regression_non_terminal\model_non_terminal.txt ^
        "..\dataset\new processed\train_non_terminal.data" ^
        "..\dataset\new processed\test_non_terminal.data" ^
        > linear_regression_char_evaluation.txt
    
    type linear_regression_char_evaluation.txt
    echo.
    echo   Results saved to: linear_regression_char_evaluation.txt
) else (
    echo   Warning: Linear Regression model not found!
    echo   Expected: ..\models\linear_regression_non_terminal\model_non_terminal.txt
)
echo.
pause

REM ========================================
REM Step 3: Evaluate Naive Bayes (Character)
REM ========================================
echo [3/4] Evaluating Naive Bayes Model (Character Format)...
echo.

if exist "..\models\naive_bayes_non_terminal\model_non_terminal.txt" (
    echo ----------------------------------------
    confusion-matrix.exe ^
        ..\models\naive_bayes_non_terminal\model_non_terminal.txt ^
        "..\dataset\new processed\train_non_terminal.data" ^
        "..\dataset\new processed\test_non_terminal.data" ^
        > naive_bayes_char_evaluation.txt
    
    type naive_bayes_char_evaluation.txt
    echo.
    echo   Results saved to: naive_bayes_char_evaluation.txt
) else (
    echo   Warning: Naive Bayes model not found!
    echo   Expected: ..\models\naive_bayes_non_terminal\model_non_terminal.txt
)
echo.
pause

REM ========================================
REM Step 4: Evaluate Q-Learning (Character)
REM ========================================
echo [4/4] Evaluating Q-Learning Model (Character Format)...
echo.

if exist "..\models\q learning\q_learning_non_terminal.txt" (
    echo ----------------------------------------
    confusion-matrix.exe ^
        "..\models\q learning\q_learning_non_terminal.txt" ^
        "..\dataset\new processed\train_non_terminal.data" ^
        "..\dataset\new processed\test_non_terminal.data" ^
        > q_learning_char_evaluation.txt
    
    type q_learning_char_evaluation.txt
    echo.
    echo   Results saved to: q_learning_char_evaluation.txt
) else (
    echo   Warning: Q-Learning model not found!
    echo   Expected: ..\models\q learning\q_learning_non_terminal.txt
)
echo.

REM ========================================
REM Summary
REM ========================================
echo ========================================
echo  EVALUATION COMPLETE
echo ========================================
echo.
echo Generated files:
if exist "linear_regression_char_evaluation.txt" echo   [✓] linear_regression_char_evaluation.txt
if exist "naive_bayes_char_evaluation.txt" echo   [✓] naive_bayes_char_evaluation.txt
if exist "q_learning_char_evaluation.txt" echo   [✓] q_learning_char_evaluation.txt
echo.
echo All results saved in: evaluation\
echo.
pause

endlocal
