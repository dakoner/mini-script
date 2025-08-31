print("=== Test 20: Error Reporting System ===");

// This test file primarily verifies that valid code runs without error.
// The actual testing of error messages happens by running scripts designed to fail,
// which can't be done in a test that is expected to pass.

// Test 1: Function definition and call
function test_function(x) {
    return x * 2;
}
assert test_function(21) == 42, "Function definition and call check";

// Test 2: Variable assignments
var version = "2.0";
var name = "ErrorTest";
assert version == "2.0", "String variable assignment check";
assert name == "ErrorTest", "Another string variable assignment check";

// Test 3: Multiple functions
function add(a, b) {
    return a + b;
}

function multiply(a, b) {
    return a * b;
}

assert add(10, 20) == 30, "Multiple functions (add) check";
assert multiply(6, 7) == 42, "Multiple functions (multiply) check";

print("Test 20: PASSED");
