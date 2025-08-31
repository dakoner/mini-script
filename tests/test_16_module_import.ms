// Test 16: Module Import System
print("=== Test 16: Module Import System ===");

// Test 1: Basic import functionality (ensures no errors)
print("Test 1: Basic import");
import "scripts/tiny_module";
assert tiny_var == 1, "Basic import variable check";

// Test 2: Variable import
print("Test 2: Variable import");
import "scripts/vars_module";
assert var1 == 10, "Imported var1 check";
assert var2 == "hello", "Imported var2 check";
assert var3 == true, "Imported var3 check";

// Test 3: Advanced math utilities with functions and variables
print("Test 3: Math utilities import");
import "scripts/math_utils";
assert add(5, 3) == 8, "Imported add function check";
assert multiply(4, 6) == 24, "Imported multiply function check";
assert square(7) == 49, "Imported square function check";
assert module_name == "Math Utilities", "Imported module_name check";
assert module_version == "1.0", "Imported module_version check";

// Test 4: Multiple imports and function combinations
print("Test 4: Function combinations");
var mult_result = multiply(4, 6);
var square_result = square(7);
var combined_result = add(mult_result, square_result);
assert combined_result == 73, "Combined calculation with imported functions check";

// Test 5: Module not found is handled by the interpreter, can't be asserted here.

print("Test 16: PASSED");
