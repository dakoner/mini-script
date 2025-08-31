// Test 9: Basic Lists
print("=== Test 9: Basic Lists ===");

var list1 = [1, 2, 3];
assert list1[0] == 1, "List index 0 check";
assert list1[1] == 2, "List index 1 check";
assert list1[2] == 3, "List index 2 check";

var list2 = ["a", "b", "c"];
assert list2[0] == "a", "String list index 0 check";
assert list2[1] == "b", "String list index 1 check";
assert list2[2] == "c", "String list index 2 check";

var list3 = [true, false];
assert list3[0] == true, "Boolean list index 0 check";
assert list3[1] == false, "Boolean list index 1 check";

var mixed = [1, "hello", true];
assert mixed[0] == 1, "Mixed list index 0 check";
assert mixed[1] == "hello", "Mixed list index 1 check";
assert mixed[2] == true, "Mixed list index 2 check";

var empty = [];
assert len(empty) == 0, "Empty list length check";

assert len(list1) == 3, "List1 length check";
assert len(list2) == 3, "List2 length check";

print("Test 9: PASSED");
