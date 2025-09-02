#!/bin/bash
# Unified test runner for Mini Script (C + Python implementations)
set -euo pipefail

VERBOSE=false
USE_PYTHON=false
INTERPRETER=""
TIMEOUT=30

usage() {
    cat <<EOF
Usage: $0 [options]
    -v, --verbose      Show interpreter output for each test
            --python       Use Python implementation (auto-detect path)
    -t, --timeout N    Per-test timeout seconds (default: $TIMEOUT)
    -h, --help         Show this help
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -v|--verbose) VERBOSE=true; shift ;;
    --python) USE_PYTHON=true; shift ;;
        -t|--timeout) TIMEOUT="$2"; shift 2 ;;
        -h|--help) usage; exit 0 ;;
        *) echo "Unknown option: $1"; usage; exit 1 ;;
    esac
done

detect_interpreter() {
    if $USE_PYTHON; then
        if [[ -f src/py/mini_script.py ]]; then
            INTERPRETER="python src/py/mini_script.py"
        elif [[ -f mini_script.py ]]; then
            INTERPRETER="python mini_script.py"
        else
            echo "Python implementation not found (expected src/py/mini_script.py)" >&2
            exit 1
        fi
    else
        # Prefer new src/c build
        if [[ -x src/c/mini_script ]]; then
            INTERPRETER="src/c/mini_script"
        elif [[ -x build/debug/mini_script ]]; then
            INTERPRETER="build/debug/mini_script"
        elif [[ -x ./mini_script ]]; then
            INTERPRETER="./mini_script"
        else
            echo "C interpreter binary not found. Tried: src/c/mini_script, build/debug/mini_script, ./mini_script" >&2
            echo "Build it manually first, e.g.: (cd src/c && make)" >&2
            exit 1
        fi
    fi
}

detect_interpreter

echo "Running Mini Script test suite..."
if $USE_PYTHON; then
    echo "(Python interpreter: $INTERPRETER)"
else
    echo "(C interpreter: $INTERPRETER)"
fi
$VERBOSE && echo "(Verbose mode)"
echo "=================================="

total_tests=0
passed_tests=0
failed_tests=0

run_test() {
    local test_file="$1"
    local name
    name=$(basename "$test_file")
    printf 'Running %s... ' "$name"
    local cmd="timeout $TIMEOUT $INTERPRETER \"$test_file\""
    if $VERBOSE; then
        if eval $cmd; then
            echo "✓ PASSED"
            passed_tests=$((passed_tests+1))
        else
            handle_failure $? "$test_file"
        fi
    else
        if eval $cmd > /dev/null 2>&1; then
            echo "✓ PASSED"
            passed_tests=$((passed_tests+1))
        else
            handle_failure $? "$test_file"
        fi
    fi
    total_tests=$((total_tests+1))
}

handle_failure() {
    local code=$1 file=$2
    case $code in
        124) echo "✗ TIMEOUT (${TIMEOUT}s)" ;;
        139) echo "✗ SEGFAULT" ;;
        137) echo "✗ KILLED" ;;
        *)   echo "✗ FAILED (exit $code)" ;;
    esac
    $VERBOSE || echo "  Re-run with -v to inspect: $INTERPRETER $file"
    failed_tests=$((failed_tests+1))
}

shopt -s nullglob
test_files=(tests/test_*.ms)
if [[ ${#test_files[@]} -eq 0 ]]; then
    echo "No tests found (pattern tests/test_*.ms)" >&2
    exit 1
fi
for f in "${test_files[@]}"; do
    run_test "$f"
done

echo "=================================="
echo "Test Summary:"; printf '  Total:  %d\n  Passed: %d\n  Failed: %d\n' $total_tests $passed_tests $failed_tests
if [[ $failed_tests -eq 0 ]]; then
    echo "✓ All tests passed!"; exit 0
else
    echo "✗ $failed_tests test(s) failed"; exit 1
fi
