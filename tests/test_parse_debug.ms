print("Testing if namespace syntax is parsed...");

// Test without 'from' to see if it's the parsing that's the issue
import "lib/time_library";

// Should work with global access
global_ver = version;
print("Global version from regular import: " + global_ver);
