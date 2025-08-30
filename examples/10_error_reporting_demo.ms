// Error Reporting Demonstration
// This script demonstrates the enhanced error reporting with filename

print("=== Error Reporting Demo ===");
print("");

print("1. Testing basic syntax error:");
print("   The following line has an undefined variable...");

// This will cause an error and show filename + line number
result = undefined_variable_name + 42;

print("This line will not be reached due to the error above.");
