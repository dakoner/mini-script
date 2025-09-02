# Mini Script - C Implementation

This directory contains the C implementation of the Mini Script interpreter.

## Building

```bash
make
```

## Running

```bash
# Run a script file
./mini_script script.ms

# Start REPL
./mini_script
```

## Files

- `main.c` - Main entry point and file handling
- `lexer.c` - Tokenization and lexical analysis  
- `parser.c` - Recursive descent parser
- `interpreter.c` - AST evaluation and runtime
- `value.c` - Value type management and memory handling
- `environment.c` - Variable scope and environment management
- `mini_script.h` - Main header with type definitions

## Features

- Core Mini Script language support
- Basic built-in functions (print, len, time_now)
- Memory management
- Variable scoping
- Control flow (if/else, while loops)
- Arithmetic and string operations
- REPL mode

## Requirements

- GCC compiler
- C99 standard support
- Math library (linked with -lm)
