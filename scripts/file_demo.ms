// File Operations Demo
// Demonstrates file I/O capabilities of Mini Script Language

print("=== File Operations Demonstration ===");
print("");

// Import the file library documentation
import "lib/file_library.ms";

print("");
print("=== Practical File Operations Demo ===");

// Create a data file
data_file = "demo_data.txt";
print("Creating data file:", data_file);

// Write some sample data
file_handle = fopen(data_file, "w");
fwriteline(file_handle, "Name: John Doe");
fwriteline(file_handle, "Age: 30");
fwriteline(file_handle, "City: New York");
fwriteline(file_handle, "Email: john.doe@example.com");
fclose(file_handle);

print("Data written to file successfully!");

// Check if file exists and read it back
if (fexists(data_file)) {
    print("File exists! Reading contents:");
    
    // Read line by line
    read_handle = fopen(data_file, "r");
    
    name_line = freadline(read_handle);
    age_line = freadline(read_handle);
    city_line = freadline(read_handle);
    email_line = freadline(read_handle);
    
    print("  ", name_line);
    print("  ", age_line);
    print("  ", city_line);
    print("  ", email_line);
    
    fclose(read_handle);
} else {
    print("File was not created successfully");
}

// Demonstrate append mode
print("");
print("Appending additional information...");
append_handle = fopen(data_file, "a");
fwriteline(append_handle, "Phone: 555-1234");
fwriteline(append_handle, "Status: Active");
fclose(append_handle);

// Read entire file content
print("Complete file contents:");
final_handle = fopen(data_file, "r");
full_content = fread(final_handle);
print(full_content);
fclose(final_handle);

print("");
print("File operations demonstration completed successfully!");
print("Files created: demo_data.txt");
