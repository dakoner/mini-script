# Mini Script Language

A powerful scripting language implemented in C that supports variables, user-defined functions, modules, control flow, and data structures.

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

#### User-Defined Functions (✅ **FULLY IMPLEMENTED**)
Define and call your own functions with typed parameters:

```javascript
// Function definition
function add(int a, int b) {
    return a + b;
}

function greet(string name) {
    return "Hello, " + name + "!";
}

function factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// Function calls
result = add(10, 20);              // Returns 30
message = greet("Alice");          // Returns "Hello, Alice!"
fact = factorial(5);               // Returns 120
```

**Features:**
- ✅ Typed parameters (`int`, `float`, `string`, `char`)
- ✅ Return values with automatic type conversion
- ✅ Local variable scoping with stack management
- ✅ Recursive function calls supported
- ✅ Function calls as expressions and statements

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

### Modules and Imports (✅ **FULLY IMPLEMENTED**)

Import functionality from other script files to create modular programs:

#### Module Definition (`math_utils.ms`)
```javascript
// math_utils.ms - A utility module
print("Math utilities loaded!");

function square(int x) {
    return x * x;
}

function cube(int x) {
    return x * x * x;
}

function power(int base, int exp) {
    result = 1;
    i = 0;
    while (i < exp) {
        result = result * base;
        i = i + 1;
    }
    return result;
}
```

#### Using Modules (`main.ms`)
```javascript
// Import the math utilities
import "math_utils";

// Use imported functions
x = 5;
print("Square of", x, "is", square(x));       // Output: Square of 5 is 25
print("Cube of", x, "is", cube(x));           // Output: Cube of 5 is 125
print("2^8 =", power(2, 8));                  // Output: 2^8 = 256
```

#### Module Path Resolution
The interpreter searches for modules in the following order:
1. **Current directory** (relative path)
2. **MODULESPATH environment variable** (semicolon-separated paths)

**Example MODULESPATH setup:**
```bash
# Windows
set MODULESPATH=C:\MyScripts\lib;C:\SharedModules

# PowerShell
$env:MODULESPATH = "C:\MyScripts\lib;C:\SharedModules"
```

**Features:**
- ✅ Import statements (`import "module_name";`)
- ✅ Automatic `.ms` extension handling
- ✅ MODULESPATH environment variable support
- ✅ Function definitions persist after import
- ✅ Module execution isolation
- ✅ Circular import protection

### Examples

#### User-Defined Functions
```javascript
// Define a utility function
function calculate_area(float width, float height) {
    return width * height;
}

// Define a function with conditional logic
function grade_letter(int score) {
    if (score >= 90) {
        return "A";
    } else if (score >= 80) {
        return "B";
    } else if (score >= 70) {
        return "C";
    } else {
        return "F";
    }
}

// Use the functions
area = calculate_area(5.5, 3.2);
print("Area:", area);                    // Output: Area: 17.60

student_score = 87;
grade = grade_letter(student_score);
print("Grade:", grade);                  // Output: Grade: B
```

#### Modular Programming
```javascript
// File: string_utils.ms
function reverse_string(string str) {
    // Simple reverse implementation
    return str;  // Placeholder - would need proper string reversal
}

function to_uppercase(string str) {
    // Placeholder for uppercase conversion
    return str;
}

// File: main.ms
import "string_utils";

text = "Hello World";
reversed = reverse_string(text);
upper = to_uppercase(text);
print("Original:", text);
print("Reversed:", reversed);
print("Uppercase:", upper);
```

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
- **Windows**: Microsoft Visual Studio 2022 Build Tools or Visual Studio Community
- **Alternative**: MinGW-w64 or any C compiler with C17 support

### Compilation

#### Using the Build Script (Windows - Recommended)
```bash
# Run the provided build script
.\build.bat
```

The build script automatically:
- Locates Visual Studio 2022 Build Tools
- Sets up the compiler environment
- Compiles with optimizations and C17 standard
- Runs the program to show usage information

#### Manual Compilation (Windows)
```bash
# Using Visual Studio compiler
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cl /W1 /std:c17 mini_script.c /Fe:mini_script.exe

# Using MinGW
gcc -Wall -Wextra -std=c17 -O2 -o mini_script.exe mini_script.c
```

#### Manual compilation (Alternative)
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

#### Example Usage
```bash
# Create a simple script
echo 'function greet(string name) { return "Hello, " + name; } print(greet("World"));' > hello.ms

# Run the script
.\mini_script.exe hello.ms
# Output: Hello, World

# Test module functionality
# Create a module file
echo 'function multiply(int a, int b) { return a * b; }' > math.ms

# Create main script
echo 'import "math"; result = multiply(6, 7); print("6 * 7 =", result);' > main.ms

# Run the main script
.\mini_script.exe main.ms
# Output: 6 * 7 = 42
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
4. **Function Management**: Handles user-defined function definitions and calls
5. **Module System**: Manages imports and separate compilation contexts
6. **Memory Management**: Dynamic allocation for function bodies and variables

### Architecture
- **Token-based lexer**: Converts source code into tokens with comprehensive token types
- **Recursive descent parser**: Builds AST from tokens with support for functions and imports
- **Tree-walking interpreter**: Evaluates AST nodes recursively with proper scoping
- **Dynamic typing**: Values carry type information at runtime
- **Function scoping**: Supports local parameters and stack-based variable management
- **Module isolation**: Parser state management for separate module contexts
- **Memory management**: Proper allocation/deallocation for dynamic function storage

### Recent Major Updates (2025)

#### ✅ **User-Defined Functions (Complete)**
- **Function Parsing**: Complete implementation of function definition parsing
- **Parameter Types**: Support for `int`, `float`, `string`, `char` parameter types  
- **Function Bodies**: Source-based storage and execution of function bodies
- **Local Variables**: Stack-based parameter and local variable management
- **Return Handling**: Proper return value processing with type conversion
- **Recursive Calls**: Full support for recursive function execution
- **Function Lookup**: Efficient function name resolution system

#### ✅ **Module Import System (Complete)**
- **Import Statements**: `import "module_name";` syntax support
- **Module Path Resolution**: Current directory and MODULESPATH environment variable
- **File Extension Handling**: Automatic `.ms` extension detection
- **Module Execution**: Isolated parsing and execution contexts for imported modules
- **Function Persistence**: Imported functions remain available after module loading
- **Parser State Management**: Robust state save/restore for nested execution contexts

#### 🔧 **Enhanced Error Handling**
- Comprehensive error messages for function-related issues
- Import path resolution error reporting
- Function parameter type checking
- Stack overflow protection for recursive calls

### Current Status
- ✅ **Core Language Features**: Variables, operators, control flow - Fully functional
- ✅ **User-Defined Functions**: Complete implementation with typed parameters
- ✅ **Module System**: Full import functionality with MODULESPATH support
- ✅ **Built-in Functions**: `print()`, `len()` working correctly
- ⚠️ **Maps**: Defined but limited functionality
- 🔄 **DLL Support**: Framework in place, needs completion

## Advanced Features

### Environment Variables
- **MODULESPATH**: Semicolon-separated list of directories to search for modules
  ```bash
  # Windows Command Prompt
  set MODULESPATH=C:\MyLibs;D:\SharedModules;C:\Scripts\utils
  
  # PowerShell  
  $env:MODULESPATH = "C:\MyLibs;D:\SharedModules;C:\Scripts\utils"
  
  # Then import modules from any of these paths
  import "my_utilities";  # Searches all MODULESPATH directories
  ```

### Function Features
- **Type Safety**: Function parameters are type-checked at runtime
- **Stack Management**: Proper local variable scoping with stack frames
- **Return Values**: Functions can return values of any supported type
- **Recursion**: Full support for recursive function calls with stack overflow protection
- **Function Overloading**: Functions with the same name will overwrite previous definitions

### Module Features  
- **Execution Isolation**: Each module executes in its own parsing context
- **Function Registration**: Functions defined in modules become globally available
- **Import Once**: Multiple imports of the same module are handled efficiently
- **Path Resolution**: Flexible module location with fallback search paths

## Planned Features

### Enhanced Function System
- Function overloading by parameter count/types
- Default parameter values
- Variable argument lists (varargs)
- Anonymous functions/lambdas

### Advanced Module System
- Module namespaces to prevent function name collisions
- Selective imports (`import "module" { function1, function2 }`)
- Module versioning and dependency management
- Compiled module caching

### DLL Support (Windows)
The interpreter includes a framework for calling external Windows DLL functions:

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

## Current Limitations
- Maps are defined but not fully functional
- Limited string manipulation functions
- No garbage collection (manual memory management)
- Single-threaded execution only
- Limited error recovery in parser

## Version History

### Version 2.0 (August 2025)
- ✅ **Complete user-defined function implementation**
- ✅ **Full module import system with MODULESPATH support**  
- ✅ **Enhanced parser state management**
- ✅ **Stack-based local variable scoping**
- ✅ **Recursive function call support**
- 🔧 **Improved build system with Visual Studio integration**

### Version 1.0 (Initial Release)
- Basic interpreter with variables, operators, control flow
- Built-in functions (`print`, `len`)
- Data types: int, float, char, string, list, map, bool
- Simple expression evaluation and statement execution

## License

This project is provided as-is for educational purposes.
