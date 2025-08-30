# Build Scripts Documentation

The Mini Script Language project provides two build scripts for compiling the interpreter:

## PowerShell Script (Recommended): `build.ps1`

### Features
- **Enhanced compiler detection** - Automatically finds Visual Studio 2022, GCC, or Clang
- **Better error handling** - Clear error messages and troubleshooting guidance
- **Debug/Release builds** - Support for both optimized and debug builds
- **Verbose output** - Detailed compilation information when needed
- **Cross-platform ready** - Works on Windows PowerShell and PowerShell Core

### Usage

#### Basic Usage
```powershell
# Build with auto-detected compiler (recommended)
.\build.ps1

# Build debug version with symbols
.\build.ps1 -Debug

# Build with verbose output
.\build.ps1 -Verbose

# Force specific compiler
.\build.ps1 -Compiler vs      # Visual Studio
.\build.ps1 -Compiler gcc     # GCC/MinGW
.\build.ps1 -Compiler clang   # Clang
```

#### Parameters
- `-Debug` - Build debug version with symbols and no optimization
- `-Verbose` - Show detailed compilation commands
- `-Compiler <type>` - Force specific compiler: `vs`, `gcc`, `clang`, or `auto` (default)

#### Example Output
```
==================================================
Mini Script Language Build System
PowerShell Build Script v2.0
==================================================

Auto-detecting available compilers...
Found Visual Studio 2022
Setting up Visual Studio 2022 environment...
Building RELEASE version with optimizations...
Compiling with Visual Studio compiler...
✓ Compilation successful with Visual Studio!

🎉 Build completed successfully!
Executable: mini_script.exe (232.5 KB)
Build type: RELEASE
```

## Batch Script (Legacy): `build.bat`

### Features
- **Simple batch file** - Works on all Windows systems
- **Basic compiler detection** - Finds Visual Studio 2022 or MinGW
- **Backward compatibility** - Maintains compatibility with older scripts

### Usage
```cmd
# Build with auto-detected compiler
.\build.bat
```

The batch script will recommend using the PowerShell script for enhanced features.

## Compiler Support

### Visual Studio 2022 (Recommended)
- **Editions Supported**: Community, Professional, Enterprise, Build Tools
- **Detection**: Automatic detection of installation paths
- **Features**: Full C17 support, excellent optimization, integrated debugging

### GCC/MinGW
- **Versions Supported**: MSYS2, TDM-GCC, standalone MinGW-w64
- **Detection**: Checks common installation paths and PATH
- **Features**: Good C17 support, cross-platform compatibility

### Clang
- **Detection**: Checks PATH for clang executable
- **Features**: Modern C17 support, excellent error messages

## Installation Recommendations

### For New Users
1. **Install Visual Studio 2022 Community** (free)
   - Download: https://visualstudio.microsoft.com/vs/community/
   - Select "Desktop development with C++" workload
   - Use: `.\build.ps1` (auto-detects)

### For Developers
1. **Install Visual Studio 2022 Build Tools** (lightweight)
   - Download: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
   - Select "C++ build tools" workload
   - Use: `.\build.ps1` (auto-detects)

### For Cross-Platform Development
1. **Install MSYS2** (portable Unix environment)
   - Download: https://www.msys2.org/
   - Install GCC: `pacman -S mingw-w64-x86_64-gcc`
   - Use: `.\build.ps1 -Compiler gcc`

## Build Outputs

### Release Build (Default)
- **Optimization**: Full optimization (-O2)
- **Size**: ~230KB executable
- **Debug info**: None (faster execution)
- **Use case**: Production, distribution

### Debug Build
- **Optimization**: None (-O0)
- **Size**: Larger executable
- **Debug info**: Full symbols (-g or /Zi)
- **Use case**: Development, debugging

## Troubleshooting

### Common Issues

**"No suitable compiler found"**
- Install Visual Studio 2022 Community or Build Tools
- Or install MSYS2 with GCC
- Restart terminal after installation

**"PowerShell execution policy"**
- Run: `Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser`
- Or use: `powershell -ExecutionPolicy Bypass -File build.ps1`

**"Compilation failed"**
- Check that `mini_script.c` exists in current directory
- Try debug build: `.\build.ps1 -Debug -Verbose`
- Check compiler installation

**"vcvars64.bat not found"**
- Reinstall Visual Studio with C++ workload
- Use alternative compiler: `.\build.ps1 -Compiler gcc`

### Getting Help

**Check compiler versions:**
```powershell
# Check available compilers
gcc --version     # GCC
clang --version   # Clang
cl                # Visual Studio (after running vcvars64.bat)
```

**Verbose build output:**
```powershell
.\build.ps1 -Verbose -Debug
```

## Integration with IDEs

### Visual Studio Code
- Use PowerShell terminal: `Ctrl+Shift+`` 
- Run: `.\build.ps1`
- Configure tasks.json to use build script

### Visual Studio
- Use Developer PowerShell
- Run: `.\build.ps1 -Compiler vs`
- Or open folder and use integrated terminal

### Command Line
- Use any Windows terminal (CMD, PowerShell, Windows Terminal)
- Both scripts work from any terminal environment

## Performance Notes

### Compiler Performance
- **Visual Studio**: Best optimization, fastest execution
- **GCC**: Good optimization, portable code
- **Clang**: Excellent error messages, good optimization

### Build Time
- **Release builds**: ~2-3 seconds
- **Debug builds**: ~1-2 seconds  
- **Clean builds**: Automatic cleanup of previous builds

## Future Enhancements

- **CMake support**: Cross-platform build system
- **Package management**: Automatic dependency handling
- **CI/CD integration**: GitHub Actions and other platforms
- **Multi-target builds**: Different architectures and platforms
