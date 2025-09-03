// Example 17: Functions and modules
print("=== Example 17: Functions and modules ===");

// Define a simple function
function add(x, y) {
    return x + y;
}

// Define a more complex function
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

// Define a function with string operations
function greet(name) {
    return "Hello, " + name + "!";
}

// Test the functions
print("Testing add function:");
result1 = add(5, 3);
print("add(5, 3) =", result1);

print("");
print("Testing factorial function:");
result2 = factorial(5);
print("factorial(5) =", result2);

print("");
print("Testing greet function:");
result3 = greet("Alice");
print(result3);

print("");
print("Example 17 completed!");
