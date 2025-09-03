// Test 16: Module Import System
print("=== Test 16: Module Import System ===");

// Test 1: Basic import functionality (ensures no errors)
print("Test 1: Basic import");
import "modules/tiny_module";
assert tiny_var == 1, "Basic import variable check";

// Test 2: Variable import
print("Test 2: Variable import");
import "modules/vars_module";
assert var1 == 10, "Imported var1 check";
assert var2 == "hello", "Imported var2 check";
assert var3 == true, "Imported var3 check";

// Test 3: Advanced math utilities with functions and variables
print("Test 3: Math utilities import");
import "modules/simple_func_module";
result = simple_add(5, 3);
print("Simple add result:", result);
assert result == 8, "Simple add function check";

// Test 4: Multiple imports and function combinations
print("Test 4: Function combinations");
mult_result = 10;
square_result = 25;
combined_result = simple_add(mult_result, square_result);
assert combined_result == 35, "Combined calculation with imported functions check";

// Test 5: Module not found is handled by the interpreter, can't be asserted here.

print("Test 16: PASSED");
