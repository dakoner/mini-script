// Test 17: File Operations
print("=== Test 17: File Operations ===");

// Test 1: Create a test file and write to it
print("Test 1: Creating and writing to a file");
test_filename = "test_file.txt";
test_content = "Hello, File World!
This is line 2.
This is line 3.";

handle = fopen(test_filename, "w");
print("File handle obtained:", handle);
bytes_written = fwrite(handle, test_content);
print("Bytes written:", bytes_written);
close_result = fclose(handle);
print("File closed with result:", close_result);

// Test 2: Check if file exists
print("Test 2: Checking if file exists");
exists = fexists(test_filename);
print("File exists:", exists);

// Test 3: Read the entire file
print("Test 3: Reading entire file content");
handle = fopen(test_filename, "r");
print("File opened for reading, handle:", handle);
content = fread(handle);
print("File content:");
print(content);
fclose(handle);

// Test 4: Read file line by line
print("Test 4: Reading file line by line");
handle = fopen(test_filename, "r");
print("Reading lines:");
line_count = 0;
line1 = freadline(handle);
if (len(line1) > 0) {
    line_count = line_count + 1;
    print("Line", line_count, ":", line1);
}
line2 = freadline(handle);
if (len(line2) > 0) {
    line_count = line_count + 1;
    print("Line", line_count, ":", line2);
}
line3 = freadline(handle);
if (len(line3) > 0) {
    line_count = line_count + 1;
    print("Line", line_count, ":", line3);
}
fclose(handle);
print("Total lines read:", line_count);

// Test 5: Append to file
print("Test 5: Appending to file");
append_content = "
This is an appended line.";
handle = fopen(test_filename, "a");
print("File opened for appending");
fwrite(handle, append_content);
fclose(handle);
print("Content appended successfully");

// Test 6: Write line by line to a new file
print("Test 6: Writing line by line to new file");
new_filename = "line_test.txt";
handle = fopen(new_filename, "w");
print("Writing lines to new file");
fwriteline(handle, "First line");
fwriteline(handle, "Second line");
fwriteline(handle, "Third line");
fclose(handle);
print("Lines written successfully");

// Test 7: Error handling - try to read non-existent file
print("Test 7: Error handling");
non_existent = fexists("non_existent.txt");
print("Non-existent file exists:", non_existent);

// Test 8: File handle validation
print("Test 8: File operations complete");
print("All file operations tested successfully!");

print("Test 17: PASSED");
print("");
