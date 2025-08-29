@echo off
REM Mini Script Test Runner for Windows
REM Runs all test files and reports results

setlocal enabledelayedexpansion

set "INTERPRETER=%~dp0..\mini_script.exe"
set "TESTS_DIR=%~dp0"
set "PASSED=0"
set "FAILED=0"

echo Mini Script Test Suite
echo ==================================================
echo Interpreter: %INTERPRETER%
echo Tests directory: %TESTS_DIR%
echo.

if not exist "%INTERPRETER%" (
    echo Error: Interpreter not found at %INTERPRETER%
    echo Please build the interpreter first using build.bat
    exit /b 1
)

echo Running tests...
echo.

REM Run each test file
for %%f in ("%TESTS_DIR%test_*.ms") do (
    echo Running %%~nf... 
    
    "%INTERPRETER%" "%%f" > temp_output.txt 2>&1
    
    REM Check if "PASSED" appears in output
    findstr /c:"PASSED" temp_output.txt >nul
    if !errorlevel! equ 0 (
        echo   PASSED
        set /a PASSED+=1
    ) else (
        echo   FAILED
        set /a FAILED+=1
        echo   Output:
        type temp_output.txt | findstr /n "."
        echo.
    )
)

del temp_output.txt 2>nul

echo.
echo ==================================================
echo Test Results: %PASSED% passed, %FAILED% failed

if %FAILED% gtr 0 (
    echo Overall result: FAILED ^(%FAILED% test^(s^) failed^)
    exit /b 1
) else (
    echo Overall result: ALL TESTS PASSED
    exit /b 0
)
