use std::fmt;

#[derive(Debug)]
pub struct RuntimeError {
    pub message: String,
    pub line: Option<usize>,
    pub filename: String,
    pub return_value: Option<crate::interpreter::Value>,
}

impl RuntimeError {
    pub fn new(message: String, line: Option<usize>, filename: &str) -> Self {
        Self {
            message,
            line,
            filename: filename.to_string(),
            return_value: None,
        }
    }
    
    pub fn with_return_value(message: String, line: Option<usize>, filename: &str, return_value: crate::interpreter::Value) -> Self {
        Self {
            message,
            line,
            filename: filename.to_string(),
            return_value: Some(return_value),
        }
    }
}

impl fmt::Display for RuntimeError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let line_info = match self.line {
            Some(line) => format!(" at line {}", line),
            None => " at unknown".to_string(),
        };
        write!(f, "Error in {}{}: {}", self.filename, line_info, self.message)
    }
}

impl std::error::Error for RuntimeError {}

#[derive(Debug)]
pub struct ReturnValue {
    pub value: crate::interpreter::Value,
}

impl ReturnValue {
    pub fn new(value: crate::interpreter::Value) -> Self {
        Self { value }
    }
}
