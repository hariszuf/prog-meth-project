@echo off
REM ========================================
REM Evaluate ALL Models (Both Formats)
REM Comprehensive confusion matrix evaluation
REM ========================================

setlocal enabledelayedexpansion

echo ========================================
echo  COMPREHENSIVE MODEL EVALUATION
echo ========================================
echo.
echo This script evaluates:
echo   [1] Character format models (x,o,b)
echo   [2] Matrix format models (1.0,-1.0,0.0)
echo.
pause

REM ========================================
REM Compile Evaluator
REM ========================================
echo ========================================
echo  COMPILING EVALUATOR
echo ========================================
echo.
gcc confusion-matrix.c -o confusion-matrix.exe -lm

if errorlevel 1 (
    echo Error: Compilation failed!
    pause
    exit /b 1
)
echo ✓ Compiled: confusion-matrix.exe
echo.
pause

REM ========================================
REM PART 1: Character Format Models
REM ========================================
echo.
echo ========================================
echo  PART 1: CHARACTER FORMAT MODELS
echo ========================================
echo.
pause

REM Linear Regression (Character)
echo [1/6] Linear Regression (Character)...
if exist "..\models\linear_regression_non_terminal\model_non_terminal.txt" (
    confusion-matrix.exe ^
        ..\models\linear_regression_non_terminal\model_non_terminal.txt ^
        "..\dataset\new processed\train_non_terminal.data" ^
        "..\dataset\new processed\test_non_terminal.data" ^
        > linear_regression_char_evaluation.txt 2>&1
    echo ✓ Saved: linear_regression_char_evaluation.txt
) else (
    echo ✗ Model not found
)
echo.

REM Naive Bayes (Character)
echo [2/6] Naive Bayes (Character)...
if exist "..\models\naive_bayes_non_terminal\model_non_terminal.txt" (
    confusion-matrix.exe ^
        ..\models\naive_bayes_non_terminal\model_non_terminal.txt ^
        "..\dataset\new processed\train_non_terminal.data" ^
        "..\dataset\new processed\test_non_terminal.data" ^
        > naive_bayes_char_evaluation.txt 2>&1
    echo ✓ Saved: naive_bayes_char_evaluation.txt
) else (
    echo ✗ Model not found
)
echo.

REM Q-Learning (Character)
echo [3/6] Q-Learning (Character)...
if exist "..\models\q learning\q_learning_non_terminal.txt" (
    confusion-matrix.exe ^
        "..\models\q learning\q_learning_non_terminal.txt" ^
        "..\dataset\new processed\train_non_terminal.data" ^
        "..\dataset\new processed\test_non_terminal.data" ^
        > q_learning_char_evaluation.txt 2>&1
    echo ✓ Saved: q_learning_char_evaluation.txt
) else (
    echo ✗ Model not found
)
echo.
pause

REM ========================================
REM PART 2: Matrix Format Models
REM ========================================
echo.
echo ========================================
echo  PART 2: MATRIX FORMAT MODELS
echo ========================================
echo.
pause

REM Linear Regression (Matrix)
echo [4/6] Linear Regression (Matrix)...
if exist "..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt" (
    confusion-matrix.exe ^
        ..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt ^
        "..\dataset\new processed\train_non_terminal_matrix.data" ^
        "..\dataset\new processed\test_non_terminal_matrix.data" ^
        > linear_regression_matrix_evaluation.txt 2>&1
    echo ✓ Saved: linear_regression_matrix_evaluation.txt
) else (
    echo ✗ Model not found (run train_matrix_models.bat first)
)
echo.

REM Naive Bayes (Matrix)
echo [5/6] Naive Bayes (Matrix)...
if exist "..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt" (
    confusion-matrix.exe ^
        ..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt ^
        "..\dataset\new processed\train_non_terminal_matrix.data" ^
        "..\dataset\new processed\test_non_terminal_matrix.data" ^
        > naive_bayes_matrix_evaluation.txt 2>&1
    echo ✓ Saved: naive_bayes_matrix_evaluation.txt
) else (
    echo ✗ Model not found (run train_matrix_models.bat first)
)
echo.

REM Q-Learning (Matrix)
echo [6/6] Q-Learning (Matrix)...
if exist "..\models\q learning\q_learning_non_terminal_matrix.txt" (
    confusion-matrix.exe ^
        "..\models\q learning\q_learning_non_terminal_matrix.txt" ^
        "..\dataset\new processed\train_non_terminal_matrix.data" ^
        "..\dataset\new processed\test_non_terminal_matrix.data" ^
        > q_learning_matrix_evaluation.txt 2>&1
    echo ✓ Saved: q_learning_matrix_evaluation.txt
) else (
    echo ✗ Model not found (run train_matrix_models.bat first)
)
echo.
pause

REM ========================================
REM Summary
REM ========================================
echo.
echo ========================================
echo  EVALUATION SUMMARY
echo ========================================
echo.
echo Character Format Models:
if exist "linear_regression_char_evaluation.txt" (echo   [✓] Linear Regression) else (echo   [✗] Linear Regression)
if exist "naive_bayes_char_evaluation.txt" (echo   [✓] Naive Bayes) else (echo   [✗] Naive Bayes)
if exist "q_learning_char_evaluation.txt" (echo   [✓] Q-Learning) else (echo   [✗] Q-Learning)
echo.
echo Matrix Format Models:
if exist "linear_regression_matrix_evaluation.txt" (echo   [✓] Linear Regression) else (echo   [✗] Linear Regression)
if exist "naive_bayes_matrix_evaluation.txt" (echo   [✓] Naive Bayes) else (echo   [✗] Naive Bayes)
if exist "q_learning_matrix_evaluation.txt" (echo   [✓] Q-Learning) else (echo   [✗] Q-Learning)
echo.
echo All results saved in: evaluation\
echo.

REM ========================================
REM Create Comparison Table
REM ========================================
echo ========================================
echo  ACCURACY COMPARISON
echo ========================================
echo.
echo Format    Model                 Train    Test
echo ------------------------------------------------------

for %%F in (char matrix) do (
    for %%M in (linear_regression naive_bayes q_learning) do (
        if exist "%%M_%%F_evaluation.txt" (
            echo | set /p="%%F      %%M    "
            findstr /C:"Accuracy:" "%%M_%%F_evaluation.txt" | findstr /C:"Testing"
        )
    )
)

echo.
echo For detailed confusion matrices, see individual .txt files
echo.
pause

endlocal
