# Build Environment Setup for Mini Script Language

The Mini Script Language comes with an automated build system that detects and configures the appropriate compiler automatically. This guide covers setup options and manual compilation if needed.

## Recommended: Automated Build (Windows)

The project includes `build.bat` which automatically detects and configures Visual Studio 2022:

```cmd
# Simply run the build script
.\build.bat
```

**The build script automatically:**
- Locates Visual Studio 2022 Build Tools or Community Edition
- Sets up the compiler environment (vcvars64.bat)
- Compiles with optimizations and C17 standard
- Runs the program to show usage information
- Reports compilation success/failure

**Output example:**
```
Building Mini Script Language...
Setting up Visual Studio 2022 environment...
Compiling with optimizations...
Build successful: mini_script.exe created
```

## Compiler Options

### Option 1: Visual Studio 2022 (Recommended)

**For Build Tools (Lightweight)**:
1. **Download**: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
2. **Install**: Choose "C++ build tools" workload
3. **Use**: Run `.\build.bat` (automatic detection)

**For Community Edition (Full IDE)**:
1. **Download**: https://visualstudio.microsoft.com/vs/community/
2. **Install**: Choose "Desktop development with C++" workload
3. **Use**: Run `.\build.bat` or use Developer Command Prompt

### Option 2: MinGW-w64 (Alternative)

1. **Download**: Go to https://www.mingw-w64.org/downloads/
2. **Choose**: MSYS2 (https://www.msys2.org/)
3. **Install**: Download and run the installer
4. **Setup**: Open MSYS2 terminal and run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S make
   ```
5. **Add to PATH**: Add `C:\msys64\mingw64\bin` to your Windows PATH

### Option 3: TDM-GCC (Simple installer)

1. **Download**: https://jmeubank.github.io/tdm-gcc/
2. **Install**: Run the installer
3. **Add to PATH**: Usually adds automatically

## Quick Setup Verification

### Test Build System
```cmd
# Test the automated build
.\build.bat

# Expected output: successful compilation and usage message
```

### Test Compiler Manually
```cmd
# Test GCC (if using MinGW/TDM-GCC)
gcc --version

# Test MSVC (if using Visual Studio)
cl
```

If you see version information, the compiler is ready!

## Manual Compilation (If Needed)

If you need to compile manually or use a different compiler:

### Using Visual Studio Compiler (Manual)
```cmd
# Set up environment (if not using build.bat)
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

# Compile with optimizations
cl /W1 /std:c17 /O2 mini_script.c /Fe:mini_script.exe
```

### Using GCC (MinGW/TDM-GCC)
```cmd
# Standard compilation
gcc -Wall -Wextra -std=c17 -O2 -o mini_script.exe mini_script.c

# Debug version
gcc -Wall -Wextra -std=c17 -g -o mini_script.exe mini_script.c
```

### Using Clang (Alternative)
```cmd
# If you have Clang installed
clang -Wall -Wextra -std=c17 -O2 -o mini_script.exe mini_script.c
```

## Build Verification

After compilation, test the interpreter:

### Basic Test
```cmd
# Test the interpreter
.\mini_script.exe --help

# Expected output: Usage information and command-line options
```

### Run Test Suite
```cmd
# Run comprehensive tests
.\run_tests.bat

# Expected output: "Test Results: 20 passed, 0 failed"
```

### Test REPL Mode
```cmd
# Start interactive mode
.\mini_script.exe

# Expected: Interactive prompt for entering commands
```

## Troubleshooting

### Common Issues

**Build.bat reports "Visual Studio 2022 not found":**
- Install Visual Studio 2022 Build Tools or Community Edition
- Ensure C++ build tools workload is selected during installation

**GCC not found:**
- Verify GCC is in your PATH: `gcc --version`
- Reinstall MinGW-w64 and add to PATH

**Compilation errors:**
- Ensure you're using C17 standard or newer
- Check that mini_script.c is present in the current directory
- Verify compiler supports the required language features

**Runtime errors:**
- Test with simple script: `echo 'print("Hello");' > test.ms && .\mini_script.exe test.ms`
- Check file permissions and antivirus software
- Verify all required DLLs are available

### Development Setup

For development and modification:

```cmd
# Build debug version
gcc -Wall -Wextra -std=c17 -g -DDEBUG -o mini_script_debug.exe mini_script.c

# Build release version (optimized)
gcc -Wall -Wextra -std=c17 -O2 -DNDEBUG -o mini_script.exe mini_script.c
```

## Cross-Platform Notes

### Windows (Primary)
- Fully supported with automated build system
- Visual Studio 2022 recommended for best compatibility
- MinGW-w64 as alternative compiler option

### Linux/Unix (Compatible)
- Standard compilation: `gcc -Wall -Wextra -std=c17 -O2 -o mini_script mini_script.c`
- All core features supported
- File and time operations work correctly

### macOS (Should work)
- Standard compilation with Clang or GCC
- Most features should work correctly
- May need minor adjustments for time functions

## Performance Notes

### Optimized Builds
- Use `-O2` for good performance with reasonable compile time
- Use `-O3` for maximum optimization (longer compile time)
- Use `-Os` for size-optimized builds

### Debug Builds
- Use `-g` for debugging symbols
- Use `-DDEBUG` for additional debug output
- Disable optimizations (`-O0`) for easier debugging

## Integration with IDEs

### Visual Studio Code
- Install C/C++ extension
- Configure build task to use `build.bat`
- Set up debugging with generated executable

### Visual Studio Community
- Open folder containing project
- Use CMake or manual project setup
- Build and debug directly in IDE

### Code::Blocks
- Create new project from existing source
- Configure compiler settings
- Build and run from IDE

## Summary

The Mini Script Language is designed to build easily on Windows with the automated `build.bat` script. For most users, simply running the build script with Visual Studio 2022 installed is sufficient. Manual compilation options are available for custom setups or alternative compilers.

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
