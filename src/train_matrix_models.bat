@echo off
REM ========================================
REM Matrix-Based Model Training Script
REM Trains all models using standardized matrix format
REM ========================================

echo ========================================
echo MATRIX FORMAT MODEL TRAINING
echo Standardized Evaluation Pipeline
echo ========================================
echo.

REM Check if we're in the src directory
if not exist "data related\dataset_processor_matrix.c" (
    echo ERROR: This script must be run from the src directory!
    echo Current directory: %CD%
    echo.
    echo Please navigate to src and run again:
    echo   cd c:\Users\muhdh\Documents\GitHub\prog-meth-project\src
    echo   .\train_matrix_models.bat
    pause
    exit /b 1
)

echo Step 1/5: Checking prerequisites...
echo.

REM ========================================
REM STEP 1: Process Dataset to Matrix Format
REM ========================================
echo ========================================
echo STEP 1: PROCESS DATASET TO MATRIX FORMAT
echo ========================================
echo.

cd "data related"

REM Compile matrix processor if needed
if not exist "dataset_processor_matrix.exe" (
    echo Compiling matrix dataset processor...
    gcc dataset_processor_matrix.c -o dataset_processor_matrix.exe
    if errorlevel 1 (
        echo ERROR: Failed to compile dataset_processor_matrix.c
        pause
        exit /b 1
    )
    echo Done!
    echo.
)

REM Check if dataset exists
if not exist "..\..\dataset\tic-tac-toe-minimax-non-terminal.data" (
    echo ERROR: Non-terminal dataset not found!
    echo Expected: ..\..\dataset\tic-tac-toe-minimax-non-terminal.data
    pause
    exit /b 1
)

REM Process non-terminal dataset
echo Processing non-terminal dataset to matrix format...
echo Input:  ..\..\dataset\tic-tac-toe-minimax-non-terminal.data
echo Output: ..\..\dataset\new processed\train_non_terminal_matrix.data
echo         ..\..\dataset\new processed\test_non_terminal_matrix.data
echo.

dataset_processor_matrix.exe ..\..\dataset\tic-tac-toe-minimax-non-terminal.data 0.8

if errorlevel 1 (
    echo ERROR: Dataset processing failed!
    pause
    exit /b 1
)

echo.
echo ✓ Dataset processed successfully!
echo.
pause

REM ========================================
REM STEP 2: Compile Matrix Training Programs
REM ========================================
echo ========================================
echo STEP 2: COMPILE MATRIX TRAINING PROGRAMS
echo ========================================
echo.

cd "..\model training matrix"

echo Compiling Linear Regression (Matrix)...
gcc linear_regression_matrix.c -o linear_regression_matrix.exe -lm
if errorlevel 1 (
    echo ERROR: Failed to compile linear_regression_matrix.c
    pause
    exit /b 1
)
echo ✓ Linear Regression compiled

echo.
echo Compiling Naive Bayes (Matrix)...
gcc naive_bayes_matrix.c -o naive_bayes_matrix.exe -lm
if errorlevel 1 (
    echo ERROR: Failed to compile naive_bayes_matrix.c
    pause
    exit /b 1
)
echo ✓ Naive Bayes compiled

echo.
echo Compiling Q-Learning (with matrix support)...
cd "..\q-learning training"
gcc unified_q_trainer.c -o unified_q_trainer.exe -lm
if errorlevel 1 (
    echo ERROR: Failed to compile unified_q_trainer.c
    pause
    exit /b 1
)
echo ✓ Q-Learning compiled

echo.
echo All programs compiled successfully!
echo.
pause

REM ========================================
REM STEP 3: Train Linear Regression
REM ========================================
echo ========================================
echo STEP 3: TRAIN LINEAR REGRESSION (MATRIX)
echo ========================================
echo.

cd "..\model training matrix"

echo Training Linear Regression on non-terminal matrix dataset...
echo Train file: ..\..\dataset\new processed\train_non_terminal_matrix.data
echo Test file:  ..\..\dataset\new processed\test_non_terminal_matrix.data
echo Output:     ..\..\models\linear_regression_non_terminal\
echo.

linear_regression_matrix.exe ^
    ..\..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\..\dataset\new processed\test_non_terminal_matrix.data

if errorlevel 1 (
    echo ERROR: Linear Regression training failed!
    pause
    exit /b 1
)

REM Move output files to organized location
if not exist "..\..\models\linear_regression_non_terminal" mkdir "..\..\models\linear_regression_non_terminal"
move /Y linear_regression_model.txt ..\..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt >nul
if exist linear_regression_model.bin (
    move /Y linear_regression_model.bin ..\..\models\linear_regression_non_terminal\model_non_terminal_matrix.bin >nul
)

echo.
echo ✓ Linear Regression training complete!
echo   Model: ..\..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt
echo.
pause

REM ========================================
REM STEP 4: Train Naive Bayes
REM ========================================
echo ========================================
echo STEP 4: TRAIN NAIVE BAYES (MATRIX)
echo ========================================
echo.

echo Training Naive Bayes on non-terminal matrix dataset...
echo Train file: ..\..\dataset\new processed\train_non_terminal_matrix.data
echo Test file:  ..\..\dataset\new processed\test_non_terminal_matrix.data
echo Output:     ..\..\models\naive_bayes_non_terminal\
echo.

naive_bayes_matrix.exe ^
    ..\..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\..\dataset\new processed\test_non_terminal_matrix.data

if errorlevel 1 (
    echo ERROR: Naive Bayes training failed!
    pause
    exit /b 1
)

REM Move output files to organized location
if not exist "..\..\models\naive_bayes_non_terminal" mkdir "..\..\models\naive_bayes_non_terminal"
move /Y naive_bayes_model.txt ..\..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt >nul
if exist naive_bayes_model.bin (
    move /Y naive_bayes_model.bin ..\..\models\naive_bayes_non_terminal\model_non_terminal_matrix.bin >nul
)

echo.
echo ✓ Naive Bayes training complete!
echo   Model: ..\..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt
echo.
pause

REM ========================================
REM STEP 5: Train Q-Learning
REM ========================================
echo ========================================
echo STEP 5: TRAIN Q-LEARNING (AUTO-DETECT MATRIX)
echo ========================================
echo.

cd "..\q-learning training"

echo Training Q-Learning with dataset initialization...
echo Dataset:    ..\..\dataset\new processed\train_non_terminal_matrix.data
echo Format:     Matrix (auto-detected)
echo Output:     ..\..\models\q learning\q_learning_non_terminal_matrix.txt
echo Episodes:   5000
echo.
echo Note: Press Ctrl+C to stop training early and save checkpoint
echo.

unified_q_trainer.exe dataset ^
    ..\..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\..\models\q learning\q_learning_non_terminal_matrix.txt ^
    5000

if errorlevel 1 (
    echo ERROR: Q-Learning training failed!
    pause
    exit /b 1
)

echo.
echo ✓ Q-Learning training complete!
echo   Model: ..\..\models\q learning\q_learning_non_terminal_matrix.txt
echo.

REM ========================================
REM Summary
REM ========================================
echo.
echo ========================================
echo TRAINING COMPLETE - ALL MODELS
echo ========================================
echo.
echo All three models trained on standardized matrix format dataset!
echo.
echo [DATASET]
echo   Source:     tic-tac-toe-minimax-non-terminal.data
echo   Format:     Matrix (numerical)
echo   Processed:  ..\dataset\new processed\*_non_terminal_matrix.data
echo   Report:     ..\dataset\new results\report_non_terminal_matrix.txt
echo.
echo [MODELS TRAINED]
echo   1. Linear Regression
echo      ..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt
echo      ..\models\linear_regression_non_terminal\model_non_terminal_matrix.bin
echo.
echo   2. Naive Bayes
echo      ..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt
echo      ..\models\naive_bayes_non_terminal\model_non_terminal_matrix.bin
echo.
echo   3. Q-Learning
echo      ..\models\q learning\q_learning_non_terminal_matrix.txt
echo.
echo [EVALUATION]
echo   All models trained on identical preprocessed data
echo   Binary classification: win (+1) vs lose (-1)
echo   Ready for fair comparative analysis
echo.
echo [FOLDER STRUCTURE]
echo   prog-meth-project\
echo   ├── dataset\
echo   │   ├── tic-tac-toe-minimax-non-terminal.data (source)
echo   │   ├── new processed\
echo   │   │   ├── train_non_terminal_matrix.data
echo   │   │   └── test_non_terminal_matrix.data
echo   │   └── new results\
echo   │       └── report_non_terminal_matrix.txt
echo   └── models\
echo       ├── linear_regression_non_terminal\
echo       │   ├── model_non_terminal_matrix.txt
echo       │   └── model_non_terminal_matrix.bin
echo       ├── naive_bayes_non_terminal\
echo       │   ├── model_non_terminal_matrix.txt
echo       │   └── model_non_terminal_matrix.bin
echo       └── q learning\
echo           └── q_learning_non_terminal_matrix.txt
echo.
echo ========================================
echo.
echo Next steps:
echo   1. Review training accuracies above
echo   2. Compare model performance
echo   3. Test models in actual gameplay
echo   4. Document results for report
echo.
echo To train on COMBINED dataset instead, run:
echo   train_matrix_models_combined.bat
echo.
pause
