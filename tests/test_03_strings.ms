// Test 3: String Operations
print("=== Test 3: String Operations ===");

var str1 = "Hello";
var str2 = "World";
var space = " ";

// String concatenation
assert str1 + space + str2 == "Hello World", "String concatenation check";

// String with numbers
var name = "Alice";
var age = 25;
assert name + " is " + age + " years old" == "Alice is 25 years old", "Mixed type concatenation check";

// Empty string concatenation
var empty = "";
assert empty + "Not empty" == "Not empty", "Empty string concatenation check";

// Multiple concatenations
assert "A" + "B" + "C" + "D" == "ABCD", "Multiple concatenation check";

// String length function
var text = "Testing";
assert len(text) == 7, "Length of 'Testing' check";

// Length of empty string
assert len("") == 0, "Length of empty string check";

print("Test 3: PASSED");
