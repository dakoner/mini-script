use std::collections::HashMap;
use crate::error::RuntimeError;

#[derive(Debug, Clone, PartialEq)]
pub enum TokenType {
    // Single-character tokens
    LeftParen, RightParen, LeftBrace, RightBrace, LeftBracket, RightBracket,
    Comma, Dot, Minus, Plus, Semicolon, Divide, Multiply,

    // One or two character tokens
    Not, NotEqual,
    Assign, Equal,
    Greater, GreaterEqual,
    Less, LessEqual,
    And, Or,

    // Literals
    Identifier, String, Number, Char,

    // Keywords
    Print, Else, False, For, Function, If, Return, True, While, Import, From,
    IntType, FloatType, CharType, StringType, List, Map,
    Loadlib, Getproc, Freelib, Callext,
    Assert, Var, Nil,

    Eof,
}

#[derive(Debug, Clone)]
pub struct Token {
    pub token_type: TokenType,
    pub lexeme: String,
    pub literal: Option<LiteralValue>,
    pub line: usize,
}

#[derive(Debug, Clone, PartialEq)]
pub enum LiteralValue {
    String(String),
    Number(f64),
    Integer(i64),
    Boolean(bool),
    Char(char),
    Nil,
}

impl Token {
    pub fn new(token_type: TokenType, lexeme: String, literal: Option<LiteralValue>, line: usize) -> Self {
        Self {
            token_type,
            lexeme,
            literal,
            line,
        }
    }
}

pub struct Lexer {
    source: Vec<char>,
    filename: String,
    tokens: Vec<Token>,
    start: usize,
    current: usize,
    line: usize,
}

impl Lexer {
    pub fn new(source: &str, filename: &str) -> Self {
        Self {
            source: source.chars().collect(),
            filename: filename.to_string(),
            tokens: Vec::new(),
            start: 0,
            current: 0,
            line: 1,
        }
    }

    pub fn scan_tokens(&mut self) -> Result<Vec<Token>, RuntimeError> {
        while !self.is_at_end() {
            self.start = self.current;
            self.scan_token()?;
        }

        self.tokens.push(Token::new(TokenType::Eof, String::new(), None, self.line));
        Ok(self.tokens.clone())
    }

    fn is_at_end(&self) -> bool {
        self.current >= self.source.len()
    }

    fn advance(&mut self) -> char {
        let ch = self.source[self.current];
        self.current += 1;
        ch
    }

    fn peek(&self) -> char {
        if self.is_at_end() {
            '\0'
        } else {
            self.source[self.current]
        }
    }

    fn peek_next(&self) -> char {
        if self.current + 1 >= self.source.len() {
            '\0'
        } else {
            self.source[self.current + 1]
        }
    }

    fn match_char(&mut self, expected: char) -> bool {
        if self.is_at_end() || self.source[self.current] != expected {
            false
        } else {
            self.current += 1;
            true
        }
    }

    fn add_token(&mut self, token_type: TokenType, literal: Option<LiteralValue>) {
        let text: String = self.source[self.start..self.current].iter().collect();
        self.tokens.push(Token::new(token_type, text, literal, self.line));
    }

    fn string(&mut self) -> Result<(), RuntimeError> {
        while self.peek() != '"' && !self.is_at_end() {
            if self.peek() == '\n' {
                self.line += 1;
            }
            self.advance();
        }

        if self.is_at_end() {
            return Err(RuntimeError::new(
                "Unterminated string.".to_string(),
                Some(self.line),
                &self.filename,
            ));
        }

        self.advance(); // The closing "
        let value: String = self.source[self.start + 1..self.current - 1].iter().collect();
        self.add_token(TokenType::String, Some(LiteralValue::String(value)));
        Ok(())
    }

    fn number(&mut self) {
        let mut is_float = false;
        while self.peek().is_ascii_digit() {
            self.advance();
        }

        if self.peek() == '.' && self.peek_next().is_ascii_digit() {
            is_float = true;
            self.advance(); // Consume the "."
            while self.peek().is_ascii_digit() {
                self.advance();
            }
        }

        let text: String = self.source[self.start..self.current].iter().collect();
        if is_float {
            let value: f64 = text.parse().unwrap();
            self.add_token(TokenType::Number, Some(LiteralValue::Number(value)));
        } else {
            let value: i64 = text.parse().unwrap();
            self.add_token(TokenType::Number, Some(LiteralValue::Integer(value)));
        }
    }

    fn identifier(&mut self) {
        while self.peek().is_alphanumeric() || self.peek() == '_' {
            self.advance();
        }

        let text: String = self.source[self.start..self.current].iter().collect();
        let token_type = self.get_keyword(&text).unwrap_or(TokenType::Identifier);
        
        let literal = match token_type {
            TokenType::True => Some(LiteralValue::Boolean(true)),
            TokenType::False => Some(LiteralValue::Boolean(false)),
            TokenType::Nil => Some(LiteralValue::Nil),
            _ => None,
        };

        self.add_token(token_type, literal);
    }

    fn get_keyword(&self, text: &str) -> Option<TokenType> {
        let mut keywords = HashMap::new();
        keywords.insert("print", TokenType::Print);
        keywords.insert("if", TokenType::If);
        keywords.insert("else", TokenType::Else);
        keywords.insert("while", TokenType::While);
        keywords.insert("for", TokenType::For);
        keywords.insert("function", TokenType::Function);
        keywords.insert("return", TokenType::Return);
        keywords.insert("true", TokenType::True);
        keywords.insert("false", TokenType::False);
        keywords.insert("import", TokenType::Import);
        keywords.insert("from", TokenType::From);
        keywords.insert("int", TokenType::IntType);
        keywords.insert("float", TokenType::FloatType);
        keywords.insert("char", TokenType::CharType);
        keywords.insert("string", TokenType::StringType);
        keywords.insert("list", TokenType::List);
        keywords.insert("map", TokenType::Map);
        keywords.insert("loadlib", TokenType::Loadlib);
        keywords.insert("getproc", TokenType::Getproc);
        keywords.insert("freelib", TokenType::Freelib);
        keywords.insert("callext", TokenType::Callext);
        keywords.insert("assert", TokenType::Assert);
        keywords.insert("var", TokenType::Var);
        keywords.insert("nil", TokenType::Nil);

        keywords.get(text).cloned()
    }

    fn scan_token(&mut self) -> Result<(), RuntimeError> {
        let c = self.advance();

        match c {
            ' ' | '\r' | '\t' => {}, // Ignore whitespace
            '\n' => self.line += 1,
            '(' => self.add_token(TokenType::LeftParen, None),
            ')' => self.add_token(TokenType::RightParen, None),
            '{' => self.add_token(TokenType::LeftBrace, None),
            '}' => self.add_token(TokenType::RightBrace, None),
            '[' => self.add_token(TokenType::LeftBracket, None),
            ']' => self.add_token(TokenType::RightBracket, None),
            ',' => self.add_token(TokenType::Comma, None),
            '.' => self.add_token(TokenType::Dot, None),
            '-' => self.add_token(TokenType::Minus, None),
            '+' => self.add_token(TokenType::Plus, None),
            ';' => self.add_token(TokenType::Semicolon, None),
            '*' => self.add_token(TokenType::Multiply, None),
            '!' => {
                let token_type = if self.match_char('=') {
                    TokenType::NotEqual
                } else {
                    TokenType::Not
                };
                self.add_token(token_type, None);
            },
            '=' => {
                let token_type = if self.match_char('=') {
                    TokenType::Equal
                } else {
                    TokenType::Assign
                };
                self.add_token(token_type, None);
            },
            '<' => {
                let token_type = if self.match_char('=') {
                    TokenType::LessEqual
                } else {
                    TokenType::Less
                };
                self.add_token(token_type, None);
            },
            '>' => {
                let token_type = if self.match_char('=') {
                    TokenType::GreaterEqual
                } else {
                    TokenType::Greater
                };
                self.add_token(token_type, None);
            },
            '|' => {
                if self.match_char('|') {
                    self.add_token(TokenType::Or, None);
                } else {
                    return Err(RuntimeError::new(
                        "Unexpected character: |".to_string(),
                        Some(self.line),
                        &self.filename,
                    ));
                }
            },
            '/' => {
                if self.match_char('/') {
                    // A comment goes until the end of the line
                    while self.peek() != '\n' && !self.is_at_end() {
                        self.advance();
                    }
                } else {
                    self.add_token(TokenType::Divide, None);
                }
            },
            '&' => {
                if self.match_char('&') {
                    self.add_token(TokenType::And, None);
                } else {
                    return Err(RuntimeError::new(
                        "Unexpected character: &".to_string(),
                        Some(self.line),
                        &self.filename,
                    ));
                }
            },
            '"' => self.string()?,
            '\'' => {
                let char_val = self.advance();
                if self.advance() != '\'' {
                    return Err(RuntimeError::new(
                        "Unterminated character literal.".to_string(),
                        Some(self.line),
                        &self.filename,
                    ));
                }
                self.add_token(TokenType::Char, Some(LiteralValue::Char(char_val)));
            },
            _ => {
                if c.is_ascii_digit() {
                    self.number();
                } else if c.is_alphabetic() || c == '_' {
                    self.identifier();
                } else {
                    return Err(RuntimeError::new(
                        format!("Unexpected character: {}", c),
                        Some(self.line),
                        &self.filename,
                    ));
                }
            }
        }

        Ok(())
    }
}
