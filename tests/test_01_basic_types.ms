// Test 1: Basic Variables and Data Types
print("=== Test 1: Basic Variables and Data Types ===");

// Integer
var my_int = 42;
assert my_int == 42, "Integer value check";

// Float
var my_float = 3.14159;
assert my_float == 3.14159, "Float value check";

// Negative Number
var my_neg = -17;
assert my_neg == -17, "Negative number check";

// String
var my_str = "Hello, World!";
assert my_str == "Hello, World!", "String value check";
var empty_str = "";
assert empty_str == "", "Empty string check";
var string_with_spaces = "   spaces   ";
assert string_with_spaces == "   spaces   ", "String with spaces check";

// Boolean
var my_bool_true = true;
assert my_bool_true == true, "Boolean true check";
var my_bool_false = false;
assert my_bool_false == false, "Boolean false check";

// Reassignment
var test_var = 10;
assert test_var == 10, "Initial assignment check";
test_var = 20;
assert test_var == 20, "Reassignment check";

print("Test 1: PASSED");
