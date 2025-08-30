@echo off
setlocal enabledelayedexpansion

set "INTERPRETER=%~dp0..\mini_script.exe"

echo Testing just the failing tests...

echo.
echo Running test_09_lists...
"%INTERPRETER%" "test_09_lists.ms" > temp_output.txt 2>&1
findstr /c:"PASSED" temp_output.txt >nul
if !errorlevel! equ 0 (
    echo   PASSED
) else (
    echo   FAILED
    echo   Output:
    type temp_output.txt
)

echo.
echo Running test_11_builtin_functions...
"%INTERPRETER%" "test_11_builtin_functions.ms" > temp_output.txt 2>&1
findstr /c:"PASSED" temp_output.txt >nul
if !errorlevel! equ 0 (
    echo   PASSED
) else (
    echo   FAILED
    echo   Output:
    type temp_output.txt
)

del temp_output.txt 2>nul
echo Done.
