// Test 3: String Operations
print("=== Test 3: String Operations ===");

str1 = "Hello";
str2 = "World";
space = " ";

// String concatenation
concatenated = str1 + space + str2;
print("Concatenation:", concatenated);

// String with numbers
name = "Alice";
age = 25;
mixed = name + " is " + age + " years old";
print("Mixed string:", mixed);

// Empty string concatenation
empty = "";
result = empty + "Not empty";
print("Empty concat:", result);

// Multiple concatenations
multi = "A" + "B" + "C" + "D";
print("Multiple concat:", multi);

// String length function
text = "Testing";
length = len(text);
print("Length of '" + text + "':", length);

// Length of empty string
empty_len = len("");
print("Length of empty string:", empty_len);

print("Test 3: PASSED");
print("");
