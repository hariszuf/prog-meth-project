@echo off

echo.
echo ========================================================================
echo Matrix Model Training
echo ========================================================================
echo.

cd ..

if not exist "dataset\new processed\train_non_terminal_matrix.data" (
    echo ERROR: Matrix datasets not found!
    pause
    exit /b 1
)

echo [OK] Datasets found

if not exist "models\matrix_models_nonterminal\linear_regression" mkdir "models\matrix_models_nonterminal\linear_regression"
if not exist "models\matrix_models_nonterminal\naive_bayes" mkdir "models\matrix_models_nonterminal\naive_bayes"
if not exist "models\matrix_models_nonterminal\q_learning" mkdir "models\matrix_models_nonterminal\q_learning"

echo.
echo ========================================================================
echo Training Linear Regression
echo ========================================================================
echo.

cd "src\model training matrix"

gcc linear_regression_matrix.c -o lr.exe -lm
if errorlevel 1 (
    echo Compilation failed
    cd ..\..
    pause
    exit /b 1
)

lr.exe
if errorlevel 1 (
    echo Training failed
    cd ..\..
    pause
    exit /b 1
)

echo [OK] Linear Regression trained

echo.
echo ========================================================================
echo Training Naive Bayes
echo ========================================================================
echo.

gcc naive_bayes_matrix.c -o nb.exe -lm
if errorlevel 1 (
    echo Compilation failed
    cd ..\..
    pause
    exit /b 1
)

nb.exe
if errorlevel 1 (
    echo Training failed
    cd ..\..
    pause
    exit /b 1
)

echo [OK] Naive Bayes trained

cd ..\..

echo.
echo ========================================================================
echo Training Complete
echo ========================================================================
echo.

if exist "models\matrix_models_nonterminal\linear_regression\linear_regression_matrix_nonterminal.txt" (
    echo [OK] Linear Regression model created
)

if exist "models\matrix_models_nonterminal\naive_bayes\naive_bayes_matrix_nonterminal.txt" (
    echo [OK] Naive Bayes model created
)

echo.
echo Models saved to: models\matrix_models_nonterminal\
echo.
echo To implement in game, run: experiment\implement_best_matrix_model.bat
echo.

pause
