// File Library for Mini Script Language - Namespace Version
// Provides file I/O operations

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

function size(filename) {
    return fsize(filename);
}

version = "2.0";
name = "FileLibrary";

print("File library v2.0 loaded");
