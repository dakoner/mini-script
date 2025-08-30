#!/bin/bash
# Unix test runner script - runs test files sequentially

echo "Running Mini Script test suite..."
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
    
    if ./mini_script "$test_file" > /dev/null 2>&1; then
        echo "✓ PASSED"
        ((passed_tests++))
    else
        echo "✗ FAILED"
        echo "  Error running: ./mini_script $test_file"
        ((failed_tests++))
    fi
    ((total_tests++))
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
    exit 1
fi
