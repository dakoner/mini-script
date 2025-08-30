print("Testing namespace creation...");

import testns from "lib/time_library";

print("Trying to access testns.version");
ver = testns.version;
print("Version: " + ver);
