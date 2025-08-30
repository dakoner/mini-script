@echo off
REM Mini Script Language Test Suite
REM ===============================
REM Runs all tests and reports results

setlocal enabledelayedexpansion

echo Mini Script Test Suite
echo ==================================================
echo Interpreter: %~dp0mini_script.exe
echo.

set PASSED=0
set FAILED=0
set FAILED_TESTS=

echo Running tests...
echo.

REM Core language tests (01-13)
call :run_test test_01_basic_types "Basic Types"
call :run_test test_02_arithmetic "Arithmetic Operations"
call :run_test test_03_strings "String Operations"
call :run_test test_04_comparisons "Comparison Operations"
call :run_test test_05_logical "Logical Operations"
call :run_test test_06_conditionals "Conditional Statements"
call :run_test test_07_while_loops "While Loops"
call :run_test test_08_for_loops "For Loops"
call :run_test test_09_lists "List Operations"
call :run_test test_10_functions "User Functions"
call :run_test test_11_builtin_functions "Built-in Functions"
call :run_test test_12_complex "Complex Expressions"
call :run_test test_13_edge_cases "Edge Cases"

REM Advanced features (14-20)
call :run_test test_14_dll_loading "DLL Loading"
call :run_test test_15_large_file "Large File Handling"
call :run_test test_16_module_import "Module Import"
call :run_test test_17_file_operations "File Operations"
call :run_test test_18_time_operations "Time Operations"
call :run_test test_19_namespaces "Namespace System"
call :run_test test_20_error_reporting "Error Reporting"

echo.
echo ==================================================
echo Test Results: !PASSED! passed, !FAILED! failed

if !FAILED! gtr 0 (
    echo Failed tests: !FAILED_TESTS!
    echo Overall result: FAILED ^(!FAILED! test^(s^) failed^)
    exit /b 1
) else (
    echo Overall result: ALL TESTS PASSED
    exit /b 0
)

:run_test
set TEST_NAME=%~1
set TEST_DESC=%~2

echo Running %TEST_NAME%...
"%~dp0mini_script.exe" "tests\%TEST_NAME%.ms" > nul 2>&1

if errorlevel 1 (
    echo   FAILED - %TEST_DESC%
    set /a FAILED+=1
    set FAILED_TESTS=!FAILED_TESTS! %TEST_NAME%
) else (
    echo   PASSED - %TEST_DESC%
    set /a PASSED+=1
)

goto :eof
