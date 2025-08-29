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
```bash
# On Unix/Linux/macOS
./mini_script

# On Windows
mini_script.exe
```

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
- Limited error recovery
- No garbage collection
- Single-threaded execution only

## License

This project is provided as-is for educational purposes.
