// Test 11: Built-in Functions
print("=== Test 11: Built-in Functions ===");

// print function (already used extensively)
print("Testing print function:");
print("Single argument");
print("Multiple", "arguments", "with", "different", "types");
print("Number:", 42);
print("Boolean:", true);

// len function with different types
print("Testing len function:");

// String lengths
str1 = "Hello";
str2 = "Mini Script Language";
str3 = "";

print("Length of '" + str1 + "':", len(str1));
print("Length of '" + str2 + "':", len(str2));
print("Length of empty string:", len(str3));

// List lengths
list1 = [1, 2, 3];
list2 = ["a", "b", "c", "d", "e"];
list3 = [];
mixed_list = [1, "hello", true, 3.14];

print("Length of [1, 2, 3]:", len(list1));
print("Length of 5-element list:", len(list2));
print("Length of empty list:", len(list3));
print("Length of mixed list:", len(mixed_list));

// len with variables
data = [10, 20, 30, 40];
data_length = len(data);
print("Variable list length:", data_length);

// Using len in expressions
total_elements = len(list1) + len(list2);
print("Combined length:", total_elements);

// Using len in conditions
if (len(data) > 3) {
    print("Data list has more than 3 elements");
}

// Using len in loops
print("Using len in loops:");
items = ["first", "second", "third"];
items_count = len(items);
print("Processing", items_count, "items");
for (i = 1; i <= items_count; i = i + 1) {
    print("Processing item", i);
}

// Edge cases
very_long_string = "This is a very long string to test the len function with a significant amount of text";
print("Very long string length:", len(very_long_string));

large_list = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
print("Large list length:", len(large_list));

print("Test 11: PASSED");
print("");
