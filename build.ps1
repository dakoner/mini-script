# Mini Script Language Build Script
# PowerShell version with enhanced compiler detection and error handling

param(
    [switch]$Debug,
    [switch]$Verbose,
    [string]$Compiler = "gcc"
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Colors for output
function Write-Success { param($Message) Write-Host $Message -ForegroundColor Green }
function Write-Info { param($Message) Write-Host $Message -ForegroundColor Cyan }
function Write-Warning { param($Message) Write-Host $Message -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host $Message -ForegroundColor Red }

# Header
Write-Host "=" * 50 -ForegroundColor Blue
Write-Host "Mini Script Language Build System" -ForegroundColor Blue
Write-Host "PowerShell Build Script v2.0" -ForegroundColor Blue
Write-Host "=" * 50 -ForegroundColor Blue
Write-Host ""

# Verify source files exist in src/c directory
if (-not (Test-Path "src/c/main.c")) {
    Write-Error "Error: C source files not found in src/c directory"
    Write-Host "Please run this script from the mini-script project root directory."
    exit 1
}

# Change to C source directory
Write-Info "Changing to src/c directory..."
Push-Location "src/c"

# Compiler detection functions
function Find-VisualStudio {
    $vsPaths = @(
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
    
    foreach ($path in $vsPaths) {
        if (Test-Path $path) {
            return $path
        }
    }
    return $null
}

function Find-GCC {
    $gccPaths = @(
        "C:\msys64\ucrt64\bin\gcc.exe",
        "C:\msys64\mingw64\bin\gcc.exe",
        "C:\mingw64\bin\gcc.exe",
        "C:\TDM-GCC-64\bin\gcc.exe"
    )
    
    foreach ($path in $gccPaths) {
        if (Test-Path $path) {
            return $path
        }
    }
    
    # Check if gcc is in PATH
    try {
        gcc --version 2>$null | Out-Null
        if ($LASTEXITCODE -eq 0) {
            return "gcc"
        }
    } catch {
        # gcc not in PATH
    }
    
    return $null
}

function Find-Clang {
    try {
        clang --version 2>$null | Out-Null
        if ($LASTEXITCODE -eq 0) {
            return "clang"
        }
    } catch {
        # clang not in PATH
    }
    return $null
}

# Compilation functions
function Build-WithVisualStudio {
    param($VcvarsPath)
    
    Write-Info "Setting up Visual Studio 2022 environment..."
    
    # Create temporary batch file to run vcvars and compile
    $tempBat = [System.IO.Path]::GetTempFileName() + ".bat"
    
    $compilerFlags = "/W1 /std:c17"
    if ($Debug) {
        $compilerFlags += " /Zi /Od /D_DEBUG"
        Write-Info "Building DEBUG version with symbols..."
    } else {
        $compilerFlags += " /O2 /DNDEBUG"
        Write-Info "Building RELEASE version with optimizations..."
    }
    
    $batchContent = @"
@echo off
call "$VcvarsPath" >nul 2>&1
if errorlevel 1 (
    echo Failed to set up Visual Studio environment
    exit /b 1
)
cl $compilerFlags main.c lexer.c parser.c interpreter.c value.c environment.c /Fe:mini_script.exe /link /SUBSYSTEM:CONSOLE
exit /b %errorlevel%
"@
    
    Set-Content -Path $tempBat -Value $batchContent
    
    try {
        Write-Info "Compiling with Visual Studio compiler..."
        & cmd.exe /c $tempBat | Out-Null
        $exitCode = $LASTEXITCODE
        
        if ($exitCode -eq 0) {
            Write-Success "✓ Compilation successful with Visual Studio!"
            return $true
        } else {
            Write-Error "✗ Visual Studio compilation failed (exit code: $exitCode)"
            return $false
        }
    } finally {
        Remove-Item $tempBat -ErrorAction SilentlyContinue
    }
}

function Build-WithGCC {
    param($GccPath)
    
    Write-Info "Compiling with GCC..."
    
    $compilerFlags = "-Wall -Wextra -std=c17"
    if ($Debug) {
        $compilerFlags += " -g -O0 -DDEBUG"
        Write-Info "Building DEBUG version with symbols..."
    } else {
        $compilerFlags += " -O2 -DNDEBUG"
        Write-Info "Building RELEASE version with optimizations..."
    }
    
    $compileCommand = "$GccPath $compilerFlags -o mini_script.exe main.c lexer.c parser.c interpreter.c value.c environment.c"
    
    if ($Verbose) {
        Write-Info "Command: $compileCommand"
    }
    
    try {
        Invoke-Expression $compileCommand
        if ($LASTEXITCODE -eq 0) {
            Write-Success "✓ Compilation successful with GCC!"
            return $true
        } else {
            Write-Error "✗ GCC compilation failed (exit code: $LASTEXITCODE)"
            return $false
        }
    } catch {
        Write-Error "✗ GCC compilation failed: $($_.Exception.Message)"
        return $false
    }
}

function Build-WithClang {
    Write-Info "Compiling with Clang..."
    
    $compilerFlags = "-Wall -Wextra -std=c17"
    if ($Debug) {
        $compilerFlags += " -g -O0 -DDEBUG"
        Write-Info "Building DEBUG version with symbols..."
    } else {
        $compilerFlags += " -O2 -DNDEBUG"
        Write-Info "Building RELEASE version with optimizations..."
    }
    
    $compileCommand = "clang $compilerFlags -o mini_script.exe main.c lexer.c parser.c interpreter.c value.c environment.c"
    
    if ($Verbose) {
        Write-Info "Command: $compileCommand"
    }
    
    try {
        Invoke-Expression $compileCommand
        if ($LASTEXITCODE -eq 0) {
            Write-Success "✓ Compilation successful with Clang!"
            return $true
        } else {
            Write-Error "✗ Clang compilation failed (exit code: $LASTEXITCODE)"
            return $false
        }
    } catch {
        Write-Error "✗ Clang compilation failed: $($_.Exception.Message)"
        return $false
    }
}

# Main compilation logic
$compiled = $false

# Clean up previous build
if (Test-Path "mini_script.exe") {
    Write-Info "Removing previous build..."
    Remove-Item "mini_script.exe" -Force
}

# Determine which compiler to use
switch ($Compiler.ToLower()) {
    "vs" {
        $vsPath = Find-VisualStudio
        if ($vsPath) {
            $compiled = Build-WithVisualStudio $vsPath
        } else {
            Write-Error "Visual Studio 2022 not found!"
        }
    }
    "gcc" {
        $gccPath = Find-GCC
        if ($gccPath) {
            $compiled = Build-WithGCC $gccPath
        } else {
            Write-Error "GCC not found!"
        }
    }
    "clang" {
        $clangPath = Find-Clang
        if ($clangPath) {
            $compiled = Build-WithClang
        } else {
            Write-Error "Clang not found!"
        }
    }
    "auto" {
        # Try compilers in order of preference
        Write-Info "Auto-detecting available compilers..."
        
        # 1. Try Visual Studio first (recommended)
        $vsPath = Find-VisualStudio
        if ($vsPath) {
            Write-Info "Found Visual Studio 2022"
            $compiled = Build-WithVisualStudio $vsPath
        }
        
        # 2. Try GCC if VS failed or not found
        if (-not $compiled) {
            $gccPath = Find-GCC
            if ($gccPath) {
                Write-Info "Found GCC at: $gccPath"
                $compiled = Build-WithGCC $gccPath
            }
        }
        
        # 3. Try Clang as last resort
        if (-not $compiled) {
            $clangPath = Find-Clang
            if ($clangPath) {
                Write-Info "Found Clang"
                $compiled = Build-WithClang
            }
        }
    }
    default {
        Write-Error "Unknown compiler: $Compiler. Use 'vs', 'gcc', 'clang', or 'auto'"
        exit 1
    }
}

# Check compilation result
if (-not $compiled) {
    Pop-Location  # Return to original directory
    Write-Host ""
    Write-Error "No suitable compiler found or compilation failed!"
    Write-Host ""
    Write-Warning "Please install one of the following compilers:"
    Write-Host "  1. Visual Studio 2022 Community (recommended) - https://visualstudio.microsoft.com/vs/community/"
    Write-Host "  2. Visual Studio 2022 Build Tools - https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022"
    Write-Host "  3. MSYS2 with MinGW-w64 - https://www.msys2.org/"
    Write-Host "  4. TDM-GCC - https://jmeubank.github.io/tdm-gcc/"
    Write-Host "  5. LLVM Clang - https://clang.llvm.org/"
    Write-Host ""
    exit 1
}

# Verify executable was created
if (-not (Test-Path "mini_script.exe")) {
    Pop-Location  # Return to original directory
    Write-Error "Compilation reported success but mini_script.exe not found!"
    exit 1
}

# Get file info
$exeInfo = Get-ItemProperty "mini_script.exe"
$fileSize = [math]::Round($exeInfo.Length / 1KB, 1)

Write-Host ""
Write-Success "🎉 Build completed successfully!"
Write-Info "Executable: mini_script.exe ($fileSize KB)"
Write-Info "Build type: $(if ($Debug) { 'DEBUG' } else { 'RELEASE' })"

# Test the executable
Write-Host ""
Write-Info "Testing the executable..."
Write-Host "-" * 30

try {
    & .\mini_script.exe --help
    $testResult = $LASTEXITCODE
    
    Write-Host "-" * 30
    
    if ($testResult -eq 0) {
        Write-Success "✓ Executable test passed!"
    } else {
        Write-Warning "⚠ Executable test returned exit code: $testResult"
    }
} catch {
    Write-Warning "⚠ Could not test executable: $($_.Exception.Message)"
}

# Return to original directory
Pop-Location

Write-Host ""
Write-Success "Build process complete!"
Write-Host ""
Write-Info "Usage examples:"
Write-Host "  .\src\c\mini_script.exe script.ms     # Run a script file"
Write-Host "  .\src\c\mini_script.exe               # Start REPL mode"
Write-Host "  .\run_tests.ps1                       # Run test suite"
Write-Host ""
