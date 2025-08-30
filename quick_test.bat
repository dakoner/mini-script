@echo off
REM Quick Test Runner - runs core language tests only
REM =================================================

echo Mini Script Quick Test Suite
echo ============================
echo Running core language tests...
echo.

set PASSED=0
set FAILED=0

REM Core language tests only (01-13)
for %%f in (tests\test_0*.ms tests\test_1[0-3]*.ms) do (
    echo Testing %%~nf...
    "%~dp0mini_script.exe" "%%f" > nul 2>&1
    if errorlevel 1 (
        echo   FAILED
        set /a FAILED+=1
    ) else (
        echo   PASSED
        set /a PASSED+=1
    )
)

echo.
echo Results: %PASSED% passed, %FAILED% failed

if %FAILED% gtr 0 (
    echo Some core tests FAILED
    exit /b 1
) else (
    echo All core tests PASSED
    exit /b 0
)
