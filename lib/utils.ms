// utils.ms - Utility functions and constants
print("Loading utilities...");

// Constants
pi = 3.14159;
max_value = 1000;

// Utility functions
function power(base, exp) { 
    result = 1; 
    i = 0; 
    while (i < exp) { 
        result = result * base; 
        i = i + 1; 
    } 
    return result; 
}

print("Utilities loaded!");
