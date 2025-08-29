#!/usr/bin/env python3
"""
Mini Script Test Runner
Executes all test files and reports results
"""

import os
import subprocess
import sys
from pathlib import Path

def run_test(test_file, interpreter_path):
    """Run a single test file and return success/failure"""
    try:
        result = subprocess.run(
            [interpreter_path, test_file], 
            capture_output=True, 
            text=True, 
            timeout=30
        )
        
        # Check if test passed (look for "PASSED" in output)
        if "PASSED" in result.stdout:
            return True, result.stdout, result.stderr
        else:
            return False, result.stdout, result.stderr
            
    except subprocess.TimeoutExpired:
        return False, "", "Test timed out after 30 seconds"
    except Exception as e:
        return False, "", f"Error running test: {str(e)}"

def main():
    # Get script directory
    script_dir = Path(__file__).parent
    project_dir = script_dir.parent
    
    # Paths
    interpreter_path = project_dir / "mini_script.exe"
    tests_dir = script_dir
    
    if not interpreter_path.exists():
        print(f"Error: Interpreter not found at {interpreter_path}")
        print("Please build the interpreter first using build.bat")
        sys.exit(1)
    
    # Find all test files
    test_files = sorted(tests_dir.glob("test_*.ms"))
    
    if not test_files:
        print("No test files found in tests directory")
        sys.exit(1)
    
    print("Mini Script Test Suite")
    print("=" * 50)
    print(f"Interpreter: {interpreter_path}")
    print(f"Tests directory: {tests_dir}")
    print(f"Found {len(test_files)} test files")
    print()
    
    passed = 0
    failed = 0
    
    for test_file in test_files:
        test_name = test_file.stem
        print(f"Running {test_name}...", end=" ")
        
        success, stdout, stderr = run_test(str(test_file), str(interpreter_path))
        
        if success:
            print("PASSED")
            passed += 1
        else:
            print("FAILED")
            failed += 1
            print(f"  Output: {stdout[:200]}...")
            if stderr:
                print(f"  Error: {stderr}")
    
    print()
    print("=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print(f"Overall result: FAILED ({failed} test(s) failed)")
        sys.exit(1)
    else:
        print("Overall result: ALL TESTS PASSED")
        sys.exit(0)

if __name__ == "__main__":
    main()
