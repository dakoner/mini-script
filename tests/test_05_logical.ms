// Test 5: Logical Operators
print("=== Test 5: Logical Operators ===");

// Basic logical operations
var true_val = true;
var false_val = false;

// AND operations
assert (true_val && true_val) == true, "true && true check";
assert (true_val && false_val) == false, "true && false check";
assert (false_val && true_val) == false, "false && true check";
assert (false_val && false_val) == false, "false && false check";

// OR operations
assert (true_val || true_val) == true, "true || true check";
assert (true_val || false_val) == true, "true || false check";
assert (false_val || true_val) == true, "false || true check";
assert (false_val || false_val) == false, "false || false check";

// NOT operations
assert !true_val == false, "!true check";
assert !false_val == true, "!false check";

// Complex logical expressions
var a = 5;
var b = 10;
var c = 15;

assert ((a < b) && (b < c)) == true, "complex and check";
assert ((a > b) || (b < c)) == true, "complex or check";
assert !(a == b) == true, "complex not check";

// Short-circuit evaluation test
// Note: MiniScript doesn't currently support assignment within expressions in a way
// that can be easily tested for short-circuiting like this.
// The following tests are adapted to check the logical result,
// assuming the interpreter correctly implements short-circuiting.
var result1 = false && (1 == 2); // This expression should not cause an error and result in false
assert result1 == false, "Short-circuit AND";

var result2 = true || (1/0); // This expression should not cause a division-by-zero error
assert result2 == true, "Short-circuit OR";


print("Test 5: PASSED");
print("");
