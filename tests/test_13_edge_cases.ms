// Test 13: Edge Cases
print("=== Test 13: Edge Cases ===");

// Zero handling
var zero = 0;
assert 5 + zero == 5, "Addition with zero check";
assert 5 * zero == 0, "Multiplication with zero check";
// Division by zero is expected to error, so we don't test it here.

// Empty values
var empty_str = "";
var empty_list = [];
assert len(empty_str) == 0, "Empty string length edge case";
assert len(empty_list) == 0, "Empty list length edge case";
assert empty_str + "a" == "a", "Empty string concatenation edge case";

// Boolean edge cases
var true_cond = false;
if (true) {
    true_cond = true;
}
assert true_cond == true, "If(true) block execution check";

var not_false_cond = false;
if (!false) {
    not_false_cond = true;
}
assert not_false_cond == true, "If(!false) block execution check";

// Large numbers
var big_num = 999999999;
assert big_num + 1 == 1000000000, "Large number arithmetic check";

// Whitespace handling
var x   =   10  ;
assert x == 10, "Whitespace handling in assignment";

print("Test 13: PASSED");
