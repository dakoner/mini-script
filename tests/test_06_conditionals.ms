// Test 6: Conditional Statements (if/else)
print("=== Test 6: Conditional Statements ===");

var result = "";

// Simple if statement
var age = 18;
if (age >= 18) {
    result = "adult";
}
assert result == "adult", "Simple if check";

// If-else statement
var score = 85;
if (score >= 90) {
    result = "Grade A";
} else {
    result = "Grade B or below";
}
assert result == "Grade B or below", "If-else check";

// If-else if-else chain
var temperature = 75;
var weather = "";
if (temperature > 90) {
    weather = "very hot";
} else if (temperature > 70) {
    weather = "warm";
} else if (temperature > 50) {
    weather = "cool";
} else {
    weather = "cold";
}
assert weather == "warm", "If-else if-else chain check";

// Nested if statements
var x = 10;
var y = 5;
var quadrant = "";
if (x > 0) {
    if (y > 0) {
        quadrant = "Both x and y are positive";
    } else {
        quadrant = "x is positive, y is not";
    }
} else {
    quadrant = "x is not positive";
}
assert quadrant == "Both x and y are positive", "Nested if check";

// Complex conditions
var username = "admin";
var password = "secret";
var login_status = "";
if (username == "admin" && password == "secret") {
    login_status = "Login successful";
} else {
    login_status = "Login failed";
}
assert login_status == "Login successful", "Complex condition check";

// Boolean variable in condition
var is_weekend = true;
var activity = "";
if (is_weekend) {
    activity = "Enjoy your weekend!";
} else {
    activity = "Back to work!";
}
assert activity == "Enjoy your weekend!", "Boolean condition check";

// Comparison in condition
var num1 = 15;
var num2 = 10;
var comparison_result = "";
if (num1 != num2) {
    comparison_result = "Numbers are different";
}
assert comparison_result == "Numbers are different", "Comparison in condition check";

print("Test 6: PASSED");
print("");
