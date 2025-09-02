<#
.SYNOPSIS
    Mini Script Test Suite - Run tests for multiple language implementations

.DESCRIPTION
    This script runs the Mini Script test suite against different implementations:
    - Rust implementation (default)
    - C implementation (-C flag)
    - Python implementation (-Python flag)

.PARAMETER Python
    Run tests using the Python implementation (src/py/mini_script.py)

.PARAMETER C
    Run tests using the C implementation (src/c/mini_script.exe)

.PARAMETER Release
    Use the release build of the Rust implementation (only applies to Rust mode)

.PARAMETER Tests
    Specify specific test names to run (without .ms extension)
    If not specified, all tests in the tests/ directory will be run

.EXAMPLE
    .\run_tests.ps1
    Run all tests with the default Rust implementation (debug build)

.EXAMPLE
    .\run_tests.ps1 -C
    Run all tests with the C implementation

.EXAMPLE
    .\run_tests.ps1 -Python -Tests test_01_basic_types,test_02_arithmetic
    Run specific tests with the Python implementation

.EXAMPLE
    .\run_tests.ps1 -Release
    Run all tests with the Rust implementation (release build)
#>

param(
    [switch]$Python,
    [switch]$C,
    [switch]$Release,
    [string[]]$Tests
)

$PSScriptRoot = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

# Determine which Rust binary to use (debug or release)
$BuildType = if ($Release) { "release" } else { "debug" }
$RustBinaryPath = Join-Path $PSScriptRoot "target\$BuildType\mini_script.exe"
$PythonPath = Join-Path $PSScriptRoot "src\py\mini_script.py"
$CBinaryPath = Join-Path $PSScriptRoot "src\c\mini_script.exe"

$Command = $RustBinaryPath
$InitialArgs = @()

# Check if the Rust binary exists, if not, build it
if (-not $Python -and -not $C -and -not (Test-Path $RustBinaryPath)) {
    Write-Host "Rust binary not found at $RustBinaryPath"
    Write-Host "Building Rust binary..."
    if ($Release) {
        $BuildResult = & cargo build --release
    } else {
        $BuildResult = & cargo build
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to build Rust binary" -ForegroundColor Red
        exit 1
    }
    
    if (-not (Test-Path $RustBinaryPath)) {
        Write-Host "Build completed but binary still not found at $RustBinaryPath" -ForegroundColor Red
        exit 1
    }
    Write-Host "Build completed successfully"
    Write-Host ""
}

if ($Python) {
    $Command = "python"
    $InitialArgs = @($PythonPath)
    Write-Host "Running in Python mode"
} elseif ($C) {
    # Check if C binary exists, if not, build it
    if (-not (Test-Path $CBinaryPath)) {
        Write-Host "C binary not found at $CBinaryPath"
        Write-Host "Building C binary..."
        $BuildResult = & .\build.ps1
        
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Failed to build C binary" -ForegroundColor Red
            exit 1
        }
        
        if (-not (Test-Path $CBinaryPath)) {
            Write-Host "Build completed but binary still not found at $CBinaryPath" -ForegroundColor Red
            exit 1
        }
        Write-Host "Build completed successfully"
        Write-Host ""
    }
    
    $Command = $CBinaryPath
    $InitialArgs = @()
    Write-Host "Running in C mode"
}

Write-Host "Mini Script Test Suite"
Write-Host "=================================================="
if ($Python) {
    Write-Host "Interpreter: python $PythonPath"
} elseif ($C) {
    Write-Host "Interpreter: $CBinaryPath (C implementation)"
} else {
    Write-Host "Interpreter: $RustBinaryPath ($BuildType build)"
}
Write-Host ""

$TestsPath = Join-Path $PSScriptRoot "tests"
$TestsToRun = $Tests

if ($TestsToRun.Count -eq 0) {
    # If no specific files are provided, find all test scripts
    $TestsToRun = (Get-ChildItem -Path $TestsPath -Filter "test_*.ms" | ForEach-Object { $_.BaseName })
}

$Passed = 0
$Failed = 0
$FailedTests = [System.Collections.Generic.List[string]]::new()

Write-Host "Running tests..."
Write-Host ""

foreach ($TestName in $TestsToRun) {
    $TestDesc = ($TestName -replace "test_\d+_", "" -replace "_", " ").Split(' ') | ForEach-Object { $_.Substring(0,1).ToUpper() + $_.Substring(1) } | Join-String -Separator " "
    $ScriptPath = Join-Path $TestsPath "$TestName.ms"

    if (-not (Test-Path $ScriptPath)) {
        Write-Host "Test script not found: $ScriptPath" -ForegroundColor Yellow
        continue
    }

    $CurrentArgs = $InitialArgs + $ScriptPath
    
    Write-Host "Running $TestName..."
    
    # Execute the script and capture all output streams.
    # We check the exit code to determine pass/fail.
    $Output = & $Command $CurrentArgs 2>&1
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  FAILED - $TestDesc" -ForegroundColor Red
        Write-Host "  Interpreter returned a non-zero exit code: $LASTEXITCODE"
        Write-Host "  Output:"
        Write-Host "$($Output | Out-String)" -ForegroundColor Gray
        $Failed++
        $FailedTests.Add($TestName)
    } else {
        Write-Host "  PASSED - $TestDesc" -ForegroundColor Green
        $Passed++
    }
}

# Clean up old .txt and .actual.txt files
Get-ChildItem -Path $TestsPath -Filter "*.txt" | Remove-Item -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "=================================================="
Write-Host "Test Results: $Passed passed, $Failed failed"

if ($Failed -gt 0) {
    Write-Host "Failed tests: $($FailedTests -join ', ')" -ForegroundColor Red
    Write-Host "Overall result: FAILED ($Failed test(s) failed)" -ForegroundColor Red
    exit 1
} else {
    Write-Host "Overall result: PASSED" -ForegroundColor Green
    exit 0
}
