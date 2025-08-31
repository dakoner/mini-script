// Test 2: Arithmetic Operators
print("=== Test 2: Arithmetic Operators ===");

var a = 10;
var b = 3;

// Basic arithmetic
assert a + b == 13, "Addition check";
assert a - b == 7, "Subtraction check";
assert a * b == 30, "Multiplication check";
assert a / b == 10/3, "Division check";

// Order of operations
assert (a + b) * 2 - b / 2 == 24.5, "Order of operations check";

// Negative numbers
assert -a + b == -7, "Negative number arithmetic check";

// Zero operations
var zero = 0;
assert zero + 5 == 5, "Zero addition check";
assert zero * 10 == 0, "Zero multiplication check";

print("Test 2: PASSED");

