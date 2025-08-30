@echo off
REM Advanced Features Test Runner
REM =============================
REM Tests advanced features like modules, namespaces, file/time operations

setlocal enabledelayedexpansion

echo Mini Script Advanced Features Test Suite
echo =========================================
echo Testing advanced features...
echo.

set PASSED=0
set FAILED=0

REM Advanced feature tests (14-20)
call :run_test test_14_dll_loading "DLL Loading"
call :run_test test_15_large_file "Large File Handling"
call :run_test test_16_module_import "Module Import"
call :run_test test_17_file_operations "File Operations"
call :run_test test_18_time_operations "Time Operations"
call :run_test test_19_namespaces "Namespace System"
call :run_test test_20_error_reporting "Error Reporting"

echo.
echo Results: !PASSED! passed, !FAILED! failed

if !FAILED! gtr 0 (
    echo Some advanced tests FAILED
    exit /b 1
) else (
    echo All advanced tests PASSED
    exit /b 0
)

:run_test
set TEST_NAME=%~1
set TEST_DESC=%~2

echo Testing %TEST_NAME%...
"%~dp0mini_script.exe" "tests\%TEST_NAME%.ms" > nul 2>&1

if errorlevel 1 (
    echo   FAILED - %TEST_DESC%
    set /a FAILED+=1
) else (
    echo   PASSED - %TEST_DESC%
    set /a PASSED+=1
)

goto :eof
