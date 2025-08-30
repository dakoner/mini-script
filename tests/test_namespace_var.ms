print("Testing namespace variable access...");

import time from "lib/time_library";

print("Import successful!");
print("Trying to access time.version");
ver = time.version;
print("Version: " + ver);
