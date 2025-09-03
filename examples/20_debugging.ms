// Example 20: Debugging and troubleshooting
print("=== Example 20: Debugging and troubleshooting ===");

// This example demonstrates debugging techniques and common issues

print("Debugging technique 1: Print statements");
function debugExample(value) {
    print("DEBUG: Entering function with value =", value);
    
    if (value < 0) {
        print("DEBUG: Value is negative");
        return 0;
    }
    
    result = value * 2;
    print("DEBUG: Calculated result =", result);
    return result;
}

print("Testing debug function:");
test1 = debugExample(5);
print("Final result:", test1);

print("");
print("Debugging technique 2: Step-by-step execution");
print("Breaking complex operations into steps:");

step1 = 10;
print("Step 1: Initial value =", step1);

step2 = step1 + 5;
print("Step 2: After addition =", step2);

step3 = step2 * 2;
print("Step 3: After multiplication =", step3);

print("");
print("Debugging technique 3: Validate inputs");
function safeDivide(a, b) {
    print("DEBUG: Attempting to divide", a, "by", b);
    
    if (b == 0) {
        print("ERROR: Division by zero!");
        return 0;
    }
    
    return a / b;
}

print("Testing safe division:");
result1 = safeDivide(10, 2);
print("10 / 2 =", result1);

result2 = safeDivide(10, 0);
print("10 / 0 =", result2);

print("");
print("Example 20 completed!");
