// Test 6: Conditional Statements (if/else)
print("=== Test 6: Conditional Statements ===");

// Simple if statement
age = 18;
if (age >= 18) {
    print("You are an adult");
}

// If-else statement
score = 85;
if (score >= 90) {
    print("Grade: A");
} else {
    print("Grade: B or below");
}

// If-else if-else chain
temperature = 75;
if (temperature > 90) {
    print("It's very hot");
} else if (temperature > 70) {
    print("It's warm");
} else if (temperature > 50) {
    print("It's cool");
} else {
    print("It's cold");
}

// Nested if statements
x = 10;
y = 5;
if (x > 0) {
    if (y > 0) {
        print("Both x and y are positive");
    } else {
        print("x is positive, y is not");
    }
} else {
    print("x is not positive");
}

// Complex conditions
username = "admin";
password = "secret";
if (username == "admin" && password == "secret") {
    print("Login successful");
} else {
    print("Login failed");
}

// Boolean variable in condition
is_weekend = true;
if (is_weekend) {
    print("Enjoy your weekend!");
} else {
    print("Back to work!");
}

// Comparison in condition
num1 = 15;
num2 = 10;
if (num1 != num2) {
    print("Numbers are different");
}

print("Test 6: PASSED");
print("");
