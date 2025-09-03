// Example 19: Advanced module system and paths
print("=== Example 19: Advanced module system and paths ===");

// This example demonstrates module path resolution and importing
print("Testing module import capabilities...");

// Import from modules/ directory if available
print("Attempting to import from modules directory...");

// Note: This example shows how module paths work
// In a real scenario, you would import actual modules like:
// import "modules/config.ms";
// import "modules/util_module.ms";

print("Module path resolution follows these rules:");
print("1. Look in current directory");
print("2. Look in modules/ directory");
print("3. Look in lib/ directory");
print("4. Use MODULESPATH environment variable if set");

print("");
print("Best practices for module organization:");
print("- Put configuration in modules/config.ms");
print("- Put utilities in modules/util_module.ms");
print("- Put libraries in lib/ directory");
print("- Use descriptive module names");

print("");
print("Example 19 completed!");
