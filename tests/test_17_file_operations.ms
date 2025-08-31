// Test 17: File Operations
print("=== Test 17: File Operations ===");

// Note: These tests depend on file I/O functions (fopen, fwrite, etc.)
// which may not be fully implemented in the Python interpreter.
// This test script verifies the syntax and expected behavior.

var test_filename = "test_file_17.txt";
var test_content = "Hello, File World!
This is line 2.
This is line 3.";

// Test 1: Create a test file and write to it
var handle = fopen(test_filename, "w");
assert handle != nil, "File handle for write should be valid";
var bytes_written = fwrite(handle, test_content);
// The exact number of bytes can vary with line endings, so we check it's greater than a minimum.
assert bytes_written > 30, "Bytes written should be greater than 30";
var close_result = fclose(handle);
assert close_result == 0, "File close should return 0 on success";

// Test 2: Check if file exists
assert fexists(test_filename) == true, "File should exist after writing";

// Test 3: Read the entire file
handle = fopen(test_filename, "r");
assert handle != nil, "File handle for read should be valid";
var content = fread(handle);
assert content == test_content, "Read content should match written content";
fclose(handle);

// Test 4: Read file line by line
handle = fopen(test_filename, "r");
assert freadline(handle) == "Hello, File World!", "First line read should be correct";
assert freadline(handle) == "This is line 2.", "Second line read should be correct";
assert freadline(handle) == "This is line 3.", "Third line read should be correct";
fclose(handle);

// Test 5: Append to file
var append_content = "
This is an appended line.";
handle = fopen(test_filename, "a");
assert handle != nil, "File handle for append should be valid";
fwrite(handle, append_content);
fclose(handle);

// Verify append
handle = fopen(test_filename, "r");
var final_content = fread(handle);
assert final_content == test_content + append_content, "Appended content should be correct";
fclose(handle);

// Test 6: Write line by line to a new file
var new_filename = "line_test_17.txt";
handle = fopen(new_filename, "w");
fwriteline(handle, "First line");
fwriteline(handle, "Second line");
fwriteline(handle, "Third line");
fclose(handle);

handle = fopen(new_filename, "r");
assert freadline(handle) == "First line", "fwriteline first line check";
assert freadline(handle) == "Second line", "fwriteline second line check";
assert freadline(handle) == "Third line", "fwriteline third line check";
fclose(handle);

// Test 7: Error handling
assert fexists("non_existent_file.txt") == false, "Non-existent file should not exist";
var bad_handle = fopen("non_existent_file.txt", "r");
assert bad_handle == nil, "Opening a non-existent file for reading should return nil";

print("Test 17: PASSED");
print("");
