#!/bin/bash
# Unix test runner script - runs test files sequentially

# Check for verbose flag
VERBOSE=false
if [ "$1" = "-v" ] || [ "$1" = "--verbose" ]; then
    VERBOSE=true
    shift
fi

echo "Running Mini Script test suite..."
if [ "$VERBOSE" = true ]; then
    echo "(Verbose mode enabled)"
fi
echo "=================================="

# Initialize counters
total_tests=0
passed_tests=0
failed_tests=0

# Function to run a single test file
run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file")
    
    echo -n "Running $test_name... "
    
    # Use timeout to prevent hanging tests (30 second limit)
    if [ "$VERBOSE" = true ]; then
        # In verbose mode, show output
        if timeout 30 ./mini_script "$test_file"; then
            echo "✓ PASSED"
            ((passed_tests++))
        else
            local exit_code=$?
            handle_test_failure "$exit_code" "$test_file"
        fi
    else
        # In normal mode, suppress output
        if timeout 30 ./mini_script "$test_file" > /dev/null 2>&1; then
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
        echo "  Error running: ./mini_script $test_file"
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
