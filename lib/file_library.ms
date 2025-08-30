// File Library for Mini Script Language
// Provides file I/O operations

print("=== File Library ===");
print("This library provides file input/output operations:");
print("");

print("Available functions:");
print("- fopen(filename, mode) - Open a file and return file handle");
print("- fclose(handle) - Close an open file handle");
print("- fread(handle) - Read entire content of file as string");
print("- fwrite(handle, content) - Write string content to file");
print("- freadline(handle) - Read one line from file");
print("- fwriteline(handle, line) - Write one line to file");
print("- fexists(filename) - Check if file exists");
print("- fsize(filename) - Get file size in bytes");
print("");

print("File modes:");
print("- 'r' - Read only (default)");
print("- 'w' - Write only (creates new file)");
print("- 'a' - Append (write to end of file)");
print("- 'r+' - Read and write");
print("");

print("Usage examples:");
print("  handle = fopen('data.txt', 'r');");
print("  content = fread(handle);");
print("  fclose(handle);");
print("");
print("  handle = fopen('output.txt', 'w');");
print("  fwrite(handle, 'Hello, World!');");
print("  fclose(handle);");
print("");

print("File library loaded successfully!");
