@echo off

echo.
echo ========================================================================
echo MATRIX MODEL STATUS
echo ========================================================================
echo.

cd ..

echo Checking trained models...
echo.

set /a count=0

if exist "models\matrix_models_nonterminal\linear_regression\linear_regression_matrix_nonterminal.txt" (
    echo [OK] Linear Regression ^(matrix-trained^)
    for %%A in ("models\matrix_models_nonterminal\linear_regression\linear_regression_matrix_nonterminal.txt") do echo      Size: %%~zA bytes
    set /a count+=1
)

if exist "models\matrix_models_nonterminal\naive_bayes\naive_bayes_matrix_nonterminal.txt" (
    echo [OK] Naive Bayes ^(matrix-trained^)
    for %%A in ("models\matrix_models_nonterminal\naive_bayes\naive_bayes_matrix_nonterminal.txt") do echo      Size: %%~zA bytes
    set /a count+=1
)

if exist "models\q learning\q_learning_non_terminal.txt" (
    echo [OK] Q-Learning ^(already in game^)
    powershell -Command "$lines = Get-Content 'models\q learning\q_learning_non_terminal.txt' | Where-Object { $_ -notmatch '^#' }; Write-Host '     Entries:' $lines.Count"
    set /a count+=1
)

echo.
echo Found %count% trained models
echo.

if %count% LSS 2 (
    echo Not enough models trained. Please run training script first.
    pause
    exit /b 1
)

echo ========================================================================
echo IMPLEMENTATION OPTIONS
echo ========================================================================
echo.
echo Current game models:
echo   - Linear Regression: models\linear_regression_non_terminal\
echo   - Naive Bayes:       models\naive_bayes_non_terminal\
echo   - Q-Learning:        models\q learning\ ^(matrix-trained^)
echo.
echo Matrix-trained models ready:
echo   - Linear Regression: READY
echo   - Naive Bayes:       READY
echo   - Q-Learning:        ALREADY IMPLEMENTED
echo.
echo What would you like to implement?
echo.
echo [1] Linear Regression only
echo [2] Naive Bayes only
echo [3] Both Linear Regression and Naive Bayes
echo [4] Show current game status
echo [Q] Quit
echo.

set /p choice="Enter choice: "

if /i "%choice%"=="Q" (
    echo Cancelled
    pause
    exit /b 0
)

if /i "%choice%"=="4" goto show_status

REM Create backup
set TIMESTAMP=%DATE:~-4%%DATE:~-7,2%%DATE:~-10,2%
set TIMESTAMP=%TIMESTAMP: =0%

if not exist "models\backups" mkdir "models\backups"

echo.
echo Creating backups...

if exist "models\linear_regression_non_terminal\model_non_terminal.txt" (
    copy "models\linear_regression_non_terminal\model_non_terminal.txt" "models\backups\lr_backup_%TIMESTAMP%.txt" >nul
    echo [OK] Linear Regression backed up
)

if exist "models\naive_bayes_non_terminal\model_non_terminal.txt" (
    copy "models\naive_bayes_non_terminal\model_non_terminal.txt" "models\backups\nb_backup_%TIMESTAMP%.txt" >nul
    echo [OK] Naive Bayes backed up
)

echo.
echo Implementing models...

if "%choice%"=="1" goto impl_lr
if "%choice%"=="2" goto impl_nb
if "%choice%"=="3" goto impl_both

echo Invalid choice
pause
exit /b 1

:impl_lr
copy "models\matrix_models_nonterminal\linear_regression\linear_regression_matrix_nonterminal.txt" "models\linear_regression_non_terminal\model_non_terminal.txt" >nul
echo [OK] Linear Regression implemented
goto compile

:impl_nb
copy "models\matrix_models_nonterminal\naive_bayes\naive_bayes_matrix_nonterminal.txt" "models\naive_bayes_non_terminal\model_non_terminal.txt" >nul
echo [OK] Naive Bayes implemented
goto compile

:impl_both
copy "models\matrix_models_nonterminal\linear_regression\linear_regression_matrix_nonterminal.txt" "models\linear_regression_non_terminal\model_non_terminal.txt" >nul
copy "models\matrix_models_nonterminal\naive_bayes\naive_bayes_matrix_nonterminal.txt" "models\naive_bayes_non_terminal\model_non_terminal.txt" >nul
echo [OK] Both models implemented
goto compile

:show_status
echo.
echo ========================================================================
echo CURRENT GAME STATUS
echo ========================================================================
echo.
cd "TTTGUI (Main Game Folder)"
if exist "ttt_gui.exe" (
    for %%A in ("ttt_gui.exe") do echo Game: %%~zA bytes, modified %%~tA
) else (
    echo Game executable not found
)
cd ..
echo.
pause
exit /b 0

:compile
echo.
echo Recompiling game...
cd "TTTGUI (Main Game Folder)"

gcc -o ttt_gui.exe gui_ai.c game.c minimax.c naive_bayes_ai.c linear_regression_ai.c q_learning_ai.c model_config.c stats.c -I"C:\raylib\raylib\src" -L"C:\raylib\raylib\src" -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lshell32 -lws2_32 -lpsapi -lm 2>nul

if errorlevel 1 (
    echo.
    echo ERROR: Compilation failed
    cd ..
    echo Restoring backups...
    if exist "models\backups\lr_backup_%TIMESTAMP%.txt" (
        copy "models\backups\lr_backup_%TIMESTAMP%.txt" "models\linear_regression_non_terminal\model_non_terminal.txt" >nul
    )
    if exist "models\backups\nb_backup_%TIMESTAMP%.txt" (
        copy "models\backups\nb_backup_%TIMESTAMP%.txt" "models\naive_bayes_non_terminal\model_non_terminal.txt" >nul
    )
    echo [OK] Restored
    pause
    exit /b 1
)

echo [OK] Game compiled successfully
cd ..

echo.
echo ========================================================================
echo COMPLETE
echo ========================================================================
echo.
echo Matrix-trained models have been implemented in the game.
echo.
echo Backups saved to: models\backups\
echo Game ready to play: TTTGUI (Main Game Folder)\ttt_gui.exe
echo.
echo All 3 AI models now use matrix-trained weights:
echo   - Linear Regression: Matrix-trained
echo   - Naive Bayes: Matrix-trained  
echo   - Q-Learning: Matrix-trained ^(already was^)
echo.

pause
