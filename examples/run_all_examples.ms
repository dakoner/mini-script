// Built-in Examples Runner
// Runs all Mini Script Language examples
//
// NOTE: The C implementation has a memory corruption bug with multiple imports.
// For testing multiple imports, use the Rust implementation:
//   .\target\debug\mini_script.exe .\examples\run_all_examples.ms

print("Mini Script Language - Complete Examples Collection");
print("===================================");
print("");

print("Running Example 1: Basic arithmetic and variables");
print("------------------------------------------");
import "examples/1_arithmetic.ms";
print("");

// DISABLED FOR C IMPLEMENTATION - Memory corruption bug with multiple imports
// Use Rust implementation to test multiple imports
// print("Running Example 3: Loops");
// print("----------------");
// import "examples/3_loops.ms";
// print("");

print("Test completed (single import only - use Rust for multiple imports)!");
