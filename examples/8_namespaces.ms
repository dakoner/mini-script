print("=== Example 8: Namespace System ===");

// Import with namespace - cleaner API
import time from "lib/time_library";
import file from "lib/file_library";

print("Using namespaced libraries:");
print("Time library: " + time.name + " v" + time.version);
print("File library: " + file.name + " v" + file.version);
print("");

// Time operations with clean namespace
now = time.now();
print("Current time functions:");
print("- Timestamp: " + now);
print("- Formatted: " + time.format(now, "%Y-%m-%d %H:%M:%S"));
print("- Year: " + time.year(now));
print("- Month: " + time.month(now));
print("- Day: " + time.day(now));

// File operations with clean namespace
log_file = "namespace_demo.log";
print("");
print("File operations:");

if (file.exists(log_file)) {
    print("- Log file exists, size: " + file.size(log_file) + " bytes");
} else {
    print("- Creating new log file");
}

// Write timestamped log entry
handle = file.open(log_file, "a");
timestamp = time.format(now, "%Y-%m-%d %H:%M:%S");
file.writeline(handle, "[" + timestamp + "] Namespace demo executed");
file.close(handle);

print("- Log entry written");

// Read and display log
handle = file.open(log_file, "r");
content = file.read(handle);
file.close(handle);

print("- Log contents:");
print(content);

print("");
print("Benefits of namespace system:");
print("- Clean API: time.now() instead of time_now()");
print("- No naming conflicts between libraries");
print("- Better code organization");
print("- Professional syntax similar to modern languages");

print("");
print("Namespace example completed!");
