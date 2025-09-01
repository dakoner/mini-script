use std::env;
use std::process;

mod lexer;
mod parser;
mod interpreter;
mod ast;
mod builtin;
mod error;

use lexer::Lexer;
use parser::Parser;
use interpreter::Interpreter;
use error::RuntimeError;

fn main() {
    let args: Vec<String> = env::args().collect();

    match args.len() {
        1 => run_prompt(),
        2 => run_file(&args[1]),
        _ => {
            eprintln!("Usage: mini_script [script]");
            process::exit(64);
        }
    }
}

fn run(source: &str, filename: &str, interpreter: Option<&mut Interpreter>) -> Result<(), RuntimeError> {
    let mut lexer = Lexer::new(source, filename);
    let tokens = lexer.scan_tokens()?;

    let mut parser = Parser::new(tokens, filename);
    let statements = parser.parse()?;

    match interpreter {
        Some(interp) => {
            let original_filename = interp.filename.clone();
            interp.filename = filename.to_string();
            let result = interp.interpret(&statements);
            interp.filename = original_filename;
            result
        }
        None => {
            let mut interp = Interpreter::new(filename);
            interp.interpret(&statements)
        }
    }
}

fn run_file(path: &str) {
    println!("Mini Script Language Interpreter");
    println!("=================================");
    
    let display_path = std::path::Path::new(path)
        .strip_prefix(std::env::current_dir().unwrap_or_default())
        .unwrap_or(std::path::Path::new(path))
        .display()
        .to_string()
        .replace("\\", "/");
    
    println!("Executing: {}", display_path);
    println!("---------------------------------");
    println!();

    let source = match std::fs::read_to_string(path) {
        Ok(content) => content,
        Err(_) => {
            println!("Error: File not found at {}", path);
            process::exit(1);
        }
    };

    let abs_path = std::fs::canonicalize(path)
        .unwrap_or_else(|_| std::path::PathBuf::from(path))
        .display()
        .to_string();

    if let Err(e) = run(&source, &abs_path, None) {
        println!("{}", e);
        process::exit(1);
    }
}

fn run_prompt() {
    println!("Mini Script REPL (type 'exit' to quit)");
    let mut interpreter = Interpreter::new("<REPL>");
    
    loop {
        print!("> ");
        use std::io::{self, Write};
        io::stdout().flush().unwrap();
        
        let mut input = String::new();
        match io::stdin().read_line(&mut input) {
            Ok(_) => {
                let line = input.trim();
                if line.to_lowercase() == "exit" {
                    break;
                }
                if let Err(e) = run(line, "<REPL>", Some(&mut interpreter)) {
                    println!("{}", e);
                }
            }
            Err(_) => break,
        }
    }
}
