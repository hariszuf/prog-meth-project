@echo off
REM ========================================
REM Evaluate Matrix-Format Models
REM Generates confusion matrices for all trained models
REM ========================================

setlocal enabledelayedexpansion

echo ========================================
echo  CONFUSION MATRIX EVALUATION
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
REM Step 2: Evaluate Linear Regression
REM ========================================
echo [2/4] Evaluating Linear Regression Model...
echo.

if exist "..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt" (
    echo ----------------------------------------
    confusion-matrix.exe ^
        ..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt ^
        ..\dataset\new processed\train_non_terminal_matrix.data ^
        ..\dataset\new processed\test_non_terminal_matrix.data ^
        > linear_regression_evaluation.txt
    
    type linear_regression_evaluation.txt
    echo.
    echo   Results saved to: linear_regression_evaluation.txt
) else (
    echo   Warning: Linear Regression model not found!
    echo   Expected: ..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt
)
echo.
pause

REM ========================================
REM Step 3: Evaluate Naive Bayes
REM ========================================
echo [3/4] Evaluating Naive Bayes Model...
echo.

if exist "..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt" (
    echo ----------------------------------------
    confusion-matrix.exe ^
        ..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt ^
        ..\dataset\new processed\train_non_terminal_matrix.data ^
        ..\dataset\new processed\test_non_terminal_matrix.data ^
        > naive_bayes_evaluation.txt
    
    type naive_bayes_evaluation.txt
    echo.
    echo   Results saved to: naive_bayes_evaluation.txt
) else (
    echo   Warning: Naive Bayes model not found!
    echo   Expected: ..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt
)
echo.
pause

REM ========================================
REM Step 4: Evaluate Q-Learning
REM ========================================
echo [4/4] Evaluating Q-Learning Model...
echo.

if exist "..\models\q learning\q_learning_non_terminal_matrix.txt" (
    echo ----------------------------------------
    confusion-matrix.exe ^
        "..\models\q learning\q_learning_non_terminal_matrix.txt" ^
        ..\dataset\new processed\train_non_terminal_matrix.data ^
        ..\dataset\new processed\test_non_terminal_matrix.data ^
        > q_learning_evaluation.txt
    
    type q_learning_evaluation.txt
    echo.
    echo   Results saved to: q_learning_evaluation.txt
) else (
    echo   Warning: Q-Learning model not found!
    echo   Expected: ..\models\q learning\q_learning_non_terminal_matrix.txt
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
if exist "linear_regression_evaluation.txt" echo   [✓] linear_regression_evaluation.txt
if exist "naive_bayes_evaluation.txt" echo   [✓] naive_bayes_evaluation.txt
if exist "q_learning_evaluation.txt" echo   [✓] q_learning_evaluation.txt
echo.
echo All results saved in: evaluation\
echo.

REM Generate comparison summary
echo ========================================
echo  MODEL COMPARISON SUMMARY
echo ========================================
echo.

echo Extracting accuracy metrics...
echo Model                  Training    Testing
echo -------------------------------------------------

for %%M in (linear_regression naive_bayes q_learning) do (
    if exist "%%M_evaluation.txt" (
        for /f "tokens=2 delims=:" %%A in ('findstr /C:"Accuracy:" "%%M_evaluation.txt"') do (
            echo %%M: %%A
        )
    )
)

echo.
echo For detailed results, see individual evaluation files.
echo.
pause

endlocal
