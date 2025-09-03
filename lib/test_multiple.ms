// Test multiple functions
function exists(filename) {
    return fexists(filename);
}

function writeline(handle, line) {
    return fwriteline(handle, line);
}

print("Multiple functions loaded");
