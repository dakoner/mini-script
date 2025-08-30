// Test 16: Module Import System
print("=== Test 16: Module Import System ===");

// Test 1: Basic import functionality
print("Test 1: Basic import");
import "scripts/tiny_module.ms";

// Test 2: Variable import
print("Test 2: Variable import");
import "scripts/vars_module.ms";
print("Imported var1:", var1);
print("Imported var2:", var2);
print("Imported var3:", var3);

// Test 3: Module not found (should show error)
print("Test 3: Module not found test");
// Uncomment the next line to test error handling:
// import "nonexistent_module.ms";

print("Module import tests completed successfully!");
print("===");
