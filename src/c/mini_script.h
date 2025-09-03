#ifndef MINI_SCRIPT_H
#define MINI_SCRIPT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Forward declarations */
typedef struct Value Value;
typedef struct Token Token;
typedef struct Stmt Stmt;
typedef struct Expr Expr;
typedef struct Environment Environment;
typedef struct Interpreter Interpreter;
typedef struct RuntimeError RuntimeError;

/* Token types */
typedef enum {
  // Single-character tokens
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  DIVIDE,
  MULTIPLY,

  // One or two character tokens
  NOT,
  NOT_EQUAL,
  ASSIGN,
  EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,
  AND,
  OR,

  // Literals
  IDENTIFIER,
  STRING,
  NUMBER,
  MS_CHAR,

  // Keywords
  PRINT,
  ELSE,
  FALSE,
  FOR,
  FUNCTION,
  IF,
  RETURN,
  TRUE,
  WHILE,
  IMPORT,
  FROM,
  INT_TYPE,
  FLOAT_TYPE,
  CHAR_TYPE,
  STRING_TYPE,
  LIST,
  MAP,
  LOADLIB,
  GETPROC,
  FREELIB,
  CALLEXT,
  ASSERT,
  VAR,
  NIL,

  EOF_TOKEN
} MSTokenType;

/* Literal value types */
typedef enum {
  LITERAL_STRING,
  LITERAL_NUMBER,
  LITERAL_INTEGER,
  LITERAL_BOOLEAN,
  LITERAL_MS_CHAR,
  LITERAL_NIL
} LiteralType;

typedef struct {
  LiteralType type;
  union {
    char *string;
    double number;
    long integer;
    bool boolean;
    char character;
  } value;
  bool owns_string; /* for LITERAL_STRING */
} LiteralValue;

/* Token structure */
struct Token {
  MSTokenType type;
  char *lexeme;
  LiteralValue *literal;
  size_t line;
};

/* Value types */
typedef enum {
  VALUE_NIL,
  VALUE_BOOLEAN,
  VALUE_NUMBER,
  VALUE_STRING,
  VALUE_LIST,
  VALUE_FUNCTION,
  VALUE_BUILTIN,
  VALUE_FILE_HANDLE
} ValueType;

typedef struct ValueList {
  Value *elements;
  size_t count;
  size_t capacity;
} ValueList;

typedef struct MiniScriptFunction {
  Stmt *declaration;
  Environment *closure;
} MiniScriptFunction;

struct Value {
  ValueType type;
  union {
    bool boolean;
    double number;
    char *string;
    ValueList *list;
    MiniScriptFunction *function;
    char *builtin_name;
    FILE *file_handle;
  } as;
};

/* Statement types */
typedef enum {
  STMT_BLOCK,
  STMT_EXPRESSION,
  STMT_PRINT,
  STMT_FUNCTION,
  STMT_FOR,
  STMT_IF,
  STMT_RETURN,
  STMT_WHILE,
  STMT_IMPORT,
  STMT_ASSERT,
  STMT_VAR
} StmtType;

typedef struct StmtList {
  Stmt **statements; /* array of owned Stmt* */
  size_t count;
  size_t capacity;
} StmtList;

struct Stmt {
  StmtType type;
  union {
    struct {
      StmtList statements;
    } block;
    struct {
      Expr *expression;
    } expression;
    struct {
      Expr **expressions;
      size_t count;
    } print;
    struct {
      Token name;
      Token *params;
      size_t param_count;
      StmtList body;
    } function;
    struct {
      Stmt *initializer;
      Expr *condition;
      Expr *increment;
      Stmt *body;
    } for_stmt;
    struct {
      Expr *condition;
      Stmt *then_branch;
      Stmt *else_branch;
    } if_stmt;
    struct {
      Token keyword;
      Expr *value;
    } return_stmt;
    struct {
      Expr *condition;
      Stmt *body;
    } while_stmt;
    struct {
      Token path_token;
      Token *namespace_token;
    } import;
    struct {
      Token keyword;
      Expr *condition;
      Expr *message;
    } assert_stmt;
    struct {
      Token name;
      Expr *initializer;
    } var;
  } as;
};

/* Expression types */
typedef enum {
  EXPR_ASSIGN,
  EXPR_BINARY,
  EXPR_CALL,
  EXPR_GROUPING,
  EXPR_LITERAL,
  EXPR_LIST_LITERAL,
  EXPR_GET,
  EXPR_SET,
  EXPR_LOGICAL,
  EXPR_UNARY,
  EXPR_VARIABLE
} ExprType;

typedef struct ExprList {
  Expr **expressions; /* array of owned Expr* */
  size_t count;
  size_t capacity;
} ExprList;

struct Expr {
  ExprType type;
  union {
    struct {
      Token name;
      Expr *value;
    } assign;
    struct {
      Expr *left;
      Token op;
      Expr *right;
    } binary;
    struct {
      Expr *callee;
      Token paren;
      ExprList arguments;
    } call;
    struct {
      Expr *expression;
    } grouping;
    struct {
      LiteralValue value;
    } literal;
    struct {
      ExprList elements;
    } list_literal;
    struct {
      Expr *object;
      Expr *index;
    } get;
    struct {
      Expr *object;
      Expr *index;
      Expr *value;
    } set;
    struct {
      Expr *left;
      Token op;
      Expr *right;
    } logical;
    struct {
      Token op;
      Expr *right;
    } unary;
    struct {
      Token name;
    } variable;
  } as;
};

/* Environment for variable storage */
struct Environment {
  struct {
    char **keys;
    Value **values; /* Owned Value* objects */
    size_t count;
    size_t capacity;
  } values;
  Environment *enclosing;
};

/* Runtime error */
struct RuntimeError {
  char *message;
  size_t line;
  char *filename;
  Value *return_value;
};

/* Interpreter */
struct Interpreter {
  Environment *globals;
  Environment *environment;
  char **modules_path;
  size_t modules_path_count;
  Value *return_value;  // For function return values
  char *current_filename; // Current source filename for error reporting
};

/* Function prototypes */

/* Memory management */
Value *value_new(ValueType type);
void value_free(Value *value);
Value *value_copy(Value *value);

/* Token functions */
Token *token_new(MSTokenType type, const char *lexeme, LiteralValue *literal,
                 size_t line);
void token_free(Token *token);

/* Literal value functions */
LiteralValue *literal_new(LiteralType type);
void literal_free(LiteralValue *literal);

/* Statement functions */
Stmt *stmt_new(StmtType type);
void stmt_free(Stmt *stmt);

/* Expression functions */
Expr *expr_new(ExprType type);
void expr_free(Expr *expr);

/* Environment functions */
Environment *environment_new(Environment *enclosing);
void environment_free(Environment *environment);
void environment_define(Environment *env, const char *name, Value *value);
Value *environment_get(Environment *env, Token *name, RuntimeError **error, const char *filename);
void environment_assign(Environment *env, Token *name, Value *value,
                        RuntimeError **error, const char *filename);

/* Interpreter functions */
Interpreter *interpreter_new(void);
void interpreter_free(Interpreter *interpreter);
void interpreter_set_filename(Interpreter *interpreter, const char *filename);
void interpreter_interpret(Interpreter *interpreter, StmtList statements,
                           RuntimeError **error);
Value *interpreter_evaluate(Interpreter *interpreter, Expr *expr,
                            RuntimeError **error);
void interpreter_execute(Interpreter *interpreter, Stmt *stmt,
                         RuntimeError **error);

/* Lexer functions */
typedef struct {
  const char *source;
  size_t start;
  size_t current;
  size_t line;
  Token *tokens;
  size_t token_count;
  size_t token_capacity;
} Lexer;

Lexer *lexer_new(const char *source);
void lexer_free(Lexer *lexer);
void lexer_scan_tokens(Lexer *lexer);

/* Parser functions */
typedef struct {
  Token *tokens;
  size_t current;
  size_t count;
  char *filename; // Source filename for error reporting
} Parser;

Parser *parser_new(Token *tokens, size_t count, const char *filename);
void parser_free(Parser *parser);
StmtList parser_parse(Parser *parser, RuntimeError **error);

/* Runtime error functions */
RuntimeError *runtime_error_new(const char *message, size_t line,
                                const char *filename);
RuntimeError *runtime_error_with_return(const char *message, size_t line,
                                        const char *filename,
                                        Value *return_value);
void runtime_error_free(RuntimeError *error);

/* Builtin functions */
void interpreter_define_builtins(Interpreter *interpreter);

/* Utility functions */
char *stringify_value(Value *value);
bool is_truthy(Value *value);
bool values_equal(Value *a, Value *b);

/* File operations */
char *read_file(const char *filename);
void run_file(const char *filename);
void run_prompt(void);

#endif /* MINI_SCRIPT_H */
