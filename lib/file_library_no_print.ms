// File Library for Mini Script Language - Namespace Version
// Provides file I/O operations

// Define wrapper functions for namespace access
function open(filename, mode) {
    return fopen(filename, mode);
}

function close(handle) {
    return fclose(handle);
}

function read(handle) {
    return fread(handle);
}

function write(handle, content) {
    return fwrite(handle, content);
}

function readline(handle) {
    return freadline(handle);
}

function writeline(handle, line) {
    return fwriteline(handle, line);
}

function exists(filename) {
    return fexists(filename);
}

// Module metadata
version = "1.0";
name = "File Library";
