// Test 11: Built-in Functions
print("=== Test 11: Built-in Functions ===");

// print function is implicitly tested by the fact that these tests run and print PASSED.

// len function with different types
print("Testing len function:");

// String lengths
assert len("Hello") == 5, "len of 'Hello' check";
assert len("Mini Script Language") == 20, "len of long string check";
assert len("") == 0, "len of empty string check";

// List lengths
var list1 = [1, 2, 3];
var list2 = ["a", "b", "c", "d", "e"];
var list3 = [];
var mixed_list = [1, "hello", true, 3.14];

assert len(list1) == 3, "len of [1, 2, 3] check";
assert len(list2) == 5, "len of 5-element list check";
assert len(list3) == 0, "len of empty list check";
assert len(mixed_list) == 4, "len of mixed list check";

// len with variables
var data = [10, 20, 30, 40];
var data_length = len(data);
assert data_length == 4, "len using a variable check";

// Using len in expressions
var total_elements = len(list1) + len(list2);
assert total_elements == 8, "len in expression check";

// Using len in conditions
var has_elements = false;
if (len(data) > 3) {
    has_elements = true;
}
assert has_elements == true, "len in condition check";

print("Test 11: PASSED");
