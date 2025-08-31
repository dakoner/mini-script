print("=== Test 19: Namespace Import System ===");

// This test verifies the namespace import feature.

// Test time namespace import
import "lib/time_library";
assert version == "1.1", "time.version check";
assert name == "Time Library", "time.name check";

// Test accessing namespaced functions
// We can't assert the exact value of now(), but we can check it's a number.
var current_time = now();
assert current_time > 0, "now() should return a positive number";

// Test time calculations with namespace
var future_time = add(current_time, 3600);
var difference = diff(future_time, current_time);
assert difference == 3600, "diff check";

// Test file namespace import
import "lib/file_library_no_print";
assert version == "1.0", "file.version check";

// Test file operations with namespace
var test_file = "namespace_test_19.txt";
var handle = open(test_file, "w");
assert handle != nil, "open for write should succeed";
write(handle, "Namespace test data");
close(handle);

assert exists(test_file) == true, "exists check";

handle = open(test_file, "r");
assert handle != nil, "open for read should succeed";
var content = read(handle);
close(handle);
assert content == "Namespace test data", "read check";

print("Test 19: PASSED");
