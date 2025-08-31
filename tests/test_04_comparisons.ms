// Test 4: Comparison Operators
print("=== Test 4: Comparison Operators ===");

var a = 10;
var b = 5;
var c = 10;

// Equal and not equal
assert (a == b) == false, "a == b check";
assert (a == c) == true, "a == c check";
assert (a != b) == true, "a != b check";
assert (a != c) == false, "a != c check";

// Less than and greater than
assert (a > b) == true, "a > b check";
assert (a < b) == false, "a < b check";
assert (b < a) == true, "b < a check";

// Less than or equal, greater than or equal
assert (a >= c) == true, "a >= c check";
assert (b <= a) == true, "b <= a check";
assert (a <= b) == false, "a <= b check";

// String comparisons
var str1 = "apple";
var str2 = "banana";
var str3 = "apple";

assert (str1 == str2) == false, "str1 == str2 check";
assert (str1 == str3) == true, "str1 == str3 check";
assert (str1 != str2) == true, "str1 != str2 check";

// Boolean comparisons
var bool1 = true;
var bool2 = false;
var bool3 = true;

assert (bool1 == bool2) == false, "bool1 == bool2 check";
assert (bool1 == bool3) == true, "bool1 == bool3 check";
assert (bool1 != bool2) == true, "bool1 != bool2 check";

print("Test 4: PASSED");
