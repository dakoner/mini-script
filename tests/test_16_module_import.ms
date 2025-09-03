// Test 16: Module Import System
print("=== Test 16: Module Import System ===");

// Test 1: Basic import functionality (ensures no errors)
print("Test 1: Basic import");
import "lib/tiny_module";
assert tiny_var == 1, "Basic import variable check";

// Test 2: Variable import
print("Test 2: Variable import");
import "lib/vars_module";
assert var1 == 10, "Imported var1 check";
assert var2 == "hello", "Imported var2 check";
assert var3 == true, "Imported var3 check";

// Test 3: Testing config module
print("Test 3: Testing config module");
import "lib/config";
print("Config module imported successfully");
assert app_name == "Mini Script App", "Config app_name check";
assert app_version == "1.0.0", "Config app_version check";

// Test 4: Module imports completed
print("Test 4: Module imports completed");
print("All basic module imports successful");

// Test 5: Module not found is handled by the interpreter, can't be asserted here.

print("Test 16: PASSED");
