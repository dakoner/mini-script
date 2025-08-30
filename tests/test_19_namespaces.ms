print("=== Test 19: Namespace Import System ===");

print("Testing namespace-based imports...");

// Test namespace import
import time from "lib/time_library";

print("Test 1: Namespace import successful");

// Test accessing namespaced functions
current_time = time.now();
print("Test 2: time.now() = " + current_time);

formatted = time.format(current_time, "%Y-%m-%d %H:%M:%S");
print("Test 3: time.format() = " + formatted);

// Test accessing namespaced variables
print("Test 4: time.version = " + time.version);
print("Test 5: time.name = " + time.name);

// Test time calculations with namespace
future_time = time.add(current_time, 3600);
difference = time.diff(future_time, current_time);
print("Test 6: Added 1 hour, difference = " + difference);

// Test file namespace import
import file from "lib/file_library";

print("Test 7: File namespace import successful");
print("Test 8: file.version = " + file.version);

// Test file operations with namespace
test_file = "namespace_test.txt";
if (file.exists(test_file)) {
    handle = file.open(test_file, "r");
    content = file.read(handle);
    file.close(handle);
    print("Test 9: File content read successfully: " + content);
} else {
    handle = file.open(test_file, "w");
    file.write(handle, "Namespace test data");
    file.close(handle);
    print("Test 9: File created with namespace functions");
}

print("Test 19: PASSED");
