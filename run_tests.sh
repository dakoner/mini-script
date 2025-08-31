#!/bin/bash
# Unix test runner script - runs test files sequentially

# Check for flags
VERBOSE=false
USE_PYTHON=false
INTERPRETER="./mini_script"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --python)
            USE_PYTHON=true
            INTERPRETER="python mini_script.py"
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  -v, --verbose    Show detailed output for all tests"
            echo "  --python         Use Python implementation instead of C implementation"
            echo "  -h, --help       Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

echo "Running Mini Script test suite..."
if [ "$USE_PYTHON" = true ]; then
    echo "(Using Python implementation: $INTERPRETER)"
else
    echo "(Using C implementation: $INTERPRETER)"
fi
if [ "$VERBOSE" = true ]; then
    echo "(Verbose mode enabled)"
fi
echo "=================================="

# Initialize counters
total_tests=0
passed_tests=0
failed_tests=0

# Check if the chosen interpreter exists and is executable
if [ "$USE_PYTHON" = true ]; then
    if ! command -v python &> /dev/null; then
        echo "Error: Python interpreter not found"
        echo "Please ensure Python is installed and in your PATH"
        exit 1
    fi
    if [ ! -f "mini_script.py" ]; then
        echo "Error: mini_script.py not found in current directory"
        exit 1
    fi
else
    if [ ! -f "./mini_script" ]; then
        echo "Error: ./mini_script not found"
        echo "Please build the C implementation first using 'make' or use --python flag"
        exit 1
    fi
    if [ ! -x "./mini_script" ]; then
        echo "Error: ./mini_script is not executable"
        echo "Please run 'chmod +x ./mini_script' or rebuild using 'make'"
        exit 1
    fi
fi

# Function to run a single test file
run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file")
    
    echo -n "Running $test_name... "
    
    # Use timeout to prevent hanging tests (30 second limit)
    if [ "$VERBOSE" = true ]; then
        # In verbose mode, show output
        if timeout 30 $INTERPRETER "$test_file"; then
            echo "✓ PASSED"
            ((passed_tests++))
        else
            local exit_code=$?
            handle_test_failure "$exit_code" "$test_file"
        fi
    else
        # In normal mode, suppress output
        if timeout 30 $INTERPRETER "$test_file" > /dev/null 2>&1; then
            echo "✓ PASSED"
            ((passed_tests++))
        else
            local exit_code=$?
            handle_test_failure "$exit_code" "$test_file"
        fi
    fi
    ((total_tests++))
}

# Function to handle test failures
handle_test_failure() {
    local exit_code=$1
    local test_file=$2
    
    if [ $exit_code -eq 124 ]; then
        echo "✗ TIMEOUT (>30s)"
        echo "  Test timed out after 30 seconds"
    elif [ $exit_code -eq 139 ]; then
        echo "✗ SEGFAULT"
        echo "  Segmentation fault detected"
    elif [ $exit_code -eq 137 ]; then
        echo "✗ KILLED"
        echo "  Process was killed (possibly out of memory)"
    else
        echo "✗ FAILED (exit code: $exit_code)"
        echo "  Error running: $INTERPRETER $test_file"
    fi
    ((failed_tests++))
}

# Run all test files in the tests directory
if [ -d "tests" ]; then
    for test_file in tests/test_*.ms; do
        if [ -f "$test_file" ]; then
            run_test "$test_file"
        fi
    done
else
    echo "Error: tests directory not found"
    exit 1
fi

# Print summary
echo "=================================="
echo "Test Summary:"
echo "  Total:  $total_tests"
echo "  Passed: $passed_tests"
echo "  Failed: $failed_tests"

if [ $failed_tests -eq 0 ]; then
    echo "✓ All tests passed!"
    exit 0
else
    echo "✗ $failed_tests test(s) failed"
    if [ "$VERBOSE" = false ]; then
        echo ""
        echo "Run with -v or --verbose to see detailed output for failed tests"
    fi
    exit 1
fi
