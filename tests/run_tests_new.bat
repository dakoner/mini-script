@echo off
setlocal enabledelayedexpansion

set "INTERPRETER=%~dp0..\mini_script.exe"
set "PASSED=0"
set "FAILED=0"

echo Mini Script Test Suite
echo ==================================================
echo Interpreter: %INTERPRETER%
echo.

if not exist "%INTERPRETER%" (
    echo Error: Interpreter not found at %INTERPRETER%
    exit /b 1
)

echo Running tests...
echo.

REM Test each file individually to avoid for loop issues

call :run_test test_01_basic_types.ms
call :run_test test_02_arithmetic.ms
call :run_test test_03_strings.ms
call :run_test test_04_comparisons.ms
call :run_test test_05_logical.ms
call :run_test test_06_conditionals.ms
call :run_test test_07_while_loops.ms
call :run_test test_08_for_loops.ms
call :run_test test_09_lists.ms
call :run_test test_10_functions.ms
call :run_test test_11_builtin_functions.ms
call :run_test test_12_complex.ms
call :run_test test_13_edge_cases.ms
call :run_test test_14_dll_loading.ms
call :run_test test_15_large_file.ms
call :run_test test_16_module_import.ms

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

:run_test
set testfile=%1
echo Running %~n1...

"%INTERPRETER%" "%testfile%" > temp_output.txt
findstr /c:"PASSED" temp_output.txt >nul
if %errorlevel% equ 0 (
    echo   PASSED
    set /a PASSED+=1
) else (
    echo   FAILED
    set /a FAILED+=1
    echo   Output:
    type temp_output.txt
    echo.
)
goto :eof
