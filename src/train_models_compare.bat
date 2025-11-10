@echo off
REM ========================================
REM Advanced Training Script
REM Trains models with BOTH non-terminal and combined datasets
REM Compares performance
REM ========================================

REM Ensure we're in the src directory
if not exist "data related\dataset_processor.c" (
    echo ERROR: This script must be run from the src directory!
    echo Current directory: %CD%
    echo.
    echo Please navigate to src and run again:
    echo   cd c:\Users\muhdh\OneDrive\Documents\GitHub\prog-meth-project\src
    echo   .\train_models_compare.bat
    pause
    exit /b 1
)

echo ========================================
echo TIC-TAC-TOE ML TRAINING - COMPARISON MODE
echo ========================================
echo.
echo This script will train both models with:
echo   1. Non-terminal dataset (strategic positions)
echo   2. Combined dataset (all positions)
echo.
echo You can compare which performs better!
echo.
pause

REM ========================================
REM Check Prerequisites
REM ========================================
echo Checking prerequisites...

if not exist "data related\dataset_processor.exe" (
    echo Compiling dataset processor...
    gcc "data related\dataset_processor.c" -o "data related\dataset_processor.exe"
    if errorlevel 1 (
        echo ERROR: Failed to compile dataset_processor.c
        pause
        exit /b 1
    )
)

if not exist "..\dataset\tic-tac-toe-minimax-non-terminal.data" (
    echo WARNING: tic-tac-toe-minimax-non-terminal.data not found!
    echo Please generate it first using dataset-gen.exe
)

if not exist "..\dataset\tic-tac-toe-minimax-complete.data" (
    echo WARNING: tic-tac-toe-minimax-complete.data not found!
    echo Please generate it first using dataset-gen.exe
)

echo.

REM ========================================
REM Compile Training Programs
REM ========================================
echo ========================================
echo COMPILING TRAINING PROGRAMS
echo ========================================
echo.

echo Compiling Naive Bayes...
if exist "model training\naive_bayes.c" (
    gcc "model training\naive_bayes.c" -o naive_bayes.exe -lm
    if errorlevel 1 (
        echo ERROR: Failed to compile naive_bayes.c
        echo Make sure gcc is in your PATH
        pause
        exit /b 1
    )
    echo Done!
) else (
    echo ERROR: model training\naive_bayes.c not found!
    echo Current directory: %CD%
    pause
    exit /b 1
)

echo Compiling Linear Regression...
if exist "model training\linear_regression.c" (
    gcc "model training\linear_regression.c" -o linear_regression.exe -lm
    if errorlevel 1 (
        echo ERROR: Failed to compile linear_regression.c
        echo Make sure gcc is in your PATH
        pause
        exit /b 1
    )
    echo Done!
) else (
    echo ERROR: model training\linear_regression.c not found!
    echo Current directory: %CD%
    pause
    exit /b 1
)

echo.
echo All programs compiled successfully!
echo.

echo Compiling Q-Learning (optional)...
if exist "model training\q_learning.c" (
    gcc "model training\q_learning.c" -o q_learning.exe -lm
    if errorlevel 1 (
        echo WARNING: Failed to compile q_learning.c
        echo Q-Learning training will be skipped
    ) else (
        echo Q-Learning compiled successfully!
    )
) else (
    echo model training\q_learning.c not found, Q-Learning training will be skipped
)
echo.
pause

REM ========================================
REM TRAINING SET 1: NON-TERMINAL DATASET
REM ========================================
echo.
echo ========================================
echo TRAINING SET 1: NON-TERMINAL DATASET
echo ========================================
echo.

if exist "..\dataset\tic-tac-toe-minimax-non-terminal.data" (
    echo Step 1/4: Processing non-terminal dataset...
    "data related\dataset_processor.exe" ..\dataset\tic-tac-toe-minimax-non-terminal.data 0.8
    
    if not exist "..\dataset\new processed\train_non_terminal.data" (
        echo ERROR: Failed to create train_non_terminal.data
        pause
        exit /b 1
    )
    
    echo.
    echo Step 2/4: Training Naive Bayes on Non-Terminal dataset...
    copy /Y "..\dataset\new processed\train_non_terminal.data" train.data
    copy /Y "..\dataset\new processed\test_non_terminal.data" test.data
    naive_bayes.exe
    
    REM Save model with specific name
    if not exist "..\models\naive_bayes_non_terminal" mkdir "..\models\naive_bayes_non_terminal"
    copy /Y "naive_bayes_model.txt" "..\models\naive_bayes_non_terminal\model_non_terminal.txt"
    echo Saved to: ..\models\naive_bayes_non_terminal\model_non_terminal.txt
    
    echo.
    echo Step 3/4: Training Linear Regression on Non-Terminal dataset...
    copy /Y "..\dataset\new processed\train_non_terminal.data" train.data
    copy /Y "..\dataset\new processed\test_non_terminal.data" test.data
    
    linear_regression.exe
    
    REM Save model with specific name
    if not exist "..\models\linear_regression_non_terminal" mkdir "..\models\linear_regression_non_terminal"
    copy /Y "linear_regression_model.txt" "..\models\linear_regression_non_terminal\model_non_terminal.txt"
    if exist "linear_regression_model.bin" (
        copy /Y "linear_regression_model.bin" "..\models\linear_regression_non_terminal\model_non_terminal.bin"
    )
    echo Saved to: ..\models\linear_regression_non_terminal\
    
    echo.
    echo Step 4/4: Training Q-Learning on Non-Terminal dataset...
    if exist "q_learning.exe" (
        q_learning.exe ..\dataset\tic-tac-toe-minimax-non-terminal.data
        if exist "q_learning_model.txt" (
            if not exist "..\models\q learning" mkdir "..\models\q learning"
            copy /Y q_learning_model.txt "..\models\q learning\q_learning_non_terminal.txt"
            echo Saved to: ..\models\q learning\q_learning_non_terminal.txt
        )
    ) else (
        echo Q-learning not compiled. Skipping...
    )
    
    REM Move report files to dataset results folder
    if exist "report_non_terminal.txt" (
        if not exist "..\dataset\new results" mkdir "..\dataset\new results"
        move /Y report_non_terminal.txt "..\dataset\new results\" >nul
    )
    
    echo.
    echo Non-terminal training complete!
    pause
) else (
    echo Skipping non-terminal dataset (file not found)
)

REM ========================================
REM TRAINING SET 2: COMBINED DATASET
REM ========================================
echo.
echo ========================================
echo TRAINING SET 2: COMBINED DATASET
echo ========================================
echo.

if exist "..\dataset\tic-tac-toe-minimax-complete.data" (
    echo Step 1/4: Processing combined dataset...
    "data related\dataset_processor.exe" ..\dataset\tic-tac-toe-minimax-complete.data 0.8
    
    if not exist "..\dataset\new processed\train_combined.data" (
        echo ERROR: Failed to create train_combined.data
        pause
        exit /b 1
    )
    
    echo.
    echo Step 2/4: Training Naive Bayes on Combined dataset...
    copy /Y "..\dataset\new processed\train_combined.data" train.data
    copy /Y "..\dataset\new processed\test_combined.data" test.data
    naive_bayes.exe
    
    REM Save model with specific name
    if not exist "..\models\naive_bayes_combined" mkdir "..\models\naive_bayes_combined"
    copy /Y "naive_bayes_model.txt" "..\models\naive_bayes_combined\model_combined.txt"
    echo Saved to: ..\models\naive_bayes_combined\model_combined.txt
    
    echo.
    echo Step 3/4: Training Linear Regression on Combined dataset...
    copy /Y "..\dataset\new processed\train_combined.data" train.data
    copy /Y "..\dataset\new processed\test_combined.data" test.data
    
    linear_regression.exe
    
    REM Save model with specific name
    if not exist "..\models\linear_regression_combined" mkdir "..\models\linear_regression_combined"
    copy /Y "linear_regression_model.txt" "..\models\linear_regression_combined\model_combined.txt"
    if exist "linear_regression_model.bin" (
        copy /Y "linear_regression_model.bin" "..\models\linear_regression_combined\model_combined.bin"
    )
    echo Saved to: ..\models\linear_regression_combined\
    
    echo.
    echo Step 4/4: Training Q-Learning on Combined dataset...
    if exist "q_learning.exe" (
        q_learning.exe ..\dataset\tic-tac-toe-minimax-complete.data
        if exist "q_learning_model.txt" (
            if not exist "..\models\q learning" mkdir "..\models\q learning"
            copy /Y q_learning_model.txt "..\models\q learning\q_learning_combined.txt"
            echo Saved to: ..\models\q learning\q_learning_combined.txt
        )
    ) else (
        echo Q-learning not compiled. Skipping...
    )
    
    REM Move report files to dataset results folder
    if exist "report_combined.txt" (
        if not exist "..\dataset\new results" mkdir "..\dataset\new results"
        move /Y report_combined.txt "..\dataset\new results\" >nul
    )
    if exist "dataset_report.txt" (
        move /Y dataset_report.txt "..\dataset\new results\" >nul
    )
    
    echo.
    echo Combined training complete!
    pause
) else (
    echo Skipping combined dataset (file not found)
)

REM ========================================
REM Summary
REM ========================================
echo.
echo ========================================
echo TRAINING COMPLETE - COMPARISON SUMMARY
echo ========================================
echo.
echo All models have been trained and saved to their respective folders:
echo.
echo [NON-TERMINAL MODELS]
echo   Naive Bayes:        ..\models\naive_bayes_non_terminal\model_non_terminal.txt
echo   Linear Regression:  ..\models\linear_regression_non_terminal\model_non_terminal.txt
if exist "..\models\linear_regression_non_terminal\model_non_terminal.bin" (
    echo   Linear Regression:  ..\models\linear_regression_non_terminal\model_non_terminal.bin
)
if exist "..\models\q learning\q_learning_non_terminal.txt" (
    echo   Q-Learning:         ..\models\q learning\q_learning_non_terminal.txt
)
echo.
echo [COMBINED MODELS]
echo   Naive Bayes:        ..\models\naive_bayes_combined\model_combined.txt
echo   Linear Regression:  ..\models\linear_regression_combined\model_combined.txt
if exist "..\models\linear_regression_combined\model_combined.bin" (
    echo   Linear Regression:  ..\models\linear_regression_combined\model_combined.bin
)
if exist "..\models\q learning\q_learning_combined.txt" (
    echo   Q-Learning:         ..\models\q learning\q_learning_combined.txt
)
echo.
echo [DATASETS]
echo   Processed data:     ..\dataset\new processed\
echo   Results/Reports:    ..\dataset\new results\
echo.
echo Next Steps:
echo   1. Compare accuracy metrics from the training output
echo   2. Review the dataset reports in ..\dataset\new results\
echo   3. Test models with evaluate_models.exe (if available)
echo   4. Compare win rates in AI vs AI gameplay
echo.
echo Model Files:
echo   - *_non_terminal.*  = Trained on strategic positions only
echo   - *_combined.*      = Trained on all positions (terminal + non-terminal)
echo.
echo To use a specific model in your game:
echo   1. Copy the desired model file to the appropriate location
echo   2. Update your AI code to load from that model file
echo.
echo ========================================

pause
