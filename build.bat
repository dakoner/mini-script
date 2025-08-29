@echo off
echo Compiling Mini Script Language...

REM Try different compilers
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe" (
    echo Using Visual Studio 2022 Community...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    cl /W1 /std:c17 mini_script.c /Fe:mini_script.exe
) else if exist "C:\msys64\ucrt64\bin\gcc.exe" (
    echo Using MinGW...
    C:\msys64\ucrt64\bin\gcc.exe -Wall -Wextra -std=c99 -g -o mini_script.exe mini_script.c
) else if exist "C:\msys64\mingw64\bin\gcc.exe" (
    echo Using MSYS2 MinGW...
    C:\msys64\mingw64\bin\gcc.exe -Wall -Wextra -std=c99 -g -o mini_script.exe mini_script.c
) else (
    echo No C compiler found!
    echo Please install one of the following:
    echo 1. Visual Studio Community 2022 ^(free^)
    echo 2. MinGW-w64
    echo 3. MSYS2
    echo 4. TDM-GCC
    pause
    exit /b 1
)

if exist mini_script.exe (
    echo Compilation successful!
    echo Running the program...
    echo.
    mini_script.exe
) else (
    echo Compilation failed!
    pause
)
