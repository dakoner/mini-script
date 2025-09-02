use std::time::{SystemTime, UNIX_EPOCH};
use std::io::{Read, Write, BufRead, BufReader, Seek, SeekFrom};
use chrono::{DateTime, NaiveDateTime, NaiveDate, Datelike, Timelike, Utc};
use crate::interpreter::{Value, Callable};
use crate::error::RuntimeError;

#[derive(Debug, Clone)]
pub struct BuiltinPrint;

impl Callable for BuiltinPrint {
    fn arity(&self) -> i32 {
        -1 // Variadic
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let mut output = Vec::new();
        for arg in arguments {
            output.push(stringify_value(&arg));
        }
        println!("{}", output.join(" "));
        Ok(Value::Nil)
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinLen;

impl Callable for BuiltinLen {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        match &arguments[0] {
            Value::String(s) => Ok(Value::Number(s.len() as f64)),
            Value::List(list) => Ok(Value::Number(list.borrow().len() as f64)),
            _ => Err(RuntimeError::new(
                "len() expects a string or a list.".to_string(),
                None,
                "<builtin>",
            )),
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinTimeNow;

impl Callable for BuiltinTimeNow {
    fn arity(&self) -> i32 {
        0
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, _arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let now = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs_f64();
        Ok(Value::Number(now))
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinTimeFormat;

impl Callable for BuiltinTimeFormat {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let timestamp = match &arguments[0] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "time_format() expects a numeric timestamp as the first argument.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let format_str = match &arguments[1] {
            Value::String(s) => s,
            _ => return Err(RuntimeError::new(
                "time_format() expects a format string as the second argument.".to_string(),
                None,
                "<builtin>",
            )),
        };

        // Convert timestamp to DateTime in UTC (to match the parsing)
        let dt = DateTime::<Utc>::from_timestamp(timestamp as i64, 0);
        match dt {
            Some(datetime) => {
                let result = datetime.format(format_str).to_string();
                Ok(Value::String(result))
            }
            None => Err(RuntimeError::new(
                "Invalid timestamp.".to_string(),
                None,
                "<builtin>",
            )),
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinTimeParse;

impl Callable for BuiltinTimeParse {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let time_str = match &arguments[0] {
            Value::String(s) => s,
            _ => return Err(RuntimeError::new(
                "time_parse() expects two string arguments.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let format_str = match &arguments[1] {
            Value::String(s) => s,
            _ => return Err(RuntimeError::new(
                "time_parse() expects two string arguments.".to_string(),
                None,
                "<builtin>",
            )),
        };

        // Try parsing as full datetime first
        match NaiveDateTime::parse_from_str(time_str, format_str) {
            Ok(dt) => {
                let timestamp = dt.and_utc().timestamp() as f64;
                Ok(Value::Number(timestamp))
            }
            Err(_) => {
                // If that fails, try parsing as date-only and assume start of day
                match NaiveDate::parse_from_str(time_str, format_str) {
                    Ok(date) => {
                        let dt = date.and_hms_opt(0, 0, 0).unwrap();
                        let timestamp = dt.and_utc().timestamp() as f64;
                        Ok(Value::Number(timestamp))
                    }
                    Err(_) => Ok(Value::Nil),
                }
            }
        }
    }
}

// Time component extractors
macro_rules! time_component_builtin {
    ($name:ident, $method:ident) => {
        #[derive(Debug, Clone)]
        pub struct $name;

        impl Callable for $name {
            fn arity(&self) -> i32 {
                1
            }

            fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
                let timestamp = match &arguments[0] {
                    Value::Number(n) => *n,
                    _ => return Err(RuntimeError::new(
                        format!("{}() expects a numeric timestamp.", stringify!($name).to_lowercase()),
                        None,
                        "<builtin>",
                    )),
                };

                let dt = DateTime::<Utc>::from_timestamp(timestamp as i64, 0);
                match dt {
                    Some(datetime) => Ok(Value::Number(datetime.$method() as f64)),
                    None => Err(RuntimeError::new(
                        "Invalid timestamp.".to_string(),
                        None,
                        "<builtin>",
                    )),
                }
            }
        }
    };
}

time_component_builtin!(BuiltinTimeYear, year);
time_component_builtin!(BuiltinTimeMonth, month);
time_component_builtin!(BuiltinTimeDay, day);
time_component_builtin!(BuiltinTimeHour, hour);
time_component_builtin!(BuiltinTimeMinute, minute);
time_component_builtin!(BuiltinTimeSecond, second);

#[derive(Debug, Clone)]
pub struct BuiltinTimeWeekday;

impl Callable for BuiltinTimeWeekday {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let timestamp = match &arguments[0] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "time_weekday() expects a numeric timestamp.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let dt = DateTime::<Utc>::from_timestamp(timestamp as i64, 0);
        match dt {
            Some(datetime) => Ok(Value::Number(datetime.weekday().num_days_from_monday() as f64)),
            None => Err(RuntimeError::new(
                "Invalid timestamp.".to_string(),
                None,
                "<builtin>",
            )),
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinTimeAdd;

impl Callable for BuiltinTimeAdd {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let timestamp = match &arguments[0] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "time_add() expects two numeric arguments.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let seconds = match &arguments[1] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "time_add() expects two numeric arguments.".to_string(),
                None,
                "<builtin>",
            )),
        };

        Ok(Value::Number(timestamp + seconds))
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinTimeDiff;

impl Callable for BuiltinTimeDiff {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let t1 = match &arguments[0] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "time_diff() expects two numeric timestamps.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let t2 = match &arguments[1] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "time_diff() expects two numeric timestamps.".to_string(),
                None,
                "<builtin>",
            )),
        };

        Ok(Value::Number(t1 - t2))
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinSleep;

impl Callable for BuiltinSleep {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let duration = match &arguments[0] {
            Value::Number(n) => *n,
            _ => return Err(RuntimeError::new(
                "sleep() expects a numeric duration in seconds.".to_string(),
                None,
                "<builtin>",
            )),
        };

        std::thread::sleep(std::time::Duration::from_secs_f64(duration));
        Ok(Value::Nil)
    }
}

// File operations
#[derive(Debug, Clone)]
pub struct BuiltinFOpen;

impl Callable for BuiltinFOpen {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let filename = match &arguments[0] {
            Value::String(s) => s,
            _ => return Err(RuntimeError::new(
                "fopen() expects two string arguments.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let mode = match &arguments[1] {
            Value::String(s) => s,
            _ => return Err(RuntimeError::new(
                "fopen() expects two string arguments.".to_string(),
                None,
                "<builtin>",
            )),
        };

        match std::fs::OpenOptions::new()
            .read(mode.contains('r'))
            .write(mode.contains('w'))
            .append(mode.contains('a'))
            .create(mode.contains('w') || mode.contains('a'))
            .truncate(mode.contains('w') && !mode.contains('a'))
            .open(filename)
        {
            Ok(file) => Ok(Value::FileHandle(std::rc::Rc::new(std::cell::RefCell::new(file)))),
            Err(_) => Ok(Value::Nil),
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinFClose;

impl Callable for BuiltinFClose {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        match &arguments[0] {
            Value::FileHandle(_) => {
                // Rust automatically closes files when they go out of scope
                Ok(Value::Number(0.0))
            }
            _ => Ok(Value::Number(-1.0)),
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinFWrite;

impl Callable for BuiltinFWrite {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, mut arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let content = stringify_value(&arguments[1]);
        
        if let Value::FileHandle(ref file) = arguments[0] {
            let mut file_borrowed = file.borrow_mut();
            match file_borrowed.write_all(content.as_bytes()) {
                Ok(_) => {
                    let _ = file_borrowed.flush();
                    Ok(Value::Number(content.len() as f64))
                }
                Err(_) => Ok(Value::Number(-1.0)),
            }
        } else {
            Ok(Value::Number(-1.0))
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinFRead;

impl Callable for BuiltinFRead {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, mut arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        if let Value::FileHandle(ref file) = arguments[0] {
            let mut file_borrowed = file.borrow_mut();
            let mut contents = String::new();
            match file_borrowed.read_to_string(&mut contents) {
                Ok(_) => Ok(Value::String(contents)),
                Err(_) => Ok(Value::Nil),
            }
        } else {
            Ok(Value::Nil)
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinFReadLine;

impl Callable for BuiltinFReadLine {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        if let Value::FileHandle(ref file) = arguments[0] {
            // Simple approach: read one byte at a time until newline
            let mut file_borrowed = file.borrow_mut();
            let mut line = String::new();
            let mut buffer = [0; 1];
            
            loop {
                match file_borrowed.read(&mut buffer) {
                    Ok(0) => break, // EOF
                    Ok(_) => {
                        let ch = buffer[0] as char;
                        if ch == '\n' {
                            break;
                        } else if ch != '\r' {
                            line.push(ch);
                        }
                    }
                    Err(_) => return Ok(Value::Nil),
                }
            }
            
            if line.is_empty() {
                Ok(Value::Nil)
            } else {
                Ok(Value::String(line))
            }
        } else {
            Ok(Value::Nil)
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinFWriteLine;

impl Callable for BuiltinFWriteLine {
    fn arity(&self) -> i32 {
        2
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, mut arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let content = stringify_value(&arguments[1]) + "\n";
        
        if let Value::FileHandle(ref file) = arguments[0] {
            let mut file_borrowed = file.borrow_mut();
            match file_borrowed.write_all(content.as_bytes()) {
                Ok(_) => {
                    let _ = file_borrowed.flush();
                    Ok(Value::Number(content.len() as f64))
                }
                Err(_) => Ok(Value::Number(-1.0)),
            }
        } else {
            Ok(Value::Number(-1.0))
        }
    }
}

#[derive(Debug, Clone)]
pub struct BuiltinFExists;

impl Callable for BuiltinFExists {
    fn arity(&self) -> i32 {
        1
    }

    fn call(&self, _interpreter: &mut crate::interpreter::Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        let filename = match &arguments[0] {
            Value::String(s) => s,
            _ => return Err(RuntimeError::new(
                "fexists() expects a string argument.".to_string(),
                None,
                "<builtin>",
            )),
        };

        let exists = std::path::Path::new(filename).is_file();
        Ok(Value::Boolean(exists))
    }
}

pub fn stringify_value(value: &Value) -> String {
    match value {
        Value::Nil => "nil".to_string(),
        Value::Boolean(b) => if *b { "true".to_string() } else { "false".to_string() },
        Value::Number(n) => {
            if n.fract() == 0.0 {
                format!("{}", *n as i64)
            } else {
                format!("{}", n)
            }
        }
        Value::String(s) => s.clone(),
        Value::List(list) => {
            let list_borrowed = list.borrow();
            let elements: Vec<String> = list_borrowed.iter().map(stringify_value).collect();
            format!("[{}]", elements.join(", "))
        }
        Value::Function(_) => "<fn>".to_string(),
        Value::Builtin(_) => "<native fn>".to_string(),
        Value::FileHandle(_) => "<file>".to_string(),
    }
}
