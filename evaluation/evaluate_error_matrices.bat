@echo off
REM Batch script to generate 9x9 error matrices for all models

echo ========================================
echo 9x9 ERROR MATRIX EVALUATION
echo ========================================
echo.

REM Compile the error matrix program
echo Compiling error-matrix.c...
gcc error-matrix.c -o error-matrix.exe -lm
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed
    pause
    exit /b 1
)
echo Compilation successful!
echo.

REM Linear Regression Error Matrix
echo ========================================
echo Evaluating Linear Regression...
echo ========================================
error-matrix.exe "..\models\linear_regression_non_terminal\model_non_terminal.txt" linear_regression "..\dataset\new processed\test_non_terminal.data" > linear_regression_error_matrix.txt
echo Results saved to: linear_regression_error_matrix.txt
echo.

REM Naive Bayes Error Matrix
echo ========================================
echo Evaluating Naive Bayes...
echo ========================================
error-matrix.exe "..\models\naive_bayes_non_terminal\model_non_terminal.txt" naive_bayes "..\dataset\new processed\test_non_terminal.data" > naive_bayes_error_matrix.txt
echo Results saved to: naive_bayes_error_matrix.txt
echo.

REM Q-Learning Error Matrix
echo ========================================
echo Evaluating Q-Learning...
echo ========================================
error-matrix.exe "..\models\q learning\q_learning_non_terminal.txt" q_learning "..\dataset\new processed\test_non_terminal.data" > q_learning_error_matrix.txt
echo Results saved to: q_learning_error_matrix.txt
echo.

echo ========================================
echo EVALUATION COMPLETE
echo ========================================
echo All error matrices have been generated!
echo Check the following files:
echo   - linear_regression_error_matrix.txt
echo   - naive_bayes_error_matrix.txt
echo   - q_learning_error_matrix.txt
echo.
pause
