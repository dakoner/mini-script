// Test 5: Logical Operators
print("=== Test 5: Logical Operators ===");

// Basic logical operations
true_val = true;
false_val = false;

print("true_val =", true_val, ", false_val =", false_val);

// AND operations
print("true && true:", true_val && true_val);
print("true && false:", true_val && false_val);
print("false && true:", false_val && true_val);
print("false && false:", false_val && false_val);

// OR operations
print("true || true:", true_val || true_val);
print("true || false:", true_val || false_val);
print("false || true:", false_val || true_val);
print("false || false:", false_val || false_val);

// NOT operations
print("!true:", !true_val);
print("!false:", !false_val);

// Complex logical expressions
a = 5;
b = 10;
c = 15;

complex1 = (a < b) && (b < c);
complex2 = (a > b) || (b < c);
complex3 = !(a == b);

print("Complex expressions:");
print("(a < b) && (b < c):", complex1);
print("(a > b) || (b < c):", complex2);
print("!(a == b):", complex3);

// Short-circuit evaluation test
print("Testing logical with side effects:");
result1 = false && (a = 99);  // a should not be changed
print("After false && (a = 99), a =", a);

result2 = true || (b = 99);   // b should not be changed
print("After true || (b = 99), b =", b);

print("Test 5: PASSED");
print("");
