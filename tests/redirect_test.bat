@echo off

echo Method 1: stdout only
"..\mini_script.exe" "test_09_lists.ms" > temp1.txt
echo File size: 
dir temp1.txt | findstr temp1.txt

echo.
echo Method 2: stderr only
"..\mini_script.exe" "test_09_lists.ms" 2> temp2.txt
echo File size:
dir temp2.txt | findstr temp2.txt

echo.
echo Method 3: both stdout and stderr to same file
"..\mini_script.exe" "test_09_lists.ms" > temp3.txt 2>&1
echo File size:
dir temp3.txt | findstr temp3.txt

echo.
echo Method 4: stdout to file, stderr to console
"..\mini_script.exe" "test_09_lists.ms" > temp4.txt
echo File size:
dir temp4.txt | findstr temp4.txt
