@echo off
setlocal enabledelayedexpansion

set "INTERPRETER=%~dp0..\mini_script.exe"

echo Running test_09_lists...
"%INTERPRETER%" "test_09_lists.ms" > temp_output.txt 2>&1

echo Debug: Output file size:
dir temp_output.txt | findstr temp_output.txt

echo Contents of temp_output.txt:
type temp_output.txt

echo.
echo Testing findstr:
type temp_output.txt | findstr /c:"PASSED" >nul
echo findstr errorlevel: !errorlevel!

if !errorlevel! equ 0 (
    echo   RESULT: PASSED
) else (
    echo   RESULT: FAILED
)
