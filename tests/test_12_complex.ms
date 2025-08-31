// Test 12: Complex Expressions  
print("=== Test 12: Complex Expressions ===");

// Arithmetic with parentheses and order of operations
assert (5 + 3) * 2 == 16, "Order of operations (parens first) check";
assert 10 + 5 * 2 == 20, "Order of operations (multiplication first) check";
assert 10 / 2 * 5 == 25, "Order of operations (left to right) check";
assert (10 + (5 * 2)) / 4 - 1 == 4, "Complex arithmetic check";

// Boolean expressions
var a = 10;
var b = 5;
assert (a > b) && (b > 0) == true, "Complex boolean AND check";
assert (a < b) || (b > 0) == true, "Complex boolean OR check";
assert !((a > b) && (b > 0)) == false, "Complex boolean NOT check";

// Mixed operations with variables
var x = 8;
var y = 4;
var z = (x + y) / 3;
assert z == 4, "Mixed variable arithmetic check";

// String and number concatenation in expressions
var val = 5;
assert "Value is " + (val + 5) == "Value is 10", "String and number expression check";

print("Test 12: PASSED");
print("");
