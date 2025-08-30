@echo off
setlocal enabledelayedexpansion

set "INTERPRETER=%~dp0..\mini_script.exe"
set "PASSED=0"
set "FAILED=0"

echo Testing specific files in the same way as the main runner...

echo Running test_09_lists...
"%INTERPRETER%" "test_09_lists.ms" > temp_output.txt

type temp_output.txt | findstr /c:"PASSED" >nul
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

echo Running test_11_builtin_functions...
"%INTERPRETER%" "test_11_builtin_functions.ms" > temp_output.txt

type temp_output.txt | findstr /c:"PASSED" >nul
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

echo Results: %PASSED% passed, %FAILED% failed
