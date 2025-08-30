@echo off
REM Verbose Test Runner - shows detailed test output
REM ===============================================

if "%1"=="" (
    echo Usage: %0 ^<test_name^>
    echo Example: %0 test_01_basic_types
    echo          %0 test_19_namespaces
    exit /b 1
)

set TEST_FILE=tests\%1.ms

if not exist "%TEST_FILE%" (
    echo Error: Test file %TEST_FILE% not found
    exit /b 1
)

echo Running %1 with detailed output:
echo ================================
echo.

"%~dp0mini_script.exe" "%TEST_FILE%"

echo.
echo Exit code: %errorlevel%
