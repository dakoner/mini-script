// Test 8: For Loops
print("=== Test 8: For Loops ===");

// Basic for loop
print("Basic for loop (1 to 5):");
for (i = 1; i <= 5; i = i + 1) {
    print("Iteration:", i);
}

// For loop counting down
print("Counting down (10 to 6):");
for (j = 10; j >= 6; j = j - 1) {
    print("Value:", j);
}

// For loop with step of 2
print("Counting by 2s (0 to 10):");
for (k = 0; k <= 10; k = k + 2) {
    print("Even number:", k);
}

// For loop with calculations
print("Squares of numbers 1-5:");
for (n = 1; n <= 5; n = n + 1) {
    square = n * n;
    print("Square of", n, "=", square);
}

// For loop building a string
print("Building simple string:");
result_str = "";
for (letter_num = 1; letter_num <= 3; letter_num = letter_num + 1) {
    result_str = result_str + "A";
}
print("Result:", result_str);

// Nested for loops
print("Nested for loops (2x3 grid):");
for (row = 1; row <= 2; row = row + 1) {
    for (col = 1; col <= 3; col = col + 1) {
        print("Position", row, col);
    }
}

// For loop with complex condition
print("For loop with complex increment:");
for (x = 2; x <= 20; x = x * 2) {
    print("Power of 2:", x);
}

// For loop with boolean variables
print("For loop with counter:");
for (flag_count = 1; flag_count <= 3; flag_count = flag_count + 1) {
    if (flag_count == 1) {
        print("Count", flag_count, "is first");
    } else if (flag_count == 2) {
        print("Count", flag_count, "is second");
    } else {
        print("Count", flag_count, "is third");
    }
}

print("Test 8: PASSED");
print("");
