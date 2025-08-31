// Test 7: While Loops
print("=== Test 7: While Loops ===");

// Basic while loop
var i = 1;
var sum = 0;
while (i <= 5) {
    sum = sum + i;
    i = i + 1;
}
assert sum == 15, "Basic while loop sum check";
assert i == 6, "Basic while loop counter check";

// Countdown while loop
var j = 5;
var countdown_str = "";
while (j >= 1) {
    countdown_str = countdown_str + j;
    j = j - 1;
}
assert countdown_str == "54321", "Countdown while loop check";
assert j == 0, "Countdown counter check";

// While loop with condition
var power = 1;
var exponent = 0;
while (power <= 100) {
    power = power * 2;
    exponent = exponent + 1;
}
assert power == 128, "Power of 2 check";
assert exponent == 7, "Exponent check";

// While loop with string concatenation
var str = "";
var counter = 1;
while (counter <= 3) {
    str = str + "Step" + counter + " ";
    counter = counter + 1;
}
assert str == "Step1 Step2 Step3 ", "String building loop check";

// While loop with boolean condition
var continue_loop = true;
var loop_count = 0;
while (continue_loop) {
    loop_count = loop_count + 1;
    if (loop_count >= 3) {
        continue_loop = false;
    }
}
assert loop_count == 3, "Boolean condition loop check";

// Nested while loops
var outer = 1;
var total_sum = 0;
while (outer <= 3) {
    var inner = 1;
    while (inner <= 3) {
        total_sum = total_sum + (outer * inner);
        inner = inner + 1;
    }
    outer = outer + 1;
}
assert total_sum == 36, "Nested while loops sum check";

print("Test 7: PASSED");
print("");
