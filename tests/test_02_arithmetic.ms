// Test 2: Arithmetic Operators
print("=== Test 2: Arithmetic Operators ===");

a = 10;
b = 3;

// Basic arithmetic
addition = a + b;
subtraction = a - b;
multiplication = a * b;
division = a / b;

print("a =", a, ", b =", b);
print("Addition: a + b =", addition);
print("Subtraction: a - b =", subtraction);
print("Multiplication: a * b =", multiplication);
print("Division: a / b =", division);

// Order of operations
complex_expr = (a + b) * 2 - b / 2;
print("Complex: (a + b) * 2 - b / 2 =", complex_expr);

// Negative numbers
negative_result = -a + b;
print("Negative: -a + b =", negative_result);

// Zero operations
zero = 0;
zero_add = zero + 5;
zero_mult = zero * 10;
print("Zero operations:", zero_add, zero_mult);

print("Test 2: PASSED");
print("");
