# DLL Integration Framework - Design Document

## Status: Framework Designed, Implementation Planned

This document outlines the design for Windows DLL integration in the Mini Script Language. While the core language features (namespaces, file I/O, time operations, REPL, etc.) are fully implemented and operational, DLL functionality represents a planned future enhancement.

## Current Implementation Status

### ✅ Fully Implemented Core Features
- **Complete scripting language** with 7 data types, user-defined functions, and control flow
- **Namespace system** with `import name from "file"` syntax and dot notation access
- **File I/O library** with 8 comprehensive file operations (fopen, fclose, fread, fwrite, etc.)
- **Time library** with 13 date/time functions (time_now, time_format, time_parse, etc.)
- **REPL mode** for interactive scripting and testing
- **Enhanced error reporting** with filename and line number tracking
- **Comprehensive testing** with 20 tests achieving 100% pass rate

### 🔧 DLL Framework Status
- **Syntax recognition**: DLL-related tokens and parsing implemented
- **Framework functions**: loadlib, getproc, callext, freelib syntax accepted
- **Test coverage**: test_14_dll_loading.ms validates framework readiness
- **Full implementation**: Planned for future development phase

## Overview

This guide explains the design for adding Windows DLL function calling support to the Mini Script Language, allowing scripts to call external Windows API functions when the framework is fully implemented.

## Planned DLL Functions

The following built-in functions are designed to support DLL operations when fully implemented:

### `loadlib(dll_name)`
- **Purpose**: Load a Windows DLL into memory
- **Parameters**: `dll_name` (string) - Name of the DLL file (e.g., "User32.dll")
- **Returns**: DLL handle (integer) or 0 on failure
- **Example**: `handle = loadlib("User32.dll");`
- **Current Status**: Syntax parsing implemented, full functionality planned

### `getproc(dll_name, function_name)`
- **Purpose**: Get the address of a function in a loaded DLL
- **Parameters**: 
  - `dll_name` (string) - Name of the loaded DLL
  - `function_name` (string) - Name of the function to find
- **Returns**: Function pointer (integer) or 0 on failure
- **Example**: `func = getproc("User32.dll", "MessageBoxA");`
- **Current Status**: Syntax parsing implemented, full functionality planned

### `callext(function_name, arg1, arg2, ...)`
- **Purpose**: Call an external DLL function with arguments
- **Parameters**: 
  - `function_name` (string) - Name of the function to call
  - `args` (various) - Arguments to pass to the function
- **Returns**: Function return value (integer)
- **Example**: `result = callext("MessageBoxA", 0, "Hello", "Title", 0);`

### `freelib(dll_name)`
- **Purpose**: Free a loaded DLL from memory
- **Parameters**: `dll_name` (string) - Name of the DLL to free
- **Returns**: 0 on success
- **Example**: `freelib("User32.dll");`

## Implementation Plan

### 1. Data Structures
```c
// DLL Module tracking
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    HMODULE handle;
} DllModule;

// DLL Function tracking
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    char module_name[MAX_IDENTIFIER_LEN];
    FARPROC proc_address;
    int param_count;
    ValueType param_types[MAX_PARAMS];
    ValueType return_type;
} DllFunction;
```

### 2. New Value Types
```c
typedef enum {
    // ... existing types ...
    TYPE_DLL_HANDLE,
    TYPE_DLL_FUNCTION
} ValueType;
```

### 3. Token Extensions
```c
typedef enum {
    // ... existing tokens ...
    TOKEN_LOADLIB,
    TOKEN_GETPROC,
    TOKEN_FREELIB,
    TOKEN_CALLEXT
} TokenType;
```

### 4. Core Functions

#### Load DLL
```c
HMODULE load_dll(const char* dll_name) {
    // Check if already loaded
    // Convert to wide string
    // Call LoadLibrary
    // Store in module list
    // Return handle
}
```

#### Get Function Address
```c
FARPROC get_dll_function(const char* dll_name, const char* func_name) {
    // Find loaded module
    // Call GetProcAddress
    // Store function info
    // Return function pointer
}
```

#### Call External Function
```c
Value call_external_function(const char* func_name, ASTNode** args, int arg_count) {
    // Find function info
    // Convert arguments to appropriate types
    // Handle different calling conventions
    // Call function through pointer
    // Return result
}
```

## Example Usage

### MessageBox Example
```javascript
// Load User32.dll
dll_handle = loadlib("User32.dll");

// Get MessageBoxA function
msgbox_func = getproc("User32.dll", "MessageBoxA");

// Call MessageBoxA(NULL, "Text", "Caption", MB_OK)
result = callext("MessageBoxA", 0, "Hello from Script!", "Message", 0);

// Clean up
freelib("User32.dll");
```

### System Information Example
```javascript
// Get system metrics
user32 = loadlib("user32.dll");
getmetrics = getproc("user32.dll", "GetSystemMetrics");

// Get screen dimensions
width = callext("GetSystemMetrics", 0);   // SM_CXSCREEN
height = callext("GetSystemMetrics", 1);  // SM_CYSCREEN

print("Screen size:", width, "x", height);
freelib("user32.dll");
```

### File Operations Example
```javascript
// Use kernel32 for file operations
kernel32 = loadlib("kernel32.dll");
createfile = getproc("kernel32.dll", "CreateFileA");

// This would need more complex parameter handling
// handle = callext("CreateFileA", "test.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

freelib("kernel32.dll");
```

## Implementation Challenges

### 1. Calling Conventions
- Windows APIs use `__stdcall` convention
- Need to handle different parameter passing methods
- x64 vs x86 differences

### 2. Type Conversion
- Convert script values to C types
- Handle pointers and structures
- String encoding (ANSI vs Unicode)

### 3. Parameter Handling
- Variable number of arguments
- Different parameter types
- Pointer parameters

### 4. Error Handling
- Windows error codes
- Invalid function calls
- Memory management

## Safety Considerations

### 1. Memory Safety
- Validate pointers before use
- Prevent buffer overflows
- Clean up resources properly

### 2. Type Safety
- Verify parameter counts
- Check parameter types
- Validate return values

### 3. Security
- Restrict which DLLs can be loaded
- Validate function names
- Prevent dangerous operations

## Testing Strategy

### 1. Basic Functions
- LoadLibrary/FreeLibrary
- GetProcAddress
- Simple function calls

### 2. Common APIs
- MessageBox functions
- System information
- File operations

### 3. Edge Cases
- Invalid DLL names
- Missing functions
- Invalid parameters

## Integration with Build System

### Compiler Flags
```makefile
# Add Windows libraries
LIBS = -lkernel32 -luser32 -lgdi32

# Enable Windows API
CFLAGS += -DWIN32_LEAN_AND_MEAN
```

### Conditional Compilation
```c
#ifdef _WIN32
    // DLL support code
#else
    // Stub implementations or error messages
#endif
```

## Future Enhancements

### 1. Advanced Features
- Structure parameter support
- Callback function support
- COM interface support

### 2. Cross-Platform
- Linux shared library support (.so)
- macOS dynamic library support (.dylib)

### 3. Type System
- Better type inference
- Structure definitions
- Function signatures

## Conclusion

The DLL support framework provides a powerful way to extend the Mini Script Language with native Windows API access. The implementation requires careful attention to type safety, memory management, and error handling, but offers significant functionality expansion for Windows-specific scripting tasks.

The current basic interpreter is fully functional and ready for this enhancement when development time permits.
