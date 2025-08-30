@echo off

echo Testing test_08_for_loops (should work):
"..\mini_script.exe" "test_08_for_loops.ms" > temp_test8.txt
echo File size:
dir temp_test8.txt | findstr temp_test8.txt
type temp_test8.txt | findstr /c:"PASSED" >nul
echo findstr result: %errorlevel%

echo.
echo Testing test_09_lists (problematic):
"..\mini_script.exe" "test_09_lists.ms" > temp_test9.txt
echo File size:
dir temp_test9.txt | findstr temp_test9.txt
type temp_test9.txt | findstr /c:"PASSED" >nul
echo findstr result: %errorlevel%

echo.
echo Testing test_10_functions (should work):
"..\mini_script.exe" "test_10_functions.ms" > temp_test10.txt
echo File size:
dir temp_test10.txt | findstr temp_test10.txt
type temp_test10.txt | findstr /c:"PASSED" >nul
echo findstr result: %errorlevel%
