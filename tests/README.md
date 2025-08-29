# Mini Script Test Suite

This directory contains a comprehensive test suite for the Mini Script language interpreter.

## Test Files

The test suite is organized into focused test files, each testing specific language features:

### Core Language Tests

1. **test_01_basic_types.ms** - Basic data types (numbers, strings, booleans)
2. **test_02_arithmetic.ms** - Arithmetic operators (+, -, *, /, order of operations)
3. **test_03_strings.ms** - String operations (concatenation, length)
4. **test_04_comparisons.ms** - Comparison operators (==, !=, <, >, <=, >=)
5. **test_05_logical.ms** - Logical operators (&&, ||, !, short-circuit evaluation)

### Control Flow Tests

6. **test_06_conditionals.ms** - If/else statements (simple, nested, complex conditions)
7. **test_07_while_loops.ms** - While loops (basic, countdown, nested)
8. **test_08_for_loops.ms** - For loops (basic, step increments, nested)

### Advanced Features Tests

9. **test_09_lists.ms** - Lists and arrays (creation, indexing, modification, mixed types)
10. **test_10_functions.ms** - User-defined functions (parameters, return values, recursion)
11. **test_11_builtin_functions.ms** - Built-in functions (print, len)

### Complex Scenarios Tests

12. **test_12_complex.ms** - Complex expressions, operator precedence, nested calls
13. **test_13_edge_cases.ms** - Edge cases, error conditions, boundary testing

## Running the Tests

### Windows Batch Script (Recommended)
```batch
cd tests
run_tests.bat
```

### Python Script (Cross-platform)
```bash
cd tests
python run_tests.py
```

### Manual Testing
Run individual tests manually:
```batch
..\mini_script.exe test_01_basic_types.ms
..\mini_script.exe test_02_arithmetic.ms
# ... etc
```

## Test Output

Each test file produces output showing:
- Test description and purpose
- Individual test cases with expected vs actual results
- "Test X: PASSED" message if all tests in the file succeed

## Expected Results

When all tests pass, you should see:
```
Mini Script Test Suite
==================================================
Running test_01_basic_types... PASSED
Running test_02_arithmetic... PASSED
Running test_03_strings... PASSED
...
Running test_13_edge_cases... PASSED

==================================================
Test Results: 13 passed, 0 failed
Overall result: ALL TESTS PASSED
```

## Test Coverage

The test suite covers:

### Language Features
- ✅ Variable declaration and assignment
- ✅ Integer and floating-point numbers
- ✅ String literals and concatenation
- ✅ Boolean values and operations
- ✅ Arithmetic operators with correct precedence
- ✅ Comparison operators for all data types
- ✅ Logical operators with short-circuit evaluation
- ✅ If/else conditional statements
- ✅ While loops with various conditions
- ✅ For loops with different increment patterns
- ✅ Lists/arrays with mixed data types
- ✅ List indexing and modification
- ✅ User-defined functions with parameters
- ✅ Function return values
- ✅ Recursive function calls
- ✅ Built-in functions (print, len)
- ✅ Complex nested expressions
- ✅ Variable scope and shadowing

### Edge Cases
- ✅ Empty strings and lists
- ✅ Zero values in arithmetic
- ✅ Single-element lists
- ✅ Functions with no return statements
- ✅ Nested data structures
- ✅ Large and small numeric values
- ✅ Boolean edge cases
- ✅ Limited recursion depth

### Error Handling
The tests verify that the interpreter correctly handles:
- Valid syntax variations
- Boundary conditions
- Complex expression parsing
- Proper operator precedence
- Correct function call semantics

## Adding New Tests

To add new tests:

1. Create a new test file: `test_XX_feature_name.ms`
2. Follow the existing test format:
   ```javascript
   // Test XX: Feature Description
   print("=== Test XX: Feature Description ===");
   
   // Test cases here...
   
   print("Test XX: PASSED");
   print("");
   ```
3. Add comprehensive test cases for the feature
4. Include edge cases and boundary conditions
5. Run the test suite to verify the new test works

## Test Philosophy

The test suite follows these principles:

- **Comprehensive Coverage**: Tests all major language features
- **Clear Output**: Each test explains what it's testing
- **Self-Validating**: Tests include their own success/failure indicators
- **Isolated**: Each test file can be run independently
- **Progressive**: Tests build from simple to complex features
- **Documented**: Clear descriptions of what each test validates

## Troubleshooting

If tests fail:

1. **Check interpreter build**: Ensure `mini_script.exe` exists and is current
2. **Review error output**: The test runner shows failure details
3. **Run individual tests**: Test specific features in isolation
4. **Check syntax**: Verify test files use supported Mini Script syntax
5. **Check paths**: Ensure relative paths to interpreter are correct

## Maintenance

The test suite should be updated when:
- New language features are added
- Bug fixes change behavior
- Edge cases are discovered
- Performance optimizations affect output

Keep tests current with the interpreter implementation to ensure reliable validation of the Mini Script language.
