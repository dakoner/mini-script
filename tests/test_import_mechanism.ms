print("Testing import mechanism...");

// Try old syntax first
import "lib/time_library";

print("Old import completed.");

// Try to access a global function that should be available
ver = version;
print("Global version: " + ver);
