@echo off
REM ========================================
REM Automated Training Script
REM Trains Naive Bayes and Linear Regression
REM with processed datasets
REM ========================================

echo ========================================
echo TIC-TAC-TOE ML MODEL TRAINING
echo ========================================
echo.

REM Check if dataset processor exists
if not exist "dataset_processor.exe" (
    echo Compiling dataset processor...
    gcc dataset_processor.c -o dataset_processor.exe
    if errorlevel 1 (
        echo ERROR: Failed to compile dataset_processor.c
        pause
        exit /b 1
    )
    echo Done!
    echo.
)

REM Check if dataset file exists
if not exist "tic-tac-toe-minimax-complete.data" (
    echo ERROR: Dataset file not found!
    echo Please run dataset-gen.exe first to generate the dataset.
    pause
    exit /b 1
)

REM ========================================
REM Step 1: Process Dataset
REM ========================================
echo ========================================
echo STEP 1: PROCESSING DATASET
echo ========================================
echo.
echo Processing tic-tac-toe-minimax-complete.data...
echo Using 80/20 train/test split
echo.

dataset_processor.exe tic-tac-toe-minimax-complete.data 0.8
if errorlevel 1 (
    echo ERROR: Failed to process dataset
    pause
    exit /b 1
)

echo.
echo Dataset processed successfully!
echo - train.data created
echo - test.data created
echo - dataset_report.txt created
echo.
pause

REM ========================================
REM Step 2: Compile Training Programs
REM ========================================
echo ========================================
echo STEP 2: COMPILING TRAINING PROGRAMS
echo ========================================
echo.

REM Compile Naive Bayes
echo Compiling Naive Bayes trainer...
gcc ..\src\naive_bayes.c -o naive_bayes.exe -lm
if errorlevel 1 (
    echo ERROR: Failed to compile naive_bayes.c
    pause
    exit /b 1
)
echo Done!

REM Compile Linear Regression
echo Compiling Linear Regression trainer...
gcc ..\src\linear_regression.c -o linear_regression.exe -lm
if errorlevel 1 (
    echo ERROR: Failed to compile linear_regression.c
    pause
    exit /b 1
)
echo Done!
echo.
pause

REM ========================================
REM Step 3: Train Naive Bayes
REM ========================================
echo ========================================
echo STEP 3: TRAINING NAIVE BAYES MODEL
echo ========================================
echo.
echo Training Naive Bayes classifier...
echo This may take a few moments...
echo.

naive_bayes.exe
if errorlevel 1 (
    echo ERROR: Naive Bayes training failed
    pause
    exit /b 1
)

echo.
echo Naive Bayes training complete!
echo Model saved to: ..\models\naive bayes\model.txt
echo.
pause

REM ========================================
REM Step 4: Prepare for Linear Regression
REM ========================================
echo ========================================
echo STEP 4: PREPARING LINEAR REGRESSION DATA
echo ========================================
echo.

REM Linear regression expects files in ../dataset/processed/
if not exist "..\dataset\processed" mkdir "..\dataset\processed"

echo Copying training data to ..\dataset\processed\
copy /Y train.data ..\dataset\processed\train.data
copy /Y test.data ..\dataset\processed\test.data

echo Done!
echo.

REM ========================================
REM Step 5: Train Linear Regression
REM ========================================
echo ========================================
echo STEP 5: TRAINING LINEAR REGRESSION MODEL
echo ========================================
echo.
echo Training Linear Regression model...
echo This may take a few moments...
echo.

linear_regression.exe
if errorlevel 1 (
    echo ERROR: Linear Regression training failed
    pause
    exit /b 1
)

echo.
echo Linear Regression training complete!
echo Model saved to: ..\models\linear regression\model.txt
echo Model saved to: ..\models\linear regression\model.bin
echo.

REM ========================================
REM Summary
REM ========================================
echo ========================================
echo TRAINING COMPLETE!
echo ========================================
echo.
echo Summary:
echo   - Dataset processed: tic-tac-toe-minimax-complete.data
echo   - Training samples: See dataset_report.txt
echo   - Testing samples: See dataset_report.txt
echo.
echo Models created:
echo   1. Naive Bayes: ..\models\naive bayes\model.txt
echo   2. Linear Regression: ..\models\linear regression\model.txt
echo   3. Linear Regression Binary: ..\models\linear regression\model.bin
echo.
echo Next steps:
echo   - Review training accuracy in the output above
echo   - Test models with evaluate_models.exe
echo   - Use models in AI vs AI gameplay
echo.
echo ========================================

pause
