@echo off
echo Debug: Testing module import from batch script
echo Current directory: %CD%
echo.

echo Testing direct execution:
..\mini_script.exe test_16_module_import.ms

echo.
echo Testing with full paths:
echo Interpreter: %~dp0..\mini_script.exe
echo Test file: %~dp0test_16_module_import.ms

echo.
"%~dp0..\mini_script.exe" "%~dp0test_16_module_import.ms"
