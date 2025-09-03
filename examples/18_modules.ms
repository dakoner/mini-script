// Example 18: Module imports and organization
print("=== Example 18: Module imports and organization ===");

// This example demonstrates how to organize code into modules
print("Demonstrating module organization...");

// Check if we have existing modules
print("Looking for math module in modules/ directory...");

// For demonstration, we'll define our own utility functions
function calculateArea(width, height) {
    return width * height;
}

function calculatePerimeter(width, height) {
    return 2 * (width + height);
}

// Test our utility functions
print("");
print("Testing rectangle calculations:");
width = 5;
height = 8;
area = calculateArea(width, height);
perimeter = calculatePerimeter(width, height);

print("Rectangle:", width, "x", height);
print("Area:", area);
print("Perimeter:", perimeter);

print("");
print("Example 18 completed!");
