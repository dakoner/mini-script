// File Library for Mini Script Language - Namespace Version
// Provides file I/O operations

// Define wrapper functions for namespace access
function open(string filename, string mode) {
    return fopen(filename, mode);
}

function close(int handle) {
    return fclose(handle);
}

function read(int handle) {
    return fread(handle);
}

function write(int handle, string content) {
    return fwrite(handle, content);
}

function readline(int handle) {
    return freadline(handle);
}

function writeline(int handle, string line) {
    return fwriteline(handle, line);
}

function exists(string filename) {
    return fexists(filename);
}

function size(string filename) {
    return fsize(filename);
}

// Module metadata
version = "2.0";
name = "FileLibrary";

print("File library v2.0 loaded with namespace support!");
