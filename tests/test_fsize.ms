// Test the fsize function
print("Testing fsize function...");

// Test with existing file
size1 = fsize("demo_data.txt");
print("Size of demo_data.txt:", size1, "bytes");

size2 = fsize("test_file.txt");
print("Size of test_file.txt:", size2, "bytes");

// Test with non-existent file
size3 = fsize("nonexistent.txt");
print("Size of nonexistent.txt:", size3, "bytes");

print("fsize test completed!");
