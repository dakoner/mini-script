// Test for assert statement
print("=== Test 21: Assert Statement ===");

// Test 1: Successful assert with equality
var a = 10;
var b = 10;
assert a == b, "a should be equal to b";

// Test 2: Successful assert with inequality
var x = 5;
var y = 10;
assert x < y, "x should be less than y";

// Test 3: Successful assert with boolean true
var success = true;
assert success, "success should be true";

// Test 4: Successful assert with a complex expression
assert (1 + 2 * 3) == 7, "Complex expression in assert should evaluate correctly";

print("Test 21: PASSED");
