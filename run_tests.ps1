param(
    [switch]$Python,
    [string[]]$Tests
)

$PSScriptRoot = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

$ExePath = Join-Path $PSScriptRoot "mini_script.exe"
$PythonPath = Join-Path $PSScriptRoot "mini_script.py"

$Command = $ExePath
$InitialArgs = @()

if ($Python) {
    $Command = "python"
    $InitialArgs = @($PythonPath)
    Write-Host "Running in Python mode"
}

Write-Host "Mini Script Test Suite"
Write-Host "=================================================="
if ($Python) {
    Write-Host "Interpreter: python $PythonPath"
} else {
    Write-Host "Interpreter: $ExePath"
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
