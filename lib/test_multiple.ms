// Test multiple functions
function exists(string filename) {
    return fexists(filename);
}

function writeline(int handle, string line) {
    return fwriteline(handle, line);
}

print("Multiple functions loaded");
