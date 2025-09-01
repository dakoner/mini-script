use crate::lexer::{Token, TokenType};
use crate::ast::{Stmt, Expr};
use crate::error::RuntimeError;

pub struct Parser {
    tokens: Vec<Token>,
    filename: String,
    current: usize,
}

impl Parser {
    pub fn new(tokens: Vec<Token>, filename: &str) -> Self {
        Self {
            tokens,
            filename: filename.to_string(),
            current: 0,
        }
    }

    pub fn parse(&mut self) -> Result<Vec<Stmt>, RuntimeError> {
        let mut statements = Vec::new();
        
        while !self.is_at_end() {
            if let Some(stmt) = self.declaration()? {
                statements.push(stmt);
            }
        }
        
        Ok(statements)
    }

    fn is_at_end(&self) -> bool {
        self.peek().token_type == TokenType::Eof
    }

    fn peek(&self) -> &Token {
        &self.tokens[self.current]
    }

    fn peek_next(&self) -> &Token {
        if self.current + 1 >= self.tokens.len() {
            &self.tokens[self.tokens.len() - 1]
        } else {
            &self.tokens[self.current + 1]
        }
    }

    fn previous(&self) -> &Token {
        &self.tokens[self.current - 1]
    }

    fn advance(&mut self) -> &Token {
        if !self.is_at_end() {
            self.current += 1;
        }
        self.previous()
    }

    fn check(&self, token_type: &TokenType) -> bool {
        if self.is_at_end() {
            false
        } else {
            &self.peek().token_type == token_type
        }
    }

    fn match_tokens(&mut self, types: &[TokenType]) -> bool {
        for token_type in types {
            if self.check(token_type) {
                self.advance();
                return true;
            }
        }
        false
    }

    fn consume(&mut self, token_type: TokenType, message: &str) -> Result<&Token, RuntimeError> {
        if self.check(&token_type) {
            Ok(self.advance())
        } else {
            Err(self.error(self.peek(), message))
        }
    }

    fn synchronize(&mut self) {
        self.advance();

        while !self.is_at_end() {
            if self.previous().token_type == TokenType::Semicolon {
                return;
            }

            match self.peek().token_type {
                TokenType::Function | TokenType::While | TokenType::For |
                TokenType::If | TokenType::Return | TokenType::Print => return,
                _ => {}
            }

            self.advance();
        }
    }

    fn declaration(&mut self) -> Result<Option<Stmt>, RuntimeError> {
        let result = if self.match_tokens(&[TokenType::Function]) {
            self.function_declaration()
        } else if self.match_tokens(&[TokenType::Var]) {
            self.var_declaration()
        } else {
            self.statement()
        };

        match result {
            Err(_) => {
                self.synchronize();
                Ok(None)
            }
            Ok(stmt) => Ok(Some(stmt))
        }
    }

    fn var_declaration(&mut self) -> Result<Stmt, RuntimeError> {
        let name = self.consume(TokenType::Identifier, "Expect variable name.")?.clone();
        
        let initializer = if self.match_tokens(&[TokenType::Assign]) {
            Some(self.expression()?)
        } else {
            None
        };

        self.consume(TokenType::Semicolon, "Expect ';' after variable declaration.")?;
        Ok(Stmt::Var { name, initializer })
    }

    fn function_declaration(&mut self) -> Result<Stmt, RuntimeError> {
        let name = self.consume(TokenType::Identifier, "Expect function name.")?.clone();
        self.consume(TokenType::LeftParen, "Expect '(' after function name.")?;

        let mut params = Vec::new();
        if !self.check(&TokenType::RightParen) {
            loop {
                if params.len() >= 255 {
                    return Err(self.error(self.peek(), "Can't have more than 255 parameters."));
                }
                params.push(self.consume(TokenType::Identifier, "Expect parameter name.")?.clone());
                
                if !self.match_tokens(&[TokenType::Comma]) {
                    break;
                }
            }
        }
        
        self.consume(TokenType::RightParen, "Expect ')' after parameters.")?;
        self.consume(TokenType::LeftBrace, "Expect '{' before function body.")?;
        let body = self.block()?;
        
        Ok(Stmt::Function { name, params, body })
    }

    fn statement(&mut self) -> Result<Stmt, RuntimeError> {
        if self.match_tokens(&[TokenType::Print]) {
            self.print_statement()
        } else if self.match_tokens(&[TokenType::Assert]) {
            self.assert_statement()
        } else if self.match_tokens(&[TokenType::Import]) {
            self.import_statement()
        } else if self.match_tokens(&[TokenType::LeftBrace]) {
            Ok(Stmt::Block { statements: self.block()? })
        } else if self.match_tokens(&[TokenType::Return]) {
            self.return_statement()
        } else if self.match_tokens(&[TokenType::While]) {
            self.while_statement()
        } else if self.match_tokens(&[TokenType::For]) {
            self.for_statement()
        } else if self.match_tokens(&[TokenType::If]) {
            self.if_statement()
        } else {
            self.expression_statement()
        }
    }

    fn print_statement(&mut self) -> Result<Stmt, RuntimeError> {
        let mut values = vec![self.expression()?];
        
        while self.match_tokens(&[TokenType::Comma]) {
            values.push(self.expression()?);
        }
        
        self.consume(TokenType::Semicolon, "Expect ';' after value.")?;
        Ok(Stmt::Print { expressions: values })
    }

    fn assert_statement(&mut self) -> Result<Stmt, RuntimeError> {
        let keyword = self.previous().clone();
        let condition = self.expression()?;
        self.consume(TokenType::Comma, "Expect ',' after assert condition.")?;
        let message = self.expression()?;
        self.consume(TokenType::Semicolon, "Expect ';' after assert message.")?;
        
        Ok(Stmt::Assert { keyword, condition, message })
    }

    fn import_statement(&mut self) -> Result<Stmt, RuntimeError> {
        let namespace = if self.check(&TokenType::Identifier) && self.peek_next().token_type == TokenType::From {
            let ns = self.consume(TokenType::Identifier, "Expect namespace for import.")?.clone();
            self.consume(TokenType::From, "Expect 'from' after namespace.")?;
            Some(ns)
        } else {
            None
        };

        let path_token = self.consume(TokenType::String, "Expect string path for import.")?.clone();
        self.consume(TokenType::Semicolon, "Expect ';' after import statement.")?;
        
        Ok(Stmt::Import { path_token, namespace })
    }

    fn expression_statement(&mut self) -> Result<Stmt, RuntimeError> {
        let expr = self.expression()?;
        self.consume(TokenType::Semicolon, "Expect ';' after expression.")?;
        Ok(Stmt::Expression { expression: expr })
    }

    fn return_statement(&mut self) -> Result<Stmt, RuntimeError> {
        let keyword = self.previous().clone();
        let value = if !self.check(&TokenType::Semicolon) {
            Some(self.expression()?)
        } else {
            None
        };
        
        self.consume(TokenType::Semicolon, "Expect ';' after return value.")?;
        Ok(Stmt::Return { keyword, value })
    }

    fn while_statement(&mut self) -> Result<Stmt, RuntimeError> {
        self.consume(TokenType::LeftParen, "Expect '(' after 'while'.")?;
        let condition = self.expression()?;
        self.consume(TokenType::RightParen, "Expect ')' after condition.")?;
        let body = Box::new(self.statement()?);
        
        Ok(Stmt::While { condition, body })
    }

    fn for_statement(&mut self) -> Result<Stmt, RuntimeError> {
        self.consume(TokenType::LeftParen, "Expect '(' after 'for'.")?;

        // For loop initializer
        let initializer = if self.match_tokens(&[TokenType::Semicolon]) {
            None
        } else if self.match_tokens(&[TokenType::Var]) {
            Some(self.var_declaration()?)
        } else {
            Some(self.expression_statement()?)
        };

        // For loop condition
        let condition = if !self.check(&TokenType::Semicolon) {
            self.expression()?
        } else {
            Expr::Literal { value: crate::lexer::LiteralValue::Boolean(true) }
        };
        self.consume(TokenType::Semicolon, "Expect ';' after for loop condition.")?;

        // For loop increment
        let increment = if !self.check(&TokenType::RightParen) {
            Some(self.expression()?)
        } else {
            None
        };
        self.consume(TokenType::RightParen, "Expect ')' after for clauses.")?;

        let mut body = self.statement()?;

        if let Some(inc) = increment {
            body = Stmt::Block {
                statements: vec![body, Stmt::Expression { expression: inc }]
            };
        }

        body = Stmt::While {
            condition,
            body: Box::new(body),
        };

        if let Some(init) = initializer {
            body = Stmt::Block {
                statements: vec![init, body]
            };
        }

        Ok(body)
    }

    fn if_statement(&mut self) -> Result<Stmt, RuntimeError> {
        self.consume(TokenType::LeftParen, "Expect '(' after 'if'.")?;
        let condition = self.expression()?;
        self.consume(TokenType::RightParen, "Expect ')' after if condition.")?;

        let then_branch = Box::new(self.statement()?);
        let else_branch = if self.match_tokens(&[TokenType::Else]) {
            Some(Box::new(self.statement()?))
        } else {
            None
        };

        Ok(Stmt::If { condition, then_branch, else_branch })
    }

    fn block(&mut self) -> Result<Vec<Stmt>, RuntimeError> {
        let mut statements = Vec::new();

        while !self.check(&TokenType::RightBrace) && !self.is_at_end() {
            if let Some(stmt) = self.declaration()? {
                statements.push(stmt);
            }
        }

        self.consume(TokenType::RightBrace, "Expect '}' after block.")?;
        Ok(statements)
    }

    fn expression(&mut self) -> Result<Expr, RuntimeError> {
        self.assignment()
    }

    fn assignment(&mut self) -> Result<Expr, RuntimeError> {
        let expr = self.logical_or()?;

        if self.match_tokens(&[TokenType::Assign]) {
            let _equals = self.previous().clone();
            let value = self.assignment()?;

            match expr {
                Expr::Variable { name } => {
                    return Ok(Expr::Assign { name, value: Box::new(value) });
                }
                Expr::Get { object, index } => {
                    return Ok(Expr::Set { object, index, value: Box::new(value) });
                }
                _ => {
                    return Err(self.error(self.previous(), "Invalid assignment target."));
                }
            }
        }

        Ok(expr)
    }

    fn logical_or(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.logical_and()?;

        while self.match_tokens(&[TokenType::Or]) {
            let operator = self.previous().clone();
            let right = self.logical_and()?;
            expr = Expr::Logical {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }

        Ok(expr)
    }

    fn logical_and(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.equality()?;

        while self.match_tokens(&[TokenType::And]) {
            let operator = self.previous().clone();
            let right = self.equality()?;
            expr = Expr::Logical {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }

        Ok(expr)
    }

    fn equality(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.comparison()?;

        while self.match_tokens(&[TokenType::NotEqual, TokenType::Equal]) {
            let operator = self.previous().clone();
            let right = self.comparison()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }

        Ok(expr)
    }

    fn comparison(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.term()?;

        while self.match_tokens(&[TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual]) {
            let operator = self.previous().clone();
            let right = self.term()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }

        Ok(expr)
    }

    fn term(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.factor()?;

        while self.match_tokens(&[TokenType::Plus, TokenType::Minus]) {
            let operator = self.previous().clone();
            let right = self.factor()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }

        Ok(expr)
    }

    fn factor(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.unary()?;

        while self.match_tokens(&[TokenType::Multiply, TokenType::Divide]) {
            let operator = self.previous().clone();
            let right = self.unary()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }

        Ok(expr)
    }

    fn unary(&mut self) -> Result<Expr, RuntimeError> {
        if self.match_tokens(&[TokenType::Not, TokenType::Minus]) {
            let operator = self.previous().clone();
            let right = self.unary()?;
            return Ok(Expr::Unary {
                operator,
                right: Box::new(right),
            });
        }

        self.call()
    }

    fn call(&mut self) -> Result<Expr, RuntimeError> {
        let mut expr = self.primary()?;

        loop {
            if self.match_tokens(&[TokenType::LeftParen]) {
                expr = self.finish_call(expr)?;
            } else if self.match_tokens(&[TokenType::LeftBracket]) {
                let index = self.expression()?;
                self.consume(TokenType::RightBracket, "Expect ']' after index.")?;
                expr = Expr::Get {
                    object: Box::new(expr),
                    index: Box::new(index),
                };
            } else {
                break;
            }
        }

        Ok(expr)
    }

    fn finish_call(&mut self, callee: Expr) -> Result<Expr, RuntimeError> {
        let mut arguments = Vec::new();

        if !self.check(&TokenType::RightParen) {
            loop {
                if arguments.len() >= 255 {
                    return Err(self.error(self.peek(), "Can't have more than 255 arguments."));
                }
                arguments.push(self.expression()?);
                
                if !self.match_tokens(&[TokenType::Comma]) {
                    break;
                }
            }
        }

        let paren = self.consume(TokenType::RightParen, "Expect ')' after arguments.")?.clone();
        Ok(Expr::Call {
            callee: Box::new(callee),
            paren,
            arguments,
        })
    }

    fn primary(&mut self) -> Result<Expr, RuntimeError> {
        if self.match_tokens(&[TokenType::Number, TokenType::False, TokenType::True, TokenType::Nil]) {
            return Ok(Expr::Literal {
                value: self.previous().literal.clone().unwrap(),
            });
        }

        if self.match_tokens(&[TokenType::String]) {
            return Ok(Expr::Literal {
                value: self.previous().literal.clone().unwrap(),
            });
        }

        if self.match_tokens(&[TokenType::Char]) {
            return Ok(Expr::Literal {
                value: self.previous().literal.clone().unwrap(),
            });
        }

        if self.match_tokens(&[TokenType::LeftParen]) {
            let expr = self.expression()?;
            self.consume(TokenType::RightParen, "Expect ')' after expression.")?;
            return Ok(Expr::Grouping {
                expression: Box::new(expr),
            });
        }

        if self.match_tokens(&[TokenType::Identifier]) {
            return Ok(Expr::Variable {
                name: self.previous().clone(),
            });
        }

        if self.match_tokens(&[TokenType::LeftBracket]) {
            let mut elements = Vec::new();
            
            if !self.check(&TokenType::RightBracket) {
                loop {
                    elements.push(self.expression()?);
                    if !self.match_tokens(&[TokenType::Comma]) {
                        break;
                    }
                }
            }
            
            self.consume(TokenType::RightBracket, "Expect ']' after list elements.")?;
            return Ok(Expr::ListLiteral { elements });
        }

        Err(self.error(self.peek(), "Expect expression."))
    }

    fn error(&self, token: &Token, message: &str) -> RuntimeError {
        if token.token_type == TokenType::Eof {
            RuntimeError::new(
                format!("Parse Error at end: {}", message),
                Some(token.line),
                &self.filename,
            )
        } else {
            RuntimeError::new(
                format!("Parse Error at '{}': {}", token.lexeme, message),
                Some(token.line),
                &self.filename,
            )
        }
    }
}
