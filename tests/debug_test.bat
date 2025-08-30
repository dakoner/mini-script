@echo off
setlocal enabledelayedexpansion

echo Testing test_11_builtin_functions.ms...

"..\mini_script.exe" "test_11_builtin_functions.ms" > temp_debug.txt 2>&1

echo Output written to temp_debug.txt
echo Checking for PASSED...

findstr /c:"PASSED" temp_debug.txt >nul
echo findstr errorlevel: %errorlevel%

if !errorlevel! equ 0 (
    echo   PASSED detected
) else (
    echo   FAILED - no PASSED found
)

echo Done.
