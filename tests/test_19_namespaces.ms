print("=== Test 19: Namespace Import System ===");

// This test verifies the namespace import feature.

// Test time namespace import
import time from "lib/time_library";
assert time != null, "Time namespace should be imported";

// Test accessing namespaced variables
assert time.version == "1.1", "time.version check";
assert time.name == "Time Library", "time.name check";

// Test accessing namespaced functions
// We can't assert the exact value of time.now(), but we can check it's a number.
var current_time = time.now();
assert current_time > 0, "time.now() should return a positive number";

// Test time calculations with namespace
var future_time = time.add(current_time, 3600);
var difference = time.diff(future_time, current_time);
assert difference == 3600, "time.diff check";

// Test file namespace import
import file from "lib/file_library_no_print";
assert file != null, "File namespace should be imported";
assert file.version == "1.0", "file.version check";

// Test file operations with namespace
var test_file = "namespace_test_19.txt";
var handle = file.open(test_file, "w");
assert handle != null, "file.open for write should succeed";
file.write(handle, "Namespace test data");
file.close(handle);

assert file.exists(test_file) == true, "file.exists check";

handle = file.open(test_file, "r");
assert handle != null, "file.open for read should succeed";
var content = file.read(handle);
file.close(handle);
assert content == "Namespace test data", "file.read check";

print("Test 19: PASSED");
