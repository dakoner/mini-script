# Mini Script - Rust Implementation

This directory contains the Rust implementation of the Mini Script interpreter.

## Building

```bash
# From the project root
cargo build

# For release build
cargo build --release
```

## Running

```bash
# Run a script file
cargo run -- script.ms

# Start REPL
cargo run
```

## Files

- `main.rs` - Main entry point and CLI handling
- `lexer.rs` - Tokenization and lexical analysis
- `parser.rs` - Recursive descent parser
- `ast.rs` - Abstract Syntax Tree definitions
- `interpreter.rs` - AST evaluation and runtime
- `builtin.rs` - Built-in function implementations
- `error.rs` - Error handling and reporting

## Features

- Complete Mini Script language support
- All built-in functions implemented
- Comprehensive error handling
- File I/O operations
- Time operations
- Import system
- REPL mode
