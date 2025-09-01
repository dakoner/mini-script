use std::collections::HashMap;
use std::fs::File;
use std::rc::Rc;
use std::cell::RefCell;
use crate::lexer::{Token, TokenType, LiteralValue};
use crate::ast::{Stmt, Expr};
use crate::error::RuntimeError;
use crate::builtin::*;

#[derive(Debug, Clone)]
pub enum Value {
    Nil,
    Boolean(bool),
    Number(f64),
    String(String),
    List(Rc<RefCell<Vec<Value>>>),
    Function(MiniScriptFunction),
    Builtin(&'static str), // Just store function name as string for simplicity
    FileHandle(Rc<RefCell<File>>),
}

impl PartialEq for Value {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (Value::Nil, Value::Nil) => true,
            (Value::Boolean(a), Value::Boolean(b)) => a == b,
            (Value::Number(a), Value::Number(b)) => a == b,
            (Value::String(a), Value::String(b)) => a == b,
            (Value::FileHandle(_), Value::FileHandle(_)) => false, // File handles are never equal
            _ => false,
        }
    }
}

pub trait Callable: std::fmt::Debug {
    fn arity(&self) -> i32;
    fn call(&self, interpreter: &mut Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError>;
}

#[derive(Debug, Clone)]
pub struct MiniScriptFunction {
    pub declaration: Stmt,
    pub closure: Rc<RefCell<Environment>>,
}

impl MiniScriptFunction {
    pub fn new(declaration: Stmt, closure: Rc<RefCell<Environment>>) -> Self {
        Self { declaration, closure }
    }
}

impl Callable for MiniScriptFunction {
    fn arity(&self) -> i32 {
        if let Stmt::Function { params, .. } = &self.declaration {
            params.len() as i32
        } else {
            0
        }
    }

    fn call(&self, interpreter: &mut Interpreter, arguments: Vec<Value>) -> Result<Value, RuntimeError> {
        if let Stmt::Function { params, body, .. } = &self.declaration {
            let environment = Rc::new(RefCell::new(Environment::new(Some(self.closure.clone()))));
            
            for (i, param) in params.iter().enumerate() {
                environment.borrow_mut().define(&param.lexeme, arguments[i].clone());
            }

            let previous = interpreter.environment.clone();
            interpreter.environment = environment;

            let mut result = Ok(Value::Nil);
            for statement in body {
                match interpreter.execute(statement) {
                    Ok(_) => {}
                    Err(e) => {
                        if e.message == "RETURN_VALUE" && e.return_value.is_some() {
                            // Extract return value directly
                            result = Ok(e.return_value.unwrap());
                        } else {
                            result = Err(e);
                        }
                        break;
                    }
                }
            }

            interpreter.environment = previous;
            result
        } else {
            Err(RuntimeError::new(
                "Invalid function declaration".to_string(),
                None,
                "<unknown>",
            ))
        }
    }
}

#[derive(Debug, Clone)]
pub struct Environment {
    values: HashMap<String, Value>,
    enclosing: Option<Rc<RefCell<Environment>>>,
}

impl Environment {
    pub fn new(enclosing: Option<Rc<RefCell<Environment>>>) -> Self {
        Self {
            values: HashMap::new(),
            enclosing,
        }
    }

    pub fn define(&mut self, name: &str, value: Value) {
        self.values.insert(name.to_string(), value);
    }

    pub fn get(&self, name_token: &Token) -> Result<Value, RuntimeError> {
        let name = &name_token.lexeme;
        if let Some(value) = self.values.get(name) {
            Ok(value.clone())
        } else if let Some(enclosing) = &self.enclosing {
            enclosing.borrow().get(name_token)
        } else {
            Err(RuntimeError::new(
                format!("Undefined variable '{}'.", name),
                Some(name_token.line),
                "<unknown>",
            ))
        }
    }

    pub fn assign(&mut self, name_token: &Token, value: Value) -> Result<(), RuntimeError> {
        let name = &name_token.lexeme;
        
        // If variable exists in current scope, update it
        if self.values.contains_key(name) {
            self.values.insert(name.to_string(), value);
            return Ok(());
        }
        
        // Try to assign in enclosing scope recursively
        if let Some(enclosing) = &self.enclosing {
            return enclosing.borrow_mut().assign(name_token, value);
        }
        
        // If not found anywhere, create in current scope (implicit declaration)
        self.values.insert(name.to_string(), value);
        Ok(())
    }
}

pub struct Interpreter {
    pub globals: Rc<RefCell<Environment>>,
    pub environment: Rc<RefCell<Environment>>,
    pub filename: String,
}

impl Interpreter {
    pub fn new(filename: &str) -> Self {
        let globals = Rc::new(RefCell::new(Environment::new(None)));
        
        // Define built-in functions (simplified)
        globals.borrow_mut().define("print", Value::Builtin("print"));
        globals.borrow_mut().define("len", Value::Builtin("len"));
        globals.borrow_mut().define("time_now", Value::Builtin("time_now"));
        globals.borrow_mut().define("time_format", Value::Builtin("time_format"));
        globals.borrow_mut().define("time_parse", Value::Builtin("time_parse"));
        globals.borrow_mut().define("time_diff", Value::Builtin("time_diff"));
        globals.borrow_mut().define("time_year", Value::Builtin("time_year"));
        globals.borrow_mut().define("time_month", Value::Builtin("time_month"));
        globals.borrow_mut().define("time_day", Value::Builtin("time_day"));
        globals.borrow_mut().define("time_hour", Value::Builtin("time_hour"));
        globals.borrow_mut().define("time_minute", Value::Builtin("time_minute"));
        globals.borrow_mut().define("time_second", Value::Builtin("time_second"));
        globals.borrow_mut().define("time_weekday", Value::Builtin("time_weekday"));
        globals.borrow_mut().define("time_add", Value::Builtin("time_add"));
        globals.borrow_mut().define("sleep", Value::Builtin("sleep"));
        globals.borrow_mut().define("fopen", Value::Builtin("fopen"));
        globals.borrow_mut().define("fclose", Value::Builtin("fclose"));
        globals.borrow_mut().define("fwrite", Value::Builtin("fwrite"));
        globals.borrow_mut().define("fread", Value::Builtin("fread"));
        globals.borrow_mut().define("freadline", Value::Builtin("freadline"));
        globals.borrow_mut().define("fwriteline", Value::Builtin("fwriteline"));
        globals.borrow_mut().define("fexists", Value::Builtin("fexists"));

        let environment = globals.clone();

        Self {
            globals,
            environment,
            filename: filename.to_string(),
        }
    }

    pub fn interpret(&mut self, statements: &[Stmt]) -> Result<(), RuntimeError> {
        for statement in statements {
            self.execute(statement)?;
        }
        Ok(())
    }

    fn execute(&mut self, stmt: &Stmt) -> Result<(), RuntimeError> {
        match stmt {
            Stmt::Expression { expression } => {
                self.evaluate(expression)?;
                Ok(())
            }
            Stmt::Print { expressions } => {
                let mut values = Vec::new();
                for expr in expressions {
                    values.push(self.evaluate(expr)?);
                }
                let output: Vec<String> = values.iter().map(|v| stringify_value(v)).collect();
                println!("{}", output.join(" "));
                Ok(())
            }
            Stmt::Var { name, initializer } => {
                let value = if let Some(init) = initializer {
                    self.evaluate(init)?
                } else {
                    Value::Nil
                };
                self.environment.borrow_mut().define(&name.lexeme, value);
                Ok(())
            }
            Stmt::Block { statements } => {
                let environment = Rc::new(RefCell::new(Environment::new(Some(self.environment.clone()))));
                self.execute_block(statements, environment)?;
                Ok(())
            }
            Stmt::Function { name, .. } => {
                let function = MiniScriptFunction::new(stmt.clone(), self.environment.clone());
                self.environment.borrow_mut().define(&name.lexeme, Value::Function(function));
                Ok(())
            }
            Stmt::If { condition, then_branch, else_branch } => {
                let condition_result = self.evaluate(condition)?;
                if self.is_truthy(&condition_result) {
                    self.execute(then_branch)?;
                } else if let Some(else_stmt) = else_branch {
                    self.execute(else_stmt)?;
                }
                Ok(())
            }
            Stmt::Return { value, .. } => {
                let return_value = if let Some(val) = value {
                    self.evaluate(val)?
                } else {
                    Value::Nil
                };
                // Use RuntimeError with return_value to properly handle all value types
                Err(RuntimeError::with_return_value(
                    "RETURN_VALUE".to_string(),
                    None,
                    &self.filename,
                    return_value,
                ))
            }
            Stmt::While { condition, body } => {
                loop {
                    let condition_result = self.evaluate(condition)?;
                    if !self.is_truthy(&condition_result) {
                        break;
                    }
                    self.execute(body)?;
                }
                Ok(())
            }
            Stmt::Assert { condition, message, keyword } => {
                let condition_result = self.evaluate(condition)?;
                if !self.is_truthy(&condition_result) {
                    let msg_val = self.evaluate(message)?;
                    return Err(RuntimeError::new(
                        format!("Assertion failed: {}", stringify_value(&msg_val)),
                        Some(keyword.line),
                        &self.filename,
                    ));
                }
                Ok(())
            }
            Stmt::Import { path_token, .. } => {
                let module_path = if let Some(LiteralValue::String(path)) = &path_token.literal {
                    path.clone()
                } else {
                    return Err(RuntimeError::new(
                        "Import path must be a string".to_string(),
                        Some(path_token.line),
                        &self.filename,
                    ));
                };

                let full_path = self.resolve_module_path(&module_path)?;
                self.run_file(&full_path)?;
                Ok(())
            }
        }
    }

    pub fn execute_block(&mut self, statements: &[Stmt], environment: Rc<RefCell<Environment>>) -> Result<(), RuntimeError> {
        let previous = self.environment.clone();
        self.environment = environment;

        let mut result = Ok(());
        for statement in statements {
            if let Err(e) = self.execute(statement) {
                result = Err(e);
                break;
            }
        }

        self.environment = previous;
        result
    }

    fn evaluate(&mut self, expr: &Expr) -> Result<Value, RuntimeError> {
        match expr {
            Expr::Literal { value } => {
                match value {
                    LiteralValue::Boolean(b) => Ok(Value::Boolean(*b)),
                    LiteralValue::Number(n) => Ok(Value::Number(*n)),
                    LiteralValue::Integer(i) => Ok(Value::Number(*i as f64)),
                    LiteralValue::String(s) => Ok(Value::String(s.clone())),
                    LiteralValue::Char(c) => Ok(Value::String(c.to_string())),
                    LiteralValue::Nil => Ok(Value::Nil),
                }
            }
            Expr::ListLiteral { elements } => {
                let mut list = Vec::new();
                for elem in elements {
                    list.push(self.evaluate(elem)?);
                }
                Ok(Value::List(Rc::new(RefCell::new(list))))
            }
            Expr::Variable { name } => {
                self.environment.borrow().get(name)
            }
            Expr::Assign { name, value } => {
                let val = self.evaluate(value)?;
                self.environment.borrow_mut().assign(name, val.clone())?;
                Ok(val)
            }
            Expr::Grouping { expression } => {
                self.evaluate(expression)
            }
            Expr::Unary { operator, right } => {
                let right_val = self.evaluate(right)?;
                match operator.token_type {
                    TokenType::Minus => {
                        if let Value::Number(n) = right_val {
                            Ok(Value::Number(-n))
                        } else {
                            Err(RuntimeError::new(
                                "Operand must be a number.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::Not => {
                        Ok(Value::Boolean(!self.is_truthy(&right_val)))
                    }
                    _ => Err(RuntimeError::new(
                        "Unknown unary operator.".to_string(),
                        Some(operator.line),
                        &self.filename,
                    ))
                }
            }
            Expr::Binary { left, operator, right } => {
                let left_val = self.evaluate(left)?;
                let right_val = self.evaluate(right)?;

                match operator.token_type {
                    TokenType::Plus => {
                        match (&left_val, &right_val) {
                            (Value::Number(l), Value::Number(r)) => Ok(Value::Number(l + r)),
                            _ => {
                                // String concatenation
                                let left_str = stringify_value(&left_val);
                                let right_str = stringify_value(&right_val);
                                Ok(Value::String(left_str + &right_str))
                            }
                        }
                    }
                    TokenType::Minus => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            Ok(Value::Number(l - r))
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::Multiply => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            Ok(Value::Number(l * r))
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::Divide => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            if *r == 0.0 {
                                Err(RuntimeError::new(
                                    "Division by zero.".to_string(),
                                    Some(operator.line),
                                    &self.filename,
                                ))
                            } else {
                                Ok(Value::Number(l / r))
                            }
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::Greater => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            Ok(Value::Boolean(l > r))
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::GreaterEqual => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            Ok(Value::Boolean(l >= r))
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::Less => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            Ok(Value::Boolean(l < r))
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::LessEqual => {
                        if let (Value::Number(l), Value::Number(r)) = (&left_val, &right_val) {
                            Ok(Value::Boolean(l <= r))
                        } else {
                            Err(RuntimeError::new(
                                "Operands must be numbers.".to_string(),
                                Some(operator.line),
                                &self.filename,
                            ))
                        }
                    }
                    TokenType::Equal => {
                        Ok(Value::Boolean(self.is_equal(&left_val, &right_val)))
                    }
                    TokenType::NotEqual => {
                        Ok(Value::Boolean(!self.is_equal(&left_val, &right_val)))
                    }
                    _ => Err(RuntimeError::new(
                        "Unknown binary operator.".to_string(),
                        Some(operator.line),
                        &self.filename,
                    ))
                }
            }
            Expr::Logical { left, operator, right } => {
                let left_val = self.evaluate(left)?;

                match operator.token_type {
                    TokenType::Or => {
                        if self.is_truthy(&left_val) {
                            Ok(Value::Boolean(true))
                        } else {
                            let right_val = self.evaluate(right)?;
                            Ok(Value::Boolean(self.is_truthy(&right_val)))
                        }
                    }
                    TokenType::And => {
                        if !self.is_truthy(&left_val) {
                            Ok(Value::Boolean(false))
                        } else {
                            let right_val = self.evaluate(right)?;
                            Ok(Value::Boolean(self.is_truthy(&right_val)))
                        }
                    }
                    _ => Err(RuntimeError::new(
                        "Unknown logical operator.".to_string(),
                        Some(operator.line),
                        &self.filename,
                    ))
                }
            }
            Expr::Call { callee, paren, arguments } => {
                let callee_val = self.evaluate(callee)?;
                let mut args = Vec::new();
                for arg in arguments {
                    args.push(self.evaluate(arg)?);
                }

                match callee_val {
                    Value::Function(func) => {
                        if func.arity() != -1 && args.len() != func.arity() as usize {
                            return Err(RuntimeError::new(
                                format!("Expected {} args but got {}.", func.arity(), args.len()),
                                Some(paren.line),
                                &self.filename,
                            ));
                        }
                        func.call(self, args)
                    }
                    Value::Builtin(name) => {
                        self.call_builtin(name, args, paren.line)
                    }
                    _ => Err(RuntimeError::new(
                        "Can only call functions and classes.".to_string(),
                        Some(paren.line),
                        &self.filename,
                    ))
                }
            }
            Expr::Get { object, index } => {
                let obj_val = self.evaluate(object)?;
                let index_val = self.evaluate(index)?;

                match obj_val {
                    Value::List(list) => {
                        if let Value::Number(idx) = index_val {
                            let i = idx as usize;
                            let list_borrowed = list.borrow();
                            if i < list_borrowed.len() {
                                Ok(list_borrowed[i].clone())
                            } else {
                                Err(RuntimeError::new(
                                    "List index out of range.".to_string(),
                                    None,
                                    &self.filename,
                                ))
                            }
                        } else {
                            Err(RuntimeError::new(
                                "List index must be an integer.".to_string(),
                                None,
                                &self.filename,
                            ))
                        }
                    }
                    _ => Err(RuntimeError::new(
                        "Can only index lists.".to_string(),
                        None,
                        &self.filename,
                    ))
                }
            }
            Expr::Set { object, index, value } => {
                let obj_val = self.evaluate(object)?;
                let index_val = self.evaluate(index)?;
                let new_value = self.evaluate(value)?;

                match obj_val {
                    Value::List(list) => {
                        if let Value::Number(idx) = index_val {
                            let i = idx as usize;
                            let mut list_borrowed = list.borrow_mut();
                            if i < list_borrowed.len() {
                                list_borrowed[i] = new_value.clone();
                                Ok(new_value)
                            } else {
                                Err(RuntimeError::new(
                                    "List index out of range.".to_string(),
                                    None,
                                    &self.filename,
                                ))
                            }
                        } else {
                            Err(RuntimeError::new(
                                "List index must be an integer.".to_string(),
                                None,
                                &self.filename,
                            ))
                        }
                    }
                    _ => Err(RuntimeError::new(
                        "Can only set elements of lists.".to_string(),
                        None,
                        &self.filename,
                    ))
                }
            }
        }
    }

    fn call_builtin(&mut self, name: &str, args: Vec<Value>, line: usize) -> Result<Value, RuntimeError> {
        match name {
            "print" => {
                let output: Vec<String> = args.iter().map(|v| stringify_value(v)).collect();
                println!("{}", output.join(" "));
                Ok(Value::Nil)
            }
            "len" => {
                if args.len() != 1 {
                    return Err(RuntimeError::new(
                        "len() expects 1 argument.".to_string(),
                        Some(line),
                        &self.filename,
                    ));
                }
                match &args[0] {
                    Value::String(s) => Ok(Value::Number(s.len() as f64)),
                    Value::List(list) => Ok(Value::Number(list.borrow().len() as f64)),
                    _ => Err(RuntimeError::new(
                        "len() expects a string or a list.".to_string(),
                        Some(line),
                        &self.filename,
                    )),
                }
            }
            "time_parse" => {
                use crate::builtin::BuiltinTimeParse;
                let parser = BuiltinTimeParse;
                parser.call(self, args)
            }
            "time_format" => {
                use crate::builtin::BuiltinTimeFormat;
                let formatter = BuiltinTimeFormat;
                formatter.call(self, args)
            }
            "time_now" => {
                use crate::builtin::BuiltinTimeNow;
                let now_fn = BuiltinTimeNow;
                now_fn.call(self, args)
            }
            "time_year" => {
                use crate::builtin::BuiltinTimeYear;
                let year_fn = BuiltinTimeYear;
                year_fn.call(self, args)
            }
            "time_month" => {
                use crate::builtin::BuiltinTimeMonth;
                let month_fn = BuiltinTimeMonth;
                month_fn.call(self, args)
            }
            "time_day" => {
                use crate::builtin::BuiltinTimeDay;
                let day_fn = BuiltinTimeDay;
                day_fn.call(self, args)
            }
            "time_hour" => {
                use crate::builtin::BuiltinTimeHour;
                let hour_fn = BuiltinTimeHour;
                hour_fn.call(self, args)
            }
            "time_minute" => {
                use crate::builtin::BuiltinTimeMinute;
                let minute_fn = BuiltinTimeMinute;
                minute_fn.call(self, args)
            }
            "time_second" => {
                use crate::builtin::BuiltinTimeSecond;
                let second_fn = BuiltinTimeSecond;
                second_fn.call(self, args)
            }
            "time_weekday" => {
                use crate::builtin::BuiltinTimeWeekday;
                let weekday_fn = BuiltinTimeWeekday;
                weekday_fn.call(self, args)
            }
            "time_add" => {
                use crate::builtin::BuiltinTimeAdd;
                let add_fn = BuiltinTimeAdd;
                add_fn.call(self, args)
            }
            "time_diff" => {
                use crate::builtin::BuiltinTimeDiff;
                let diff_fn = BuiltinTimeDiff;
                diff_fn.call(self, args)
            }
            "sleep" => {
                use crate::builtin::BuiltinSleep;
                let sleep_fn = BuiltinSleep;
                sleep_fn.call(self, args)
            }
            "fopen" => {
                use crate::builtin::BuiltinFOpen;
                let open_fn = BuiltinFOpen;
                open_fn.call(self, args)
            }
            "fclose" => {
                use crate::builtin::BuiltinFClose;
                let close_fn = BuiltinFClose;
                close_fn.call(self, args)
            }
            "fwrite" => {
                use crate::builtin::BuiltinFWrite;
                let write_fn = BuiltinFWrite;
                write_fn.call(self, args)
            }
            "fread" => {
                use crate::builtin::BuiltinFRead;
                let read_fn = BuiltinFRead;
                read_fn.call(self, args)
            }
            "freadline" => {
                use crate::builtin::BuiltinFReadLine;
                let readline_fn = BuiltinFReadLine;
                readline_fn.call(self, args)
            }
            "fwriteline" => {
                use crate::builtin::BuiltinFWriteLine;
                let writeline_fn = BuiltinFWriteLine;
                writeline_fn.call(self, args)
            }
            "fexists" => {
                use crate::builtin::BuiltinFExists;
                let exists_fn = BuiltinFExists;
                exists_fn.call(self, args)
            }
            _ => {
                // For now, just return nil for unimplemented built-ins
                Ok(Value::Nil)
            }
        }
    }

    fn is_truthy(&self, value: &Value) -> bool {
        match value {
            Value::Nil => false,
            Value::Boolean(b) => *b,
            Value::Number(n) => *n != 0.0,
            _ => true,
        }
    }

    fn is_equal(&self, a: &Value, b: &Value) -> bool {
        a == b
    }

    fn value_to_string(&self, value: &Value) -> String {
        match value {
            Value::String(s) => format!("\"{}\"", s),
            Value::Number(n) => n.to_string(),
            Value::Boolean(b) => b.to_string(),
            Value::Nil => "nil".to_string(),
            _ => "complex".to_string(),
        }
    }

    pub fn parse_return_value(&self, s: &str) -> Value {
        if s == "nil" {
            Value::Nil
        } else if s == "true" {
            Value::Boolean(true)
        } else if s == "false" {
            Value::Boolean(false)
        } else if s.starts_with('"') && s.ends_with('"') {
            Value::String(s[1..s.len()-1].to_string())
        } else if let Ok(n) = s.parse::<f64>() {
            Value::Number(n)
        } else {
            Value::Nil
        }
    }

    fn resolve_module_path(&self, module_path: &str) -> Result<String, RuntimeError> {
        let mut search_paths = Vec::new();

        // 1. Path relative to the current script file
        if self.filename != "<REPL>" && self.filename != "<unknown>" {
            if let Some(parent) = std::path::Path::new(&self.filename).parent() {
                search_paths.push(parent.to_path_buf());
            }
        }

        // 2. Current working directory
        if let Ok(cwd) = std::env::current_dir() {
            search_paths.push(cwd);
        }

        // 3. MODULESPATH environment variable
        if let Ok(modules_path) = std::env::var("MODULESPATH") {
            for path in modules_path.split(';') {
                search_paths.push(std::path::PathBuf::from(path));
            }
        }

        for base_dir in search_paths {
            // Try the path as is
            let test_path = base_dir.join(module_path);
            if test_path.is_file() {
                return Ok(test_path.to_string_lossy().to_string());
            }

            // Try adding .ms extension
            if !module_path.ends_with(".ms") {
                let test_path_ext = base_dir.join(format!("{}.ms", module_path));
                if test_path_ext.is_file() {
                    return Ok(test_path_ext.to_string_lossy().to_string());
                }
            }
        }

        Err(RuntimeError::new(
            format!("Cannot find module: {}", module_path),
            None,
            &self.filename,
        ))
    }

    fn run_file(&mut self, path: &str) -> Result<(), RuntimeError> {
        let source = std::fs::read_to_string(path)
            .map_err(|_| RuntimeError::new(
                format!("Could not read file: {}", path),
                None,
                &self.filename,
            ))?;

        crate::run(&source, path, Some(self))
    }
}
