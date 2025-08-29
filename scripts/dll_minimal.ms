print("=== DLL Function Calling Demonstration ===");
print("Step 1: Loading DLL");
dll_name = "User32.dll";
print("loadlib DLL:", dll_name);

print("Step 2: Variables");
hwnd = 0;
message_text = "Hello from Mini Script!";
caption = "DLL Demo";
type = 0;

print("HWND:", hwnd);
print("Text:", message_text);
print("Caption:", caption);
print("Type:", type);

print("=== End ===");
