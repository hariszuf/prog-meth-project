@echo off
REM compile_example.bat - Compile and run the frozen model example

echo ========================================
echo FROZEN MODEL EXAMPLE
echo ========================================
echo.

echo Compiling frozen model library and example...
gcc -c frozen_q_model.c -Wall -O2
gcc -o example_frozen_usage.exe example_frozen_usage.c frozen_q_model.o -lm -Wall -O2

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo Compilation successful!
echo.
echo Running example...
echo.

example_frozen_usage.exe

echo.
pause
