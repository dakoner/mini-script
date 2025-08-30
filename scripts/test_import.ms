// test_import.ms - Test the import functionality

print("Testing import functionality...");

// Import the math utilities module
import "scripts/math_utils.ms";

print("Module imported successfully!");

// Test the imported functions
result1 = add(5, 3);
print("5 + 3 = ");
print(result1);

result2 = multiply(4, 6);
print("4 * 6 = ");
print(result2);

result3 = square(7);
print("7^2 = ");
print(result3);

// Test module variables
print("Module name: ");
print(module_name);
print("Module version: ");
print(module_version);

print("Import test completed!");
