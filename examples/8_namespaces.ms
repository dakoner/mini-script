print("=== Example 8: Namespace System ===");

// Import libraries - standard syntax (namespace feature not yet implemented)
import "lib/time_library";
import "lib/file_library";

print("Using imported libraries:");
print("Time library loaded successfully");
print("File library loaded successfully");
print("");

// Time operations with imported functions
timestamp = now();
print("Current time functions:");
print("- Timestamp: " + timestamp);
print("- Formatted: " + format(timestamp, "%Y-%m-%d %H:%M:%S"));
print("- Year: " + year(timestamp));
print("- Month: " + month(timestamp));
print("- Day: " + day(timestamp));

// File operations with imported functions
log_file = "namespace_demo.log";
print("");
print("File operations:");

if (exists(log_file)) {
    print("- Log file exists");
} else {
    print("- Creating new log file");
}

// Write timestamped log entry
handle = open(log_file, "a");
log_timestamp = format(timestamp, "%Y-%m-%d %H:%M:%S");
writeline(handle, "[" + log_timestamp + "] Namespace demo executed");
close(handle);

print("- Log entry written");

// Read and display log
handle = open(log_file, "r");
content = read(handle);
close(handle);

print("- Log contents:");
print(content);

print("");
print("Benefits of import system:");
print("- Direct function access after import");
print("- No naming conflicts when properly organized");
print("- Simple syntax for common operations");
print("- Extensible module system");

print("");
print("Import example completed!");
