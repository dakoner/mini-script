// Test 8: For Loops
print("=== Test 8: For Loops ===");

// Basic for loop
var sum = 0;
for (var i = 1; i <= 5; i = i + 1) {
    sum = sum + i;
}
assert sum == 15, "Basic for loop sum check";

// For loop counting down
var countdown_str = "";
for (var j = 5; j >= 1; j = j - 1) {
    countdown_str = countdown_str + j;
}
assert countdown_str == "54321", "Countdown for loop check";

// For loop with step of 2
var even_sum = 0;
for (var k = 0; k <= 10; k = k + 2) {
    even_sum = even_sum + k;
}
assert even_sum == 30, "For loop with step 2 sum check";

// For loop with calculations
var squares_sum = 0;
for (var n = 1; n <= 5; n = n + 1) {
    squares_sum = squares_sum + (n * n);
}
assert squares_sum == 55, "For loop with calculations check";

// For loop building a string
var result_str = "";
for (var letter_num = 1; letter_num <= 3; letter_num = letter_num + 1) {
    result_str = result_str + "A";
}
assert result_str == "AAA", "For loop string building check";

// Nested for loops
var nested_sum = 0;
for (var row = 1; row <= 2; row = row + 1) {
    for (var col = 1; col <= 3; col = col + 1) {
        nested_sum = nested_sum + 1;
    }
}
assert nested_sum == 6, "Nested for loops iteration count check";

// For loop with complex condition
var power_val = 0;
for (var x = 2; x <= 20; x = x * 2) {
    power_val = x;
}
assert power_val == 16, "For loop with complex increment check";

print("Test 8: PASSED");
print("");
