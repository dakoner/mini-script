print("Testing minimal namespace access...");

import ns from "modules/simple_module";

print("Trying to access ns.simple_var");
val = ns.simple_var;
print("Value: " + val);
