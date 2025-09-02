use crate::lexer::Token;

// Statements
#[derive(Debug, Clone)]
pub enum Stmt {
    Block { statements: Vec<Stmt> },
    Expression { expression: Expr },
    Print { expressions: Vec<Expr> },
    Function { name: Token, params: Vec<Token>, body: Vec<Stmt> },
    If { condition: Expr, then_branch: Box<Stmt>, else_branch: Option<Box<Stmt>> },
    Return { keyword: Token, value: Option<Expr> },
    While { condition: Expr, body: Box<Stmt> },
    Import { path_token: Token, namespace: Option<Token> },
    Assert { keyword: Token, condition: Expr, message: Expr },
    Var { name: Token, initializer: Option<Expr> },
}

// Expressions
#[derive(Debug, Clone)]
pub enum Expr {
    Assign { name: Token, value: Box<Expr> },
    Binary { left: Box<Expr>, operator: Token, right: Box<Expr> },
    Call { callee: Box<Expr>, paren: Token, arguments: Vec<Expr> },
    Grouping { expression: Box<Expr> },
    Literal { value: crate::lexer::LiteralValue },
    ListLiteral { elements: Vec<Expr> },
    Get { object: Box<Expr>, index: Box<Expr> },
    Set { object: Box<Expr>, index: Box<Expr>, value: Box<Expr> },
    Logical { left: Box<Expr>, operator: Token, right: Box<Expr> },
    Unary { operator: Token, right: Box<Expr> },
    Variable { name: Token },
}
