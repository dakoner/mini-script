# Setting Up a C Compiler on Windows

To compile and run the Mini Script Language interpreter, you need a C compiler. Here are several options:

## Option 1: MinGW-w64 (Recommended for beginners)

1. **Download**: Go to https://www.mingw-w64.org/downloads/
2. **Choose**: MSYS2 (https://www.msys2.org/)
3. **Install**: Download and run the installer
4. **Setup**: Open MSYS2 terminal and run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S make
   ```
5. **Add to PATH**: Add `C:\msys64\mingw64\bin` to your Windows PATH

## Option 2: Visual Studio Community (Full featured)

1. **Download**: https://visualstudio.microsoft.com/vs/community/
2. **Install**: Choose "Desktop development with C++" workload
3. **Use**: Open "Developer Command Prompt for VS"

## Option 3: TDM-GCC (Simple installer)

1. **Download**: https://jmeubank.github.io/tdm-gcc/
2. **Install**: Run the installer
3. **Add to PATH**: Usually adds automatically

## Option 4: Code::Blocks (IDE with compiler)

1. **Download**: http://www.codeblocks.org/downloads
2. **Choose**: Version with MinGW included
3. **Install**: Follow installer instructions

## Quick Test

After installation, open Command Prompt (cmd) or PowerShell and test:

```cmd
gcc --version
```

If you see version information, you're ready to compile!

## Compilation Commands

### Using GCC (MinGW/TDM-GCC)
```cmd
gcc -Wall -Wextra -std=c99 -g -o mini_script.exe mini_script.c
```

### Using Visual Studio (cl.exe)
```cmd
cl /W4 /std:c17 mini_script.c /Fe:mini_script.exe
```

### Using the provided batch file
```cmd
build.bat
```

## Running the Program

After successful compilation:
```cmd
mini_script.exe
```

## Troubleshooting

### "gcc is not recognized"
- GCC is not installed or not in PATH
- Install one of the compilers above
- Make sure the compiler's bin directory is in your PATH

### "Permission denied"
- Try running Command Prompt as Administrator
- Check antivirus software isn't blocking compilation

### Compilation errors
- Make sure you're using C99 or later standard
- Check for missing semicolons or brackets in the code
- Verify all header files are available

## Alternative: Online Compilers

If you can't install a local compiler, you can use online services:
- repl.it
- onlinegdb.com
- ideone.com
- compiler-explorer.com

Just copy the `mini_script.c` content to these platforms.
