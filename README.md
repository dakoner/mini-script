# Mini Script Language

A simple scripting language implemented in C that supports variables, functions, control flow, and data structures.

## Features

### Data Types
- **int**: Integer numbers (e.g., `42`, `-10`)
- **float**: Floating-point numbers (e.g., `3.14`, `-2.5`)
- **char**: Single characters (e.g., `'a'`, `'Z'`)
- **string**: Text strings (e.g., `"Hello World"`)
- **list**: Arrays of values (e.g., `[1, 2, 3]`)
- **map**: Key-value pairs (e.g., `{"key": "value"}`)
- **bool**: Boolean values (`true`, `false`)

### Variables
Variables are dynamically typed and can be assigned using the `=` operator:

```
x = 10;
name = "Alice";
numbers = [1, 2, 3, 4, 5];
```

### Operators

#### Arithmetic
- `+` : Addition (also string concatenation)
- `-` : Subtraction
- `*` : Multiplication
- `/` : Division

#### Comparison
- `==` : Equal
- `!=` : Not equal
- `<` : Less than
- `>` : Greater than
- `<=` : Less than or equal
- `>=` : Greater than or equal

#### Logical
- `&&` : Logical AND
- `||` : Logical OR
- `!` : Logical NOT

### Control Flow

#### If-Else Statements
```
if (condition) {
    // statements
} else {
    // statements
}
```

#### While Loops
```
while (condition) {
    // statements
}
```

#### For Loops
```
for (init; condition; update) {
    // statements
}
```

### Functions

#### Function Definition
```
function name(type param1, type param2) {
    // function body
    return value;
}
```

#### Built-in Functions
- `print(args...)` : Print values to console
- `len(collection)` : Get length of string or list

### Examples

#### Basic Variables and Arithmetic
```
x = 10;
y = 20;
result = x + y;
print("Result:", result);
```

#### Control Flow
```
n = 5;
if (n > 0) {
    print("Positive number");
} else {
    print("Non-positive number");
}
```

#### Loops
```
// While loop
i = 1;
while (i <= 5) {
    print("Count:", i);
    i = i + 1;
}

// For loop
for (j = 1; j <= 5; j = j + 1) {
    print("Index:", j);
}
```

#### Lists
```
numbers = [1, 2, 3, 4, 5];
print("Numbers:", numbers);
print("Length:", len(numbers));
```

#### String Operations
```
greeting = "Hello";
name = "World";
message = greeting + " " + name + "!";
print(message);
```

## Building and Running

### Prerequisites
- GCC compiler
- Make (optional, for using Makefile)

### Compilation

#### Using Makefile (recommended)
```bash
make
```

#### Manual compilation
```bash
gcc -Wall -Wextra -std=c99 -g -o mini_script mini_script.c
```

### Running

Run script files with command-line arguments:

```bash
# Run a script file
.\mini_script.exe script.ms

# Show help information  
.\mini_script.exe -h

# Run built-in examples
.\mini_script.exe -examples
```

#### Command Line Options
- `script_file` - Path to Mini Script file to execute (.ms extension recommended)
- `-h, --help` - Show help message with usage information  
- `-examples` - Run built-in example demonstrations

#### File Extensions
- `.ms` - Mini Script files (recommended)
- `.txt` - Plain text script files

## Implementation Details

The interpreter is implemented as a single C file (`mini_script.c`) and includes:

1. **Lexical Analysis**: Tokenizes the source code into tokens
2. **Parsing**: Builds an Abstract Syntax Tree (AST) from tokens
3. **Evaluation**: Executes the AST to run the program

### Architecture
- **Token-based lexer**: Converts source code into tokens
- **Recursive descent parser**: Builds AST from tokens
- **Tree-walking interpreter**: Evaluates AST nodes recursively
- **Dynamic typing**: Values carry type information at runtime
- **Variable scoping**: Supports local and global variables
- **Memory management**: Handles allocation/deallocation of dynamic data

### Current Limitations
- User-defined functions are parsed but not fully implemented
- Maps are defined but not fully functional
- DLL function calling is designed but needs implementation completion
- Limited error recovery
- No garbage collection
- Single-threaded execution only

## Planned Features

### DLL Support (Windows)
The interpreter includes a planned framework for calling external Windows DLL functions:

```javascript
// Load a Windows DLL
dll_handle = loadlib("User32.dll");

// Get function address
msgbox_func = getproc("User32.dll", "MessageBoxA");

// Call external function
result = callext("MessageBoxA", 0, "Hello from Script!", "Message", 0);

// Cleanup
freelib("User32.dll");
```

This would enable scripts to:
- Call Windows API functions
- Display message boxes
- Access system information
- Perform file operations
- Interface with any Windows DLL

See `DLL_IMPLEMENTATION_GUIDE.md` for detailed implementation plans and `dll_demo.ms` for example usage.

## License

This project is provided as-is for educational purposes.
