// Test 10: User-Defined Functions
print("=== Test 10: User-Defined Functions ===");

// Simple function with no parameters
function say_hello() {
    return "Hello";
}
assert say_hello() == "Hello", "Simple function return check";

// Function with one parameter
function greet(name) {
    return "Hello, " + name;
}
assert greet("Alice") == "Hello, Alice", "Function with one parameter check";

// Function with multiple parameters
function add(a, b) {
    return a + b;
}
assert add(5, 3) == 8, "Function with multiple parameters check";

// Function with a return statement in the middle
function check_age(age) {
    if (age >= 18) {
        return "Adult";
    }
    return "Minor";
}
assert check_age(20) == "Adult", "Function with early return (adult) check";
assert check_age(16) == "Minor", "Function with early return (minor) check";

// Function that calls another function
function double_add(x, y) {
    return add(x, y) * 2;
}
assert double_add(2, 3) == 10, "Function calling another function check";

// Recursive function
function factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
assert factorial(5) == 120, "Recursive function check";

print("Test 10: PASSED");
