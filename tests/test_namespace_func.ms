print("Testing namespace function access...");

import time from "lib/time_library";

print("Import successful!");
print("Trying to call time.now()");
now = time.now();
print("Current time: " + now);
