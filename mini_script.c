#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

// =============================================================================
// 1. TOKENIZER (LEXER)
// =============================================================================

typedef enum {
  // Single-character tokens
  MS_TOKEN_LPAREN,
  MS_TOKEN_RPAREN,
  MS_TOKEN_LBRACE,
  MS_TOKEN_RBRACE,
  MS_TOKEN_LBRACKET,
  MS_TOKEN_RBRACKET,
  MS_TOKEN_COMMA,
  MS_TOKEN_DOT,
  MS_TOKEN_MINUS,
  MS_TOKEN_PLUS,
  MS_TOKEN_SEMICOLON,
  MS_TOKEN_DIVIDE,
  MS_TOKEN_MULTIPLY,

  // One or two character tokens
  MS_TOKEN_NOT,
  MS_TOKEN_NOT_EQUAL,
  MS_TOKEN_ASSIGN,
  MS_TOKEN_EQUAL,
  MS_TOKEN_GREATER,
  MS_TOKEN_GREATER_EQUAL,
  MS_TOKEN_LESS,
  MS_TOKEN_LESS_EQUAL,
  MS_TOKEN_AND,
  MS_TOKEN_OR,

  // Literals
  MS_TOKEN_IDENTIFIER,
  MS_TOKEN_STRING,
  MS_TOKEN_NUMBER,
  MS_TOKEN_CHAR,

  // Keywords
  MS_TOKEN_PRINT,
  MS_TOKEN_ELSE,
  MS_TOKEN_FALSE,
  MS_TOKEN_FOR,
  MS_TOKEN_FUNCTION,
  MS_TOKEN_IF,
  MS_TOKEN_RETURN,
  MS_TOKEN_TRUE,
  MS_TOKEN_WHILE,
  MS_TOKEN_IMPORT,
  MS_TOKEN_FROM,
  MS_TOKEN_INT_TYPE,
  MS_TOKEN_FLOAT_TYPE,
  MS_TOKEN_CHAR_TYPE,
  MS_TOKEN_STRING_TYPE,
  MS_TOKEN_LIST,
  MS_TOKEN_MAP,
  MS_TOKEN_LOADLIB,
  MS_TOKEN_GETPROC,
  MS_TOKEN_FREELIB,
  MS_TOKEN_CALLEXT,
  MS_TOKEN_ASSERT,
  MS_TOKEN_VAR,
  MS_TOKEN_NIL,

  MS_TOKEN_EOF
} MSTokenType;

typedef struct {
  MSTokenType type;
  char *lexeme;
  union {
    double number;
    char *string;
    char character;
    bool boolean;
  } literal;
  int line;
} Token;

typedef struct {
  char *source;
  char *filename;
  Token *tokens;
  int token_count;
  int token_capacity;
  int start;
  int current;
  int line;
} Lexer;

// Token keyword mapping
typedef struct {
  char *text;
  MSTokenType type;
} Keyword;

static Keyword keywords[] = {{"print", MS_TOKEN_PRINT},
                             {"if", MS_TOKEN_IF},
                             {"else", MS_TOKEN_ELSE},
                             {"while", MS_TOKEN_WHILE},
                             {"for", MS_TOKEN_FOR},
                             {"function", MS_TOKEN_FUNCTION},
                             {"return", MS_TOKEN_RETURN},
                             {"true", MS_TOKEN_TRUE},
                             {"false", MS_TOKEN_FALSE},
                             {"import", MS_TOKEN_IMPORT},
                             {"from", MS_TOKEN_FROM},
                             {"int", MS_TOKEN_INT_TYPE},
                             {"float", MS_TOKEN_FLOAT_TYPE},
                             {"char", MS_TOKEN_CHAR_TYPE},
                             {"string", MS_TOKEN_STRING_TYPE},
                             {"list", MS_TOKEN_LIST},
                             {"map", MS_TOKEN_MAP},
                             {"loadlib", MS_TOKEN_LOADLIB},
                             {"getproc", MS_TOKEN_GETPROC},
                             {"freelib", MS_TOKEN_FREELIB},
                             {"callext", MS_TOKEN_CALLEXT},
                             {"assert", MS_TOKEN_ASSERT},
                             {"var", MS_TOKEN_VAR},
                             {"nil", MS_TOKEN_NIL},
                             {NULL, MS_TOKEN_EOF}};

// =============================================================================
// 2. ABSTRACT SYNTAX TREE (AST) NODES
// =============================================================================

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

typedef enum {
  STMT_BLOCK,
  STMT_EXPRESSION,
  STMT_PRINT,
  STMT_FUNCTION,
  STMT_IF,
  STMT_RETURN,
  STMT_WHILE,
  STMT_FOR,
  STMT_IMPORT,
  STMT_ASSERT,
  STMT_VAR
} StmtType;

typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Interpreter Interpreter;

typedef struct {
  union {
    double number;
    char *string;
    char character;
    bool boolean;
    struct Expr **elements;     // For lists
    FILE *file_handle;          // For file handles
    struct Stmt *function_stmt; // For user-defined functions
  } value;
  enum {
    VALUE_NIL,
    VALUE_BOOL,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_CHAR,
    VALUE_LIST,
    VALUE_FILE,
    VALUE_FUNCTION
  } type;
  int list_size; // For lists
} Value;

struct Expr {
  ExprType type;
  union {
    struct {
      Token *name;
      Expr *value;
    } assign;
    struct {
      Expr *left;
      Token *operator;
      Expr *right;
    } binary;
    struct {
      Expr *callee;
      Token *paren;
      Expr **arguments;
      int arg_count;
    } call;
    struct {
      Expr *expression;
    } grouping;
    struct {
      Value value;
    } literal;
    struct {
      Expr **elements;
      int element_count;
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
      Token *operator;
      Expr *right;
    } logical;
    struct {
      Token *operator;
      Expr *right;
    } unary;
    struct {
      Token *name;
    } variable;
  };
};

struct Stmt {
  StmtType type;
  union {
    struct {
      Stmt **statements;
      int stmt_count;
    } block;
    struct {
      Expr *expression;
    } expression;
    struct {
      Expr **expressions;
      int expr_count;
    } print;
    struct {
      Token *name;
      Token **params;
      int param_count;
      Stmt **body;
      int body_count;
    } function;
    struct {
      Expr *condition;
      Stmt *then_branch;
      Stmt *else_branch;
    } if_stmt;
    struct {
      Token *keyword;
      Expr *value;
    } return_stmt;
    struct {
      Expr *condition;
      Stmt *body;
    } while_stmt;
    struct {
      Stmt *initializer;
      Expr *condition;
      Expr *increment;
      Stmt *body;
    } for_stmt;
    struct {
      Token *path_token;
      Token *namespace;
    } import;
    struct {
      Token *keyword;
      Expr *condition;
      Expr *message;
    } assert_stmt;
    struct {
      Token *name;
      Expr *initializer;
    } var;
  };
};

// =============================================================================
// 3. PARSER
// =============================================================================

typedef struct {
  Token *tokens;
  int token_count;
  char *filename;
  int current;
} Parser;

// =============================================================================
// 4. BUILT-IN FUNCTIONS & INTERPRETER
// =============================================================================

typedef struct Environment Environment;

struct Environment {
  char **names;
  Value *values;
  int count;
  int capacity;
  Environment *enclosing;
};

typedef struct {
  int arity;
  Value (*call)(Interpreter *interpreter, Value *arguments);
  char *name;
} BuiltinFunction;

typedef struct {
  Token *name;
  Token **params;
  int param_count;
  Stmt **body;
  int body_count;
  Environment *closure;
} UserFunction;

typedef struct Interpreter {
  Environment *globals;
  Environment *environment;
  char *filename;
  BuiltinFunction *builtins;
  int builtin_count;
  Value return_value;
  bool has_returned;
} Interpreter;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

void error_exit(const char *message, ...) {
  va_list args;
  va_start(args, message);
  vprintf(message, args);
  va_end(args);
  exit(1);
}

char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    printf("Error: File not found at %s\n", path);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char *buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    fclose(file);
    error_exit("Not enough memory to read file.\n");
  }

  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size) {
    free(buffer);
    fclose(file);
    error_exit("Could not read file.\n");
  }

  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}

// =============================================================================
// LEXER IMPLEMENTATION
// =============================================================================

Lexer *create_lexer(char *source, char *filename) {
  Lexer *lexer = malloc(sizeof(Lexer));
  lexer->source = source;
  lexer->filename = filename;
  lexer->tokens = malloc(sizeof(Token) * 100);
  lexer->token_count = 0;
  lexer->token_capacity = 100;
  lexer->start = 0;
  lexer->current = 0;
  lexer->line = 1;
  return lexer;
}

bool is_at_end(Lexer *lexer) { return lexer->current >= strlen(lexer->source); }

char advance(Lexer *lexer) { return lexer->source[lexer->current++]; }

char peek(Lexer *lexer) {
  if (is_at_end(lexer))
    return '\0';
  return lexer->source[lexer->current];
}

char peek_next(Lexer *lexer) {
  if (lexer->current + 1 >= strlen(lexer->source))
    return '\0';
  return lexer->source[lexer->current + 1];
}

bool match(Lexer *lexer, char expected) {
  if (is_at_end(lexer))
    return false;
  if (lexer->source[lexer->current] != expected)
    return false;
  lexer->current++;
  return true;
}

void add_token_simple(Lexer *lexer, MSTokenType type) {
  if (lexer->token_count >= lexer->token_capacity) {
    lexer->token_capacity *= 2;
    lexer->tokens =
        realloc(lexer->tokens, sizeof(Token) * lexer->token_capacity);
  }

  Token *token = &lexer->tokens[lexer->token_count++];
  token->type = type;
  int length = lexer->current - lexer->start;
  token->lexeme = malloc(length + 1);
  strncpy(token->lexeme, lexer->source + lexer->start, length);
  token->lexeme[length] = '\0';
  token->line = lexer->line;
}

void add_token_string(Lexer *lexer, MSTokenType type, char *literal) {
  add_token_simple(lexer, type);
  Token *token = &lexer->tokens[lexer->token_count - 1];
  token->literal.string = malloc(strlen(literal) + 1);
  strcpy(token->literal.string, literal);
}

void add_token_number(Lexer *lexer, MSTokenType type, double number) {
  add_token_simple(lexer, type);
  Token *token = &lexer->tokens[lexer->token_count - 1];
  token->literal.number = number;
}

void add_token_char(Lexer *lexer, MSTokenType type, char character) {
  add_token_simple(lexer, type);
  Token *token = &lexer->tokens[lexer->token_count - 1];
  token->literal.character = character;
}

void add_token_bool(Lexer *lexer, MSTokenType type, bool value) {
  add_token_simple(lexer, type);
  Token *token = &lexer->tokens[lexer->token_count - 1];
  token->literal.boolean = value;
}

void scan_string(Lexer *lexer) {
  while (peek(lexer) != '"' && !is_at_end(lexer)) {
    if (peek(lexer) == '\n')
      lexer->line++;
    advance(lexer);
  }

  if (is_at_end(lexer)) {
    printf("Lexer Error in %s at line %d: Unterminated string.\n",
           lexer->filename, lexer->line);
    return;
  }

  advance(lexer); // The closing "

  // Extract string value without quotes
  int length = lexer->current - lexer->start - 2;
  char *value = malloc(length + 1);
  strncpy(value, lexer->source + lexer->start + 1, length);
  value[length] = '\0';

  add_token_string(lexer, MS_TOKEN_STRING, value);
  free(value);
}

void scan_number(Lexer *lexer) {
  while (isdigit(peek(lexer)))
    advance(lexer);

  if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
    advance(lexer); // Consume the .
    while (isdigit(peek(lexer)))
      advance(lexer);
  }

  int length = lexer->current - lexer->start;
  char *text = malloc(length + 1);
  strncpy(text, lexer->source + lexer->start, length);
  text[length] = '\0';

  double value = atof(text);
  add_token_number(lexer, MS_TOKEN_NUMBER, value);
  free(text);
}

MSTokenType get_keyword_type(char *text) {
  for (int i = 0; keywords[i].text != NULL; i++) {
    if (strcmp(text, keywords[i].text) == 0) {
      return keywords[i].type;
    }
  }
  return MS_TOKEN_IDENTIFIER;
}

void scan_identifier(Lexer *lexer) {
  while (isalnum(peek(lexer)) || peek(lexer) == '_')
    advance(lexer);

  int length = lexer->current - lexer->start;
  char *text = malloc(length + 1);
  strncpy(text, lexer->source + lexer->start, length);
  text[length] = '\0';

  MSTokenType type = get_keyword_type(text);

  if (type == MS_TOKEN_TRUE) {
    add_token_bool(lexer, type, true);
  } else if (type == MS_TOKEN_FALSE) {
    add_token_bool(lexer, type, false);
  } else {
    add_token_simple(lexer, type);
  }

  free(text);
}

void scan_token(Lexer *lexer) {
  char c = advance(lexer);

  switch (c) {
  case ' ':
  case '\r':
  case '\t':
    break;
  case '\n':
    lexer->line++;
    break;
  case '(':
    add_token_simple(lexer, MS_TOKEN_LPAREN);
    break;
  case ')':
    add_token_simple(lexer, MS_TOKEN_RPAREN);
    break;
  case '{':
    add_token_simple(lexer, MS_TOKEN_LBRACE);
    break;
  case '}':
    add_token_simple(lexer, MS_TOKEN_RBRACE);
    break;
  case '[':
    add_token_simple(lexer, MS_TOKEN_LBRACKET);
    break;
  case ']':
    add_token_simple(lexer, MS_TOKEN_RBRACKET);
    break;
  case ',':
    add_token_simple(lexer, MS_TOKEN_COMMA);
    break;
  case '.':
    add_token_simple(lexer, MS_TOKEN_DOT);
    break;
  case '-':
    add_token_simple(lexer, MS_TOKEN_MINUS);
    break;
  case '+':
    add_token_simple(lexer, MS_TOKEN_PLUS);
    break;
  case ';':
    add_token_simple(lexer, MS_TOKEN_SEMICOLON);
    break;
  case '*':
    add_token_simple(lexer, MS_TOKEN_MULTIPLY);
    break;
  case '!':
    add_token_simple(lexer,
                     match(lexer, '=') ? MS_TOKEN_NOT_EQUAL : MS_TOKEN_NOT);
    break;
  case '=':
    add_token_simple(lexer,
                     match(lexer, '=') ? MS_TOKEN_EQUAL : MS_TOKEN_ASSIGN);
    break;
  case '<':
    add_token_simple(lexer,
                     match(lexer, '=') ? MS_TOKEN_LESS_EQUAL : MS_TOKEN_LESS);
    break;
  case '>':
    add_token_simple(lexer, match(lexer, '=') ? MS_TOKEN_GREATER_EQUAL
                                              : MS_TOKEN_GREATER);
    break;
  case '|':
    if (match(lexer, '|')) {
      add_token_simple(lexer, MS_TOKEN_OR);
    } else {
      printf("Lexer Error in %s at line %d: Unexpected character: |\n",
             lexer->filename, lexer->line);
    }
    break;
  case '/':
    if (match(lexer, '/')) {
      // Comment - consume until end of line
      while (peek(lexer) != '\n' && !is_at_end(lexer))
        advance(lexer);
    } else {
      add_token_simple(lexer, MS_TOKEN_DIVIDE);
    }
    break;
  case '&':
    if (match(lexer, '&')) {
      add_token_simple(lexer, MS_TOKEN_AND);
    } else {
      printf("Lexer Error in %s at line %d: Unexpected character: &\n",
             lexer->filename, lexer->line);
    }
    break;
  case '"':
    scan_string(lexer);
    break;
  case '\'': {
    char char_val = advance(lexer);
    if (advance(lexer) != '\'') {
      printf("Lexer Error in %s at line %d: Unterminated character literal.\n",
             lexer->filename, lexer->line);
    } else {
      add_token_char(lexer, MS_TOKEN_CHAR, char_val);
    }
  } break;
  default:
    if (isdigit(c)) {
      scan_number(lexer);
    } else if (isalpha(c) || c == '_') {
      scan_identifier(lexer);
    } else {
      printf("Lexer Error in %s at line %d: Unexpected character: %c\n",
             lexer->filename, lexer->line, c);
    }
    break;
  }
}

Token *scan_tokens(Lexer *lexer) {
  while (!is_at_end(lexer)) {
    lexer->start = lexer->current;
    scan_token(lexer);
  }

  add_token_simple(lexer, MS_TOKEN_EOF);
  return lexer->tokens;
}

// =============================================================================
// PARSER IMPLEMENTATION
// =============================================================================

Parser *create_parser(Token *tokens, int token_count, char *filename) {
  Parser *parser = malloc(sizeof(Parser));
  parser->tokens = tokens;
  parser->token_count = token_count;
  parser->filename = filename;
  parser->current = 0;
  return parser;
}

bool parser_is_at_end(Parser *parser) {
  return parser->tokens[parser->current].type == MS_TOKEN_EOF;
}

Token *parser_peek(Parser *parser) { return &parser->tokens[parser->current]; }

Token *parser_previous(Parser *parser) {
  return &parser->tokens[parser->current - 1];
}

Token *parser_advance(Parser *parser) {
  if (!parser_is_at_end(parser))
    parser->current++;
  return parser_previous(parser);
}

bool parser_check(Parser *parser, MSTokenType type) {
  if (parser_is_at_end(parser))
    return false;
  return parser_peek(parser)->type == type;
}

bool parser_match(Parser *parser, int count, ...) {
  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i++) {
    MSTokenType type = va_arg(args, MSTokenType);
    if (parser_check(parser, type)) {
      parser_advance(parser);
      va_end(args);
      return true;
    }
  }

  va_end(args);
  return false;
}

// Forward declarations for recursive descent parser
Expr *expression(Parser *parser);
Stmt *statement(Parser *parser);
Value evaluate_expr(struct Interpreter *interpreter, Expr *expr);
Expr *finish_call(Parser *parser, Expr *callee);
void execute_stmt(struct Interpreter *interpreter, Stmt *stmt);
Stmt *import_statement(Parser *parser);

Expr *create_literal_expr(Value value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = EXPR_LITERAL;
  expr->literal.value = value;
  return expr;
}

Expr *create_variable_expr(Token *name) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = EXPR_VARIABLE;
  expr->variable.name = name;
  return expr;
}

Expr *create_assign_expr(Token *name, Expr *value) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = EXPR_ASSIGN;
  expr->assign.name = name;
  expr->assign.value = value;
  return expr;
}

Expr *create_binary_expr(Expr *left, Token *operator, Expr *right) {
  Expr *expr = malloc(sizeof(Expr));
  expr->type = EXPR_BINARY;
  expr->binary.left = left;
  expr->binary.operator = operator;
  expr->binary.right = right;
  return expr;
}

Expr *primary(Parser *parser) {
  if (parser_match(parser, 4, MS_TOKEN_NUMBER, MS_TOKEN_FALSE, MS_TOKEN_TRUE,
                   MS_TOKEN_NIL)) {
    Token *token = parser_previous(parser);
    Value value;

    switch (token->type) {
    case MS_TOKEN_NUMBER:
      value.type = VALUE_NUMBER;
      value.value.number = token->literal.number;
      break;
    case MS_TOKEN_TRUE:
      value.type = VALUE_BOOL;
      value.value.boolean = true;
      break;
    case MS_TOKEN_FALSE:
      value.type = VALUE_BOOL;
      value.value.boolean = false;
      break;
    case MS_TOKEN_NIL:
      value.type = VALUE_NIL;
      break;
    default:
      break;
    }

    return create_literal_expr(value);
  }

  if (parser_match(parser, 1, MS_TOKEN_STRING)) {
    Token *token = parser_previous(parser);
    Value value;
    value.type = VALUE_STRING;
    value.value.string = malloc(strlen(token->literal.string) + 1);
    strcpy(value.value.string, token->literal.string);
    return create_literal_expr(value);
  }

  if (parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
    return create_variable_expr(parser_previous(parser));
  }

  if (parser_match(parser, 1, MS_TOKEN_LPAREN)) {
    Expr *expr = expression(parser);
    if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
      printf("Parse Error: Expect ')' after expression.\n");
      exit(1);
    }

    Expr *grouping = malloc(sizeof(Expr));
    grouping->type = EXPR_GROUPING;
    grouping->grouping.expression = expr;
    return grouping;
  }

  if (parser_match(parser, 1, MS_TOKEN_LBRACKET)) {
    // Parse list literal
    Expr **elements = malloc(sizeof(Expr *) * 256); // Max 256 elements for now
    int element_count = 0;

    if (!parser_check(parser, MS_TOKEN_RBRACKET)) {
      do {
        elements[element_count++] = expression(parser);
      } while (parser_match(parser, 1, MS_TOKEN_COMMA));
    }

    if (!parser_match(parser, 1, MS_TOKEN_RBRACKET)) {
      printf("Parse Error: Expect ']' after list elements.\n");
      exit(1);
    }

    Expr *list_literal = malloc(sizeof(Expr));
    list_literal->type = EXPR_LIST_LITERAL;
    list_literal->list_literal.elements = elements;
    list_literal->list_literal.element_count = element_count;
    return list_literal;
  }

  printf("Parse Error: Expect expression.\n");
  exit(1);
}

Expr *call(Parser *parser) {
  Expr *expr = primary(parser);

  while (true) {
    if (parser_match(parser, 1, MS_TOKEN_LPAREN)) {
      expr = finish_call(parser, expr);
    } else if (parser_match(parser, 1, MS_TOKEN_LBRACKET)) {
      Expr *index = expression(parser);
      if (!parser_match(parser, 1, MS_TOKEN_RBRACKET)) {
        printf("Parse Error: Expect ']' after index.\n");
        exit(1);
      }

      Expr *get_expr = malloc(sizeof(Expr));
      get_expr->type = EXPR_GET;
      get_expr->get.object = expr;
      get_expr->get.index = index;
      expr = get_expr;
    } else {
      break;
    }
  }

  return expr;
}

Expr *finish_call(Parser *parser, Expr *callee) {
  Expr **arguments = malloc(sizeof(Expr *) * 10);
  int arg_count = 0;

  if (!parser_check(parser, MS_TOKEN_RPAREN)) {
    do {
      arguments[arg_count++] = expression(parser);
    } while (parser_match(parser, 1, MS_TOKEN_COMMA));
  }

  if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
    printf("Parse Error: Expect ')' after arguments.\n");
    exit(1);
  }

  Expr *call_expr = malloc(sizeof(Expr));
  call_expr->type = EXPR_CALL;
  call_expr->call.callee = callee;
  call_expr->call.arguments = arguments;
  call_expr->call.arg_count = arg_count;
  return call_expr;
}

Expr *unary(Parser *parser) {
  if (parser_match(parser, 2, MS_TOKEN_NOT, MS_TOKEN_MINUS)) {
    Token *operator = parser_previous(parser);
    Expr *right = unary(parser);

    Expr *expr = malloc(sizeof(Expr));
    expr->type = EXPR_UNARY;
    expr->unary.operator = operator;
    expr->unary.right = right;
    return expr;
  }

  return call(parser);
}

Expr *factor(Parser *parser) {
  Expr *expr = unary(parser);

  while (parser_match(parser, 2, MS_TOKEN_DIVIDE, MS_TOKEN_MULTIPLY)) {
    Token *operator = parser_previous(parser);
    Expr *right = unary(parser);
    expr = create_binary_expr(expr, operator, right);
  }

  return expr;
}

Expr *term(Parser *parser) {
  Expr *expr = factor(parser);

  while (parser_match(parser, 2, MS_TOKEN_MINUS, MS_TOKEN_PLUS)) {
    Token *operator = parser_previous(parser);
    Expr *right = factor(parser);
    expr = create_binary_expr(expr, operator, right);
  }

  return expr;
}

Expr *comparison(Parser *parser) {
  Expr *expr = term(parser);

  while (parser_match(parser, 4, MS_TOKEN_GREATER, MS_TOKEN_GREATER_EQUAL,
                      MS_TOKEN_LESS, MS_TOKEN_LESS_EQUAL)) {
    Token *operator = parser_previous(parser);
    Expr *right = term(parser);
    expr = create_binary_expr(expr, operator, right);
  }

  return expr;
}

Expr *equality(Parser *parser) {
  Expr *expr = comparison(parser);

  while (parser_match(parser, 2, MS_TOKEN_NOT_EQUAL, MS_TOKEN_EQUAL)) {
    Token *operator = parser_previous(parser);
    Expr *right = comparison(parser);
    expr = create_binary_expr(expr, operator, right);
  }

  return expr;
}

Expr *logical_and(Parser *parser) {
  Expr *expr = equality(parser);

  while (parser_match(parser, 1, MS_TOKEN_AND)) {
    Token *operator = parser_previous(parser);
    Expr *right = equality(parser);

    Expr *logical = malloc(sizeof(Expr));
    logical->type = EXPR_LOGICAL;
    logical->logical.left = expr;
    logical->logical.operator = operator;
    logical->logical.right = right;
    expr = logical;
  }

  return expr;
}

Expr *logical_or(Parser *parser) {
  Expr *expr = logical_and(parser);

  while (parser_match(parser, 1, MS_TOKEN_OR)) {
    Token *operator = parser_previous(parser);
    Expr *right = logical_and(parser);

    Expr *logical = malloc(sizeof(Expr));
    logical->type = EXPR_LOGICAL;
    logical->logical.left = expr;
    logical->logical.operator = operator;
    logical->logical.right = right;
    expr = logical;
  }

  return expr;
}

Expr *assignment(Parser *parser) {
  Expr *expr = logical_or(parser);

  if (parser_match(parser, 1, MS_TOKEN_ASSIGN)) {
    Expr *value = assignment(parser);

    if (expr->type == EXPR_VARIABLE) {
      Token *name = expr->variable.name;
      return create_assign_expr(name, value);
    }

    printf("Parse Error: Invalid assignment target.\n");
    exit(1);
  }

  return expr;
}

Expr *expression(Parser *parser) { return assignment(parser); }

Stmt *expression_statement(Parser *parser) {
  Expr *expr = expression(parser);
  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    printf("Parse Error: Expect ';' after expression.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_EXPRESSION;
  stmt->expression.expression = expr;
  return stmt;
}

Stmt *print_statement(Parser *parser) {
  Expr **expressions = malloc(sizeof(Expr *) * 10);
  int expr_count = 0;

  expressions[expr_count++] = expression(parser);

  while (parser_match(parser, 1, MS_TOKEN_COMMA)) {
    expressions[expr_count++] = expression(parser);
  }

  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    printf("Parse Error: Expect ';' after value.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_PRINT;
  stmt->print.expressions = expressions;
  stmt->print.expr_count = expr_count;
  return stmt;
}

Stmt *var_statement(Parser *parser) {
  Token *name = NULL;
  if (!parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
    printf("Parse Error: Expect variable name.\n");
    exit(1);
  }
  name = parser_previous(parser);

  Expr *initializer = NULL;
  if (parser_match(parser, 1, MS_TOKEN_ASSIGN)) {
    initializer = expression(parser);
  }

  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    printf("Parse Error: Expect ';' after variable declaration.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_VAR;
  stmt->var.name = name;
  stmt->var.initializer = initializer;
  return stmt;
}

Stmt *assert_statement(Parser *parser) {
  Expr *condition = expression(parser);

  if (!parser_match(parser, 1, MS_TOKEN_COMMA)) {
    printf("Parse Error: Expect ',' after assert condition.\n");
    exit(1);
  }

  Expr *message = expression(parser);

  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    printf("Parse Error: Expect ';' after assert message.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_ASSERT;
  stmt->assert_stmt.condition = condition;
  stmt->assert_stmt.message = message;
  return stmt;
}

Stmt *if_statement(Parser *parser) {
  if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
    printf("Parse Error: Expect '(' after 'if'.\n");
    exit(1);
  }

  Expr *condition = expression(parser);

  if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
    printf("Parse Error: Expect ')' after if condition.\n");
    exit(1);
  }

  Stmt *then_branch = statement(parser);
  Stmt *else_branch = NULL;

  if (parser_match(parser, 1, MS_TOKEN_ELSE)) {
    else_branch = statement(parser);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_IF;
  stmt->if_stmt.condition = condition;
  stmt->if_stmt.then_branch = then_branch;
  stmt->if_stmt.else_branch = else_branch;
  return stmt;
}

Stmt *while_statement(Parser *parser) {
  if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
    printf("Parse Error: Expect '(' after 'while'.\n");
    exit(1);
  }
  Expr *condition = expression(parser);
  if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
    printf("Parse Error: Expect ')' after while condition.\n");
    exit(1);
  }
  Stmt *body = statement(parser);

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_WHILE;
  stmt->while_stmt.condition = condition;
  stmt->while_stmt.body = body;
  return stmt;
}

Stmt *for_statement(Parser *parser) {
  if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
    printf("Parse Error: Expect '(' after 'for'.\n");
    exit(1);
  }

  // Initializer (can be var declaration or expression statement)
  Stmt *initializer = NULL;
  if (parser_match(parser, 1, MS_TOKEN_VAR)) {
    initializer = var_statement(parser);
  } else {
    initializer = expression_statement(parser);
  }

  // Condition
  Expr *condition = expression(parser);
  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    printf("Parse Error: Expect ';' after for loop condition.\n");
    exit(1);
  }

  // Increment
  Expr *increment = expression(parser);
  if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
    printf("Parse Error: Expect ')' after for clauses.\n");
    exit(1);
  }

  // Body
  Stmt *body = statement(parser);

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_FOR;
  stmt->for_stmt.initializer = initializer;
  stmt->for_stmt.condition = condition;
  stmt->for_stmt.increment = increment;
  stmt->for_stmt.body = body;
  return stmt;
}

Stmt *function_statement(Parser *parser) {
  if (!parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
    printf("Parse Error: Expect function name.\n");
    exit(1);
  }
  Token *name = parser_previous(parser);

  if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
    printf("Parse Error: Expect '(' after function name.\n");
    exit(1);
  }

  // Parse parameters
  Token **params = malloc(sizeof(Token *) * 10); // Max 10 params for now
  int param_count = 0;

  if (!parser_check(parser, MS_TOKEN_RPAREN)) {
    do {
      if (!parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
        printf("Parse Error: Expect parameter name.\n");
        exit(1);
      }
      params[param_count++] = parser_previous(parser);
    } while (parser_match(parser, 1, MS_TOKEN_COMMA));
  }

  if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
    printf("Parse Error: Expect ')' after parameters.\n");
    exit(1);
  }

  if (!parser_match(parser, 1, MS_TOKEN_LBRACE)) {
    printf("Parse Error: Expect '{' before function body.\n");
    exit(1);
  }

  // Parse function body
  Stmt **body = malloc(sizeof(Stmt *) * 50); // Max 50 statements for now
  int body_count = 0;

  while (!parser_check(parser, MS_TOKEN_RBRACE) && !parser_is_at_end(parser)) {
    body[body_count++] = statement(parser);
  }

  if (!parser_match(parser, 1, MS_TOKEN_RBRACE)) {
    printf("Parse Error: Expect '}' after function body.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_FUNCTION;
  stmt->function.name = name;
  stmt->function.params = params;
  stmt->function.param_count = param_count;
  stmt->function.body = body;
  stmt->function.body_count = body_count;
  return stmt;
}

Stmt *import_statement(Parser *parser) {
  if (!parser_match(parser, 1, MS_TOKEN_STRING)) {
    printf("Parse Error: Expect string after 'import'.\n");
    exit(1);
  }
  Token *path = parser_previous(parser);

  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    printf("Parse Error: Expect ';' after import statement.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_IMPORT;
  stmt->import.path_token = path;
  stmt->import.namespace = NULL;
  return stmt;
}

Stmt *return_statement(Parser *parser) {
  Token *keyword = parser_previous(parser);

  Expr *value = NULL;
  if (!parser_check(parser, MS_TOKEN_SEMICOLON) &&
      !parser_check(parser, MS_TOKEN_RBRACE)) {
    value = expression(parser);
  }

  if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
    // Semicolon is optional before }
    if (!parser_check(parser, MS_TOKEN_RBRACE)) {
      printf("Parse Error: Expect ';' after return value.\n");
      exit(1);
    }
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_RETURN;
  stmt->return_stmt.keyword = keyword;
  stmt->return_stmt.value = value;
  return stmt;
}

Stmt *block_statement(Parser *parser) {
  Stmt **statements = malloc(sizeof(Stmt *) * 100);
  int stmt_count = 0;

  while (!parser_check(parser, MS_TOKEN_RBRACE) && !parser_is_at_end(parser)) {
    statements[stmt_count++] = statement(parser);
  }

  if (!parser_match(parser, 1, MS_TOKEN_RBRACE)) {
    printf("Parse Error: Expect '}' after block.\n");
    exit(1);
  }

  Stmt *stmt = malloc(sizeof(Stmt));
  stmt->type = STMT_BLOCK;
  stmt->block.statements = statements;
  stmt->block.stmt_count = stmt_count;
  return stmt;
}

Stmt *statement(Parser *parser) {
  if (parser_match(parser, 1, MS_TOKEN_PRINT)) {
    return print_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_VAR)) {
    return var_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_ASSERT)) {
    return assert_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_IF)) {
    return if_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_WHILE)) {
    return while_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_FOR)) {
    return for_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_FUNCTION)) {
    return function_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_IMPORT)) {
    return import_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_RETURN)) {
    return return_statement(parser);
  }
  if (parser_match(parser, 1, MS_TOKEN_LBRACE)) {
    return block_statement(parser);
  }

  return expression_statement(parser);
}

Stmt **parse(Parser *parser) {
  int capacity = 100;
  Stmt **statements = malloc(sizeof(Stmt *) * capacity);
  int stmt_count = 0;

  while (!parser_is_at_end(parser)) {
    if (stmt_count >= capacity - 1) {
      capacity *= 2;
      statements = realloc(statements, sizeof(Stmt *) * capacity);
    }
    statements[stmt_count++] = statement(parser);
  }

  statements[stmt_count] = NULL; // Null terminator
  return statements;
}

// Forward declaration for the main interpreter loop
void interpret(Interpreter *interpreter, Stmt **statements);

// =============================================================================
// INTERPRETER IMPLEMENTATION
// =============================================================================

Environment *create_environment(Environment *enclosing) {
  Environment *env = malloc(sizeof(Environment));
  env->names = malloc(sizeof(char *) * 100);
  env->values = malloc(sizeof(Value) * 100);
  env->count = 0;
  env->capacity = 100;
  env->enclosing = enclosing;
  return env;
}

void env_define(Environment *env, char *name, Value value) {
  if (env->count >= env->capacity) {
    env->capacity *= 2;
    env->names = realloc(env->names, sizeof(char *) * env->capacity);
    env->values = realloc(env->values, sizeof(Value) * env->capacity);
  }

  env->names[env->count] = malloc(strlen(name) + 1);
  strcpy(env->names[env->count], name);
  env->values[env->count] = value;
  env->count++;
}

void env_assign(Environment *env, char *name, Value value) {
  for (int i = 0; i < env->count; i++) {
    if (strcmp(env->names[i], name) == 0) {
      env->values[i] = value;
      return;
    }
  }

  if (env->enclosing != NULL) {
    Environment *current = env;
    while (current != NULL) {
      for (int i = 0; i < current->count; i++) {
        if (strcmp(current->names[i], name) == 0) {
          current->values[i] = value;
          return;
        }
      }
      if (current->enclosing == NULL)
        break;
      current = current->enclosing;
    }
    // If not found, assign to global scope
    current->values[current->count] = value;
    current->names[current->count] = malloc(strlen(name) + 1);
    strcpy(current->names[current->count], name);
    current->count++;
  } else {
    // Assign to current environment
    env_define(env, name, value);
  }
}
Value env_get(Environment *env, char *name) {
  for (int i = 0; i < env->count; i++) {
    if (strcmp(env->names[i], name) == 0) {
      return env->values[i];
    }
  }

  if (env->enclosing != NULL) {
    return env_get(env->enclosing, name);
  }

  printf("Runtime Error: Undefined variable '%s'.\n", name);
  exit(1);
}

char *value_to_string(Value value) {
  static char buffer[256];

  switch (value.type) {
  case VALUE_NIL:
    strcpy(buffer, "nil");
    break;
  case VALUE_BOOL:
    strcpy(buffer, value.value.boolean ? "true" : "false");
    break;
  case VALUE_NUMBER: {
    double num = value.value.number;
    if (num == (int)num) {
      sprintf(buffer, "%d", (int)num);
    } else {
      sprintf(buffer, "%g", num);
    }
  } break;
  case VALUE_STRING:
    return value.value.string;
  case VALUE_CHAR:
    sprintf(buffer, "%c", value.value.character);
    break;
  default:
    strcpy(buffer, "unknown");
    break;
  }

  return buffer;
}

char *value_to_string_alloc(Value value) {
  switch (value.type) {
  case VALUE_NIL: {
    char *result = malloc(4);
    strcpy(result, "nil");
    return result;
  }
  case VALUE_BOOL: {
    char *result = malloc(6);
    strcpy(result, value.value.boolean ? "true" : "false");
    return result;
  }
  case VALUE_NUMBER: {
    char *result = malloc(32);
    double num = value.value.number;
    if (num == (int)num) {
      sprintf(result, "%d", (int)num);
    } else {
      sprintf(result, "%g", num);
    }
    return result;
  }
  case VALUE_STRING: {
    char *result = malloc(strlen(value.value.string) + 1);
    strcpy(result, value.value.string);
    return result;
  }
  case VALUE_CHAR: {
    char *result = malloc(2);
    sprintf(result, "%c", value.value.character);
    return result;
  }
  default: {
    char *result = malloc(8);
    strcpy(result, "unknown");
    return result;
  }
  }
}

bool is_truthy(Value value) {
  switch (value.type) {
  case VALUE_NIL:
    return false;
  case VALUE_BOOL:
    return value.value.boolean;
  case VALUE_NUMBER:
    return value.value.number != 0;
  default:
    return true;
  }
}

Value evaluate_literal(Interpreter *interpreter, Expr *expr) {
  return expr->literal.value;
}

Value evaluate_variable(Interpreter *interpreter, Expr *expr) {
  return env_get(interpreter->environment, expr->variable.name->lexeme);
}

Value evaluate_binary(Interpreter *interpreter, Expr *expr) {
  Value left = evaluate_expr(interpreter, expr->binary.left);
  Value right = evaluate_expr(interpreter, expr->binary.right);
  MSTokenType op = expr->binary.operator->type;

  Value result;
  result.type = VALUE_NUMBER;

  if (op == MS_TOKEN_PLUS) {
    if (left.type == VALUE_NUMBER && right.type == VALUE_NUMBER) {
      result.value.number = left.value.number + right.value.number;
    } else {
      // String concatenation
      result.type = VALUE_STRING;
      char *left_str = value_to_string_alloc(left);
      char *right_str = value_to_string_alloc(right);
      result.value.string = malloc(strlen(left_str) + strlen(right_str) + 1);
      strcpy(result.value.string, left_str);
      strcat(result.value.string, right_str);

      if (left.type != VALUE_STRING)
        free(left_str);
      if (right.type != VALUE_STRING)
        free(right_str);
    }
  } else if (op == MS_TOKEN_MINUS) {
    result.value.number = left.value.number - right.value.number;
  } else if (op == MS_TOKEN_MULTIPLY) {
    result.value.number = left.value.number * right.value.number;
  } else if (op == MS_TOKEN_DIVIDE) {
    if (right.value.number == 0) {
      printf("Runtime Error: Division by zero.\n");
      exit(1);
    }
    result.value.number = left.value.number / right.value.number;
  } else if (op == MS_TOKEN_GREATER) {
    result.type = VALUE_BOOL;
    result.value.boolean = left.value.number > right.value.number;
  } else if (op == MS_TOKEN_GREATER_EQUAL) {
    result.type = VALUE_BOOL;
    result.value.boolean = left.value.number >= right.value.number;
  } else if (op == MS_TOKEN_LESS) {
    result.type = VALUE_BOOL;
    result.value.boolean = left.value.number < right.value.number;
  } else if (op == MS_TOKEN_LESS_EQUAL) {
    result.type = VALUE_BOOL;
    result.value.boolean = left.value.number <= right.value.number;
  } else if (op == MS_TOKEN_EQUAL) {
    result.type = VALUE_BOOL;
    if (left.type == VALUE_NIL && right.type == VALUE_NIL) {
      result.value.boolean = true;
    } else if (left.type == VALUE_NIL || right.type == VALUE_NIL) {
      result.value.boolean = false;
    } else if (left.type != right.type) {
      result.value.boolean = false;
    } else if (left.type == VALUE_NUMBER) {
      result.value.boolean = left.value.number == right.value.number;
    } else if (left.type == VALUE_BOOL) {
      result.value.boolean = left.value.boolean == right.value.boolean;
    } else if (left.type == VALUE_STRING) {
      result.value.boolean = strcmp(left.value.string, right.value.string) == 0;
    } else {
      result.value.boolean = false;
    }
  } else if (op == MS_TOKEN_NOT_EQUAL) {
    result.type = VALUE_BOOL;
    if (left.type == VALUE_NIL && right.type == VALUE_NIL) {
      result.value.boolean = false;
    } else if (left.type == VALUE_NIL || right.type == VALUE_NIL) {
      result.value.boolean = true;
    } else if (left.type != right.type) {
      result.value.boolean = true;
    } else if (left.type == VALUE_NUMBER) {
      result.value.boolean = left.value.number != right.value.number;
    } else if (left.type == VALUE_BOOL) {
      result.value.boolean = left.value.boolean != right.value.boolean;
    } else if (left.type == VALUE_STRING) {
      result.value.boolean = strcmp(left.value.string, right.value.string) != 0;
    } else {
      result.value.boolean = true;
    }
  }

  return result;
}

Value evaluate_unary(Interpreter *interpreter, Expr *expr) {
  Value right = evaluate_expr(interpreter, expr->unary.right);
  MSTokenType op = expr->unary.operator->type;

  Value result;

  if (op == MS_TOKEN_MINUS) {
    result.type = VALUE_NUMBER;
    result.value.number = -right.value.number;
  } else if (op == MS_TOKEN_NOT) {
    result.type = VALUE_BOOL;
    result.value.boolean = !is_truthy(right);
  }

  return result;
}

Value evaluate_assign(Interpreter *interpreter, Expr *expr) {
  Value value = evaluate_expr(interpreter, expr->assign.value);
  env_assign(interpreter->environment, expr->assign.name->lexeme, value);
  return value;
}

Value evaluate_call(Interpreter *interpreter, Expr *expr) {
  if (expr->call.callee->type != EXPR_VARIABLE) {
    printf("Runtime Error: Can only call functions by name.\n");
    exit(1);
  }

  char *func_name = expr->call.callee->variable.name->lexeme;

  // Check for built-in functions
  for (int i = 0; i < interpreter->builtin_count; i++) {
    if (strcmp(func_name, interpreter->builtins[i].name) == 0) {
      if (expr->call.arg_count != interpreter->builtins[i].arity) {
        printf(
            "Runtime Error: Function '%s' expects %d arguments, but got %d.\n",
            func_name, interpreter->builtins[i].arity, expr->call.arg_count);
        exit(1);
      }

      Value *arg_values = malloc(sizeof(Value) * expr->call.arg_count);
      for (int j = 0; j < expr->call.arg_count; j++) {
        arg_values[j] = evaluate_expr(interpreter, expr->call.arguments[j]);
      }

      Value result = interpreter->builtins[i].call(interpreter, arg_values);
      free(arg_values);
      return result;
    }
  }

  // Check if it's a user-defined function
  Value func_value = env_get(interpreter->environment, func_name);
  if (func_value.type == VALUE_FUNCTION) {
    Stmt *func_stmt = func_value.value.function_stmt;

    if (expr->call.arg_count != func_stmt->function.param_count) {
      printf("Runtime Error: Function '%s' expects %d arguments, got %d.\n",
             func_name, func_stmt->function.param_count, expr->call.arg_count);
      exit(1);
    }

    Value *arg_values = malloc(sizeof(Value) * expr->call.arg_count);
    for (int i = 0; i < expr->call.arg_count; i++) {
      arg_values[i] = evaluate_expr(interpreter, expr->call.arguments[i]);
    }

    Environment *previous = interpreter->environment;
    Environment *func_env = create_environment(interpreter->globals);
    interpreter->environment = func_env;

    for (int i = 0; i < func_stmt->function.param_count; i++) {
      env_define(func_env, func_stmt->function.params[i]->lexeme,
                 arg_values[i]);
    }
    free(arg_values);

    interpreter->has_returned = false;
    for (int i = 0; i < func_stmt->function.body_count; i++) {
      execute_stmt(interpreter, func_stmt->function.body[i]);
      if (interpreter->has_returned) {
        break;
      }
    }

    Value result;
    if (interpreter->has_returned) {
      result = interpreter->return_value;
    } else {
      result.type = VALUE_NIL;
    }

    interpreter->environment = previous;
    return result;
  }

  printf("Runtime Error: Unknown function '%s'.\n", func_name);
  exit(1);
}

Value evaluate_logical(Interpreter *interpreter, Expr *expr) {
  Value left = evaluate_expr(interpreter, expr->logical.left);

  if (expr->logical.operator->type == MS_TOKEN_OR) {
    if (is_truthy(left)) {
      Value result;
      result.type = VALUE_BOOL;
      result.value.boolean = true;
      return result;
    }
  } else { // AND
    if (!is_truthy(left)) {
      Value result;
      result.type = VALUE_BOOL;
      result.value.boolean = false;
      return result;
    }
  }

  Value right = evaluate_expr(interpreter, expr->logical.right);
  Value result;
  result.type = VALUE_BOOL;
  result.value.boolean = is_truthy(right);
  return result;
}

Value evaluate_grouping(Interpreter *interpreter, Expr *expr) {
  return evaluate_expr(interpreter, expr->grouping.expression);
}

Value evaluate_list_literal(Interpreter *interpreter, Expr *expr) {
  Value list_value;
  list_value.type = VALUE_LIST;
  list_value.list_size = expr->list_literal.element_count;
  list_value.value.elements = malloc(sizeof(Expr *) * list_value.list_size);

  // Evaluate each element and store the result
  for (int i = 0; i < list_value.list_size; i++) {
    Value element_value =
        evaluate_expr(interpreter, expr->list_literal.elements[i]);
    // For now, store as literal expressions - a more complete implementation
    // would need a different storage approach for runtime values
    Expr *literal_expr = malloc(sizeof(Expr));
    literal_expr->type = EXPR_LITERAL;
    literal_expr->literal.value = element_value;
    list_value.value.elements[i] = literal_expr;
  }

  return list_value;
}

Value evaluate_get(Interpreter *interpreter, Expr *expr) {
  Value object = evaluate_expr(interpreter, expr->get.object);

  if (object.type != VALUE_LIST) {
    printf("Runtime Error: Only lists can be indexed.\n");
    exit(1);
  }

  Value index_value = evaluate_expr(interpreter, expr->get.index);
  if (index_value.type != VALUE_NUMBER) {
    printf("Runtime Error: List index must be a number.\n");
    exit(1);
  }

  int index = (int)index_value.value.number;
  if (index < 0 || index >= object.list_size) {
    printf("Runtime Error: List index out of bounds.\n");
    exit(1);
  }

  // Return the value from the stored literal expression
  return object.value.elements[index]->literal.value;
}

Value evaluate_set(Interpreter *interpreter, Expr *expr) {
  Value object = evaluate_expr(interpreter, expr->set.object);

  if (object.type != VALUE_LIST) {
    printf("Runtime Error: Only lists can be indexed for assignment.\n");
    exit(1);
  }

  Value index_value = evaluate_expr(interpreter, expr->set.index);
  if (index_value.type != VALUE_NUMBER) {
    printf("Runtime Error: List index must be a number.\n");
    exit(1);
  }

  int index = (int)index_value.value.number;
  if (index < 0 || index >= object.list_size) {
    printf("Runtime Error: List index out of bounds.\n");
    exit(1);
  }

  Value new_value = evaluate_expr(interpreter, expr->set.value);

  // Update the value in the list
  Expr *literal_expr = malloc(sizeof(Expr));
  literal_expr->type = EXPR_LITERAL;
  literal_expr->literal.value = new_value;
  object.value.elements[index] = literal_expr;

  return new_value;
}

Value evaluate_expr(Interpreter *interpreter, Expr *expr) {
  switch (expr->type) {
  case EXPR_LITERAL:
    return evaluate_literal(interpreter, expr);
  case EXPR_VARIABLE:
    return evaluate_variable(interpreter, expr);
  case EXPR_ASSIGN:
    return evaluate_assign(interpreter, expr);
  case EXPR_BINARY:
    return evaluate_binary(interpreter, expr);
  case EXPR_UNARY:
    return evaluate_unary(interpreter, expr);
  case EXPR_CALL:
    return evaluate_call(interpreter, expr);
  case EXPR_LOGICAL:
    return evaluate_logical(interpreter, expr);
  case EXPR_GROUPING:
    return evaluate_grouping(interpreter, expr);
  case EXPR_LIST_LITERAL:
    return evaluate_list_literal(interpreter, expr);
  case EXPR_GET:
    return evaluate_get(interpreter, expr);
  case EXPR_SET:
    return evaluate_set(interpreter, expr);
  default:
    printf("Runtime Error: Unknown expression type.\n");
    exit(1);
  }
}

void execute_expression_stmt(Interpreter *interpreter, Stmt *stmt) {
  evaluate_expr(interpreter, stmt->expression.expression);
}

void execute_print_stmt(Interpreter *interpreter, Stmt *stmt) {
  for (int i = 0; i < stmt->print.expr_count; i++) {
    Value value = evaluate_expr(interpreter, stmt->print.expressions[i]);
    printf("%s", value_to_string(value));
    if (i < stmt->print.expr_count - 1)
      printf(" ");
  }
  printf("\n");
}

void execute_var_stmt(Interpreter *interpreter, Stmt *stmt) {
  Value value;
  value.type = VALUE_NIL;

  if (stmt->var.initializer != NULL) {
    value = evaluate_expr(interpreter, stmt->var.initializer);
  }

  env_define(interpreter->environment, stmt->var.name->lexeme, value);
}

void execute_assert_stmt(Interpreter *interpreter, Stmt *stmt) {
  Value condition = evaluate_expr(interpreter, stmt->assert_stmt.condition);

  if (!is_truthy(condition)) {
    Value message = evaluate_expr(interpreter, stmt->assert_stmt.message);
    printf("Runtime Error: Assertion failed: %s\n", value_to_string(message));
    exit(1);
  }
}

void execute_if_stmt(Interpreter *interpreter, Stmt *stmt) {
  Value condition = evaluate_expr(interpreter, stmt->if_stmt.condition);

  if (is_truthy(condition)) {
    execute_stmt(interpreter, stmt->if_stmt.then_branch);
  } else if (stmt->if_stmt.else_branch != NULL) {
    execute_stmt(interpreter, stmt->if_stmt.else_branch);
  }
}

void execute_while_stmt(Interpreter *interpreter, Stmt *stmt) {
  while (true) {
    Value condition = evaluate_expr(interpreter, stmt->while_stmt.condition);
    if (!is_truthy(condition))
      break;
    execute_stmt(interpreter, stmt->while_stmt.body);
  }
}

void execute_for_stmt(Interpreter *interpreter, Stmt *stmt) {
  // Create new scope for the for loop
  Environment *previous = interpreter->environment;
  Environment *for_env = create_environment(previous);
  interpreter->environment = for_env;

  // Execute initializer
  if (stmt->for_stmt.initializer != NULL) {
    execute_stmt(interpreter, stmt->for_stmt.initializer);
  }

  // Loop with condition and increment
  while (true) {
    // Check condition
    Value condition = evaluate_expr(interpreter, stmt->for_stmt.condition);
    if (!is_truthy(condition))
      break;

    // Execute body
    execute_stmt(interpreter, stmt->for_stmt.body);

    // Execute increment
    if (stmt->for_stmt.increment != NULL) {
      evaluate_expr(interpreter, stmt->for_stmt.increment);
    }
  }

  // Restore previous scope
  interpreter->environment = previous;
}

void execute_function_stmt(Interpreter *interpreter, Stmt *stmt) {
  // Store function definition in environment as a special VALUE_FUNCTION
  Value func_value;
  func_value.type = VALUE_FUNCTION;
  // For simplicity, we'll store the function statement itself in the value
  // This is a hack for a basic implementation
  func_value.value.function_stmt = stmt;

  env_define(interpreter->environment, stmt->function.name->lexeme, func_value);
}

// Forward declarations for cleanup helpers used by import (defined later)
static void free_stmt(struct Stmt *stmt);
static void free_statements(struct Stmt **statements);
static void free_tokens(Token *tokens, int count);

void execute_import_stmt(Interpreter *interpreter, Stmt *stmt) {
  char *path = stmt->import.path_token->literal.string;

  char *full_path = malloc(strlen(path) + 4);
  sprintf(full_path, "%s.ms", path);

  char *source = read_file(full_path);
  if (source == NULL) {
    printf("Runtime Error: Could not open module '%s'.\n", full_path);
    free(full_path);
    exit(1);
  }

  Lexer *lexer = create_lexer(source, full_path);
  Token *tokens = scan_tokens(lexer);
  Parser *parser = create_parser(tokens, lexer->token_count, full_path);
  Stmt **statements = parse(parser);

  interpret(interpreter, statements);

  // Cleanup imported module resources
  free_statements(statements);             // deep free AST
  free_tokens(tokens, lexer->token_count); // tokens + lexeme/literal strings
  free(parser);                            // parser struct
  free(lexer);     // lexer struct (its tokens already freed)
  free(source);    // source code buffer
  free(full_path); // constructed path string
}

void execute_return_stmt(Interpreter *interpreter, Stmt *stmt) {
  if (stmt->return_stmt.value != NULL) {
    interpreter->return_value =
        evaluate_expr(interpreter, stmt->return_stmt.value);
  } else {
    interpreter->return_value.type = VALUE_NIL;
  }
  interpreter->has_returned = true;
}

void execute_block_stmt(Interpreter *interpreter, Stmt *stmt) {
  Environment *previous = interpreter->environment;
  Environment *block_env = create_environment(previous);
  interpreter->environment = block_env;

  for (int i = 0; i < stmt->block.stmt_count; i++) {
    execute_stmt(interpreter, stmt->block.statements[i]);
  }

  interpreter->environment = previous;
  // Note: In a production implementation, we'd want to free block_env
}

void execute_stmt(Interpreter *interpreter, Stmt *stmt) {
  switch (stmt->type) {
  case STMT_EXPRESSION:
    execute_expression_stmt(interpreter, stmt);
    break;
  case STMT_PRINT:
    execute_print_stmt(interpreter, stmt);
    break;
  case STMT_VAR:
    execute_var_stmt(interpreter, stmt);
    break;
  case STMT_ASSERT:
    execute_assert_stmt(interpreter, stmt);
    break;
  case STMT_IF:
    execute_if_stmt(interpreter, stmt);
    break;
  case STMT_BLOCK:
    execute_block_stmt(interpreter, stmt);
    break;
  case STMT_WHILE:
    execute_while_stmt(interpreter, stmt);
    break;
  case STMT_FOR:
    execute_for_stmt(interpreter, stmt);
    break;
  case STMT_FUNCTION:
    execute_function_stmt(interpreter, stmt);
    break;
  case STMT_IMPORT:
    execute_import_stmt(interpreter, stmt);
    break;
  case STMT_RETURN:
    execute_return_stmt(interpreter, stmt);
    break;
  default:
    printf("Runtime Error: Unknown statement type.\n");
    exit(1);
  }
}

// Built-in function implementations
Value builtin_len(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type == VALUE_STRING) {
    Value result;
    result.type = VALUE_NUMBER;
    result.value.number = strlen(arguments[0].value.string);
    return result;
  } else if (arguments[0].type == VALUE_LIST) {
    Value result;
    result.type = VALUE_NUMBER;
    result.value.number = arguments[0].list_size;
    return result;
  } else {
    printf("Runtime Error: len() expects a string or list argument.\n");
    exit(1);
  }
}

Value builtin_fopen(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_STRING || arguments[1].type != VALUE_STRING) {
    printf(
        "Runtime Error: fopen() expects two string arguments (path, mode).\n");
    exit(1);
  }
  char *path = arguments[0].value.string;
  char *mode = arguments[1].value.string;
  FILE *file = fopen(path, mode);
  Value result;
  if (file == NULL) {
    result.type = VALUE_NIL;
  } else {
    result.type = VALUE_FILE;
    result.value.file_handle = file;
  }
  return result;
}

Value builtin_fclose(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_FILE) {
    printf("Runtime Error: fclose() expects a file handle.\n");
    exit(1);
  }
  int ret = fclose(arguments[0].value.file_handle);
  Value result;
  result.type = VALUE_NUMBER;
  result.value.number = ret;
  return result;
}

Value builtin_fwrite(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_FILE || arguments[1].type != VALUE_STRING) {
    printf("Runtime Error: fwrite() expects a file handle and a string.\n");
    exit(1);
  }
  size_t written =
      fwrite(arguments[1].value.string, 1, strlen(arguments[1].value.string),
             arguments[0].value.file_handle);
  Value result;
  result.type = VALUE_NUMBER;
  result.value.number = written;
  return result;
}

Value builtin_fread(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_FILE) {
    printf("Runtime Error: fread() expects a file handle.\n");
    exit(1);
  }
  FILE *file = arguments[0].value.file_handle;
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *buffer = malloc(size + 1);
  fread(buffer, 1, size, file);
  buffer[size] = '\0';
  Value result;
  result.type = VALUE_STRING;
  result.value.string = buffer;
  return result;
}

Value builtin_freadline(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_FILE) {
    printf("Runtime Error: freadline() expects a file handle.\n");
    exit(1);
  }
  FILE *file = arguments[0].value.file_handle;
  char *line = NULL;
  size_t len = 0;
  ssize_t read = getline(&line, &len, file);
  if (read == -1) {
    Value result;
    result.type = VALUE_NIL;
    if (line)
      free(line);
    return result;
  }
  if (read > 0 && line[read - 1] == '\n') {
    line[read - 1] = '\0';
    if (read > 1 && line[read - 2] == '\r') {
      line[read - 2] = '\0';
    }
  }
  Value result;
  result.type = VALUE_STRING;
  result.value.string = line;
  return result;
}

Value builtin_fwriteline(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_FILE || arguments[1].type != VALUE_STRING) {
    printf("Runtime Error: fwriteline() expects a file handle and a string.\n");
    exit(1);
  }
  fprintf(arguments[0].value.file_handle, "%s\n", arguments[1].value.string);
  Value result;
  result.type = VALUE_NIL;
  return result;
}

Value builtin_fexists(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  if (arguments[0].type != VALUE_STRING) {
    printf("Runtime Error: fexists() expects a string argument (path).\n");
    exit(1);
  }
  Value result;
  result.type = VALUE_BOOL;
  if (access(arguments[0].value.string, F_OK) != -1) {
    result.value.boolean = true;
  } else {
    result.value.boolean = false;
  }
  return result;
}

// Revised time builtins using epoch seconds (NUMBER) as the canonical time
// value.
Value builtin_time_parse(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value out;
  out.type = VALUE_NIL;
  if (arguments[0].type != VALUE_STRING || arguments[1].type != VALUE_STRING)
    return out;
  const char *date_str = arguments[0].value.string;
  const char *fmt = arguments[1].value.string;
  struct tm tmval;
  memset(&tmval, 0, sizeof(tmval));
  tmval.tm_isdst = -1;
  // Support a subset: "%Y-%m-%d %H:%M:%S" or "%Y-%m-%d" with matching fmt.
  int Y = 0, m = 0, d = 0, H = 0, M = 0, S = 0;
  int matched = 0;
  if (strcmp(fmt, "%Y-%m-%d %H:%M:%S") == 0) {
    matched = sscanf(date_str, "%d-%d-%d %d:%d:%d", &Y, &m, &d, &H, &M, &S);
    if (matched != 6)
      return out;
  } else if (strcmp(fmt, "%Y-%m-%d") == 0) {
    matched = sscanf(date_str, "%d-%d-%d", &Y, &m, &d);
    if (matched != 3)
      return out;
  } else {
    return out; // unsupported format
  }
  tmval.tm_year = Y - 1900;
  tmval.tm_mon = m - 1;
  tmval.tm_mday = d;
  tmval.tm_hour = H;
  tmval.tm_min = M;
  tmval.tm_sec = S;
  time_t epoch = mktime(&tmval);
  if (epoch <= 0)
    return out;
  out.type = VALUE_NUMBER;
  out.value.number = (double)epoch;
  return out;
}
Value builtin_time_format(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value out;
  out.type = VALUE_NIL;
  if (arguments[0].type != VALUE_NUMBER || arguments[1].type != VALUE_STRING)
    return out;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  if (!tmval)
    return out;
  char buf[64];
  if (strftime(buf, sizeof(buf), arguments[1].value.string, tmval) == 0)
    return out;
  out.type = VALUE_STRING;
  out.value.string = malloc(strlen(buf) + 1);
  strcpy(out.value.string, buf);
  return out;
}
Value builtin_time_year(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  r.value.number = tmval ? (tmval->tm_year + 1900) : 0;
  return r;
}
Value builtin_time_month(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  r.value.number = tmval ? (tmval->tm_mon + 1) : 0;
  return r;
}
Value builtin_time_day(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  r.value.number = tmval ? tmval->tm_mday : 0;
  return r;
}
Value builtin_time_hour(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  r.value.number = tmval ? tmval->tm_hour : 0;
  return r;
}
Value builtin_time_minute(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  r.value.number = tmval ? tmval->tm_min : 0;
  return r;
}
Value builtin_time_second(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  r.value.number = tmval ? tmval->tm_sec : 0;
  return r;
}
// Map Monday=0 ... Sunday=6 -> adjust so Saturday=5 for test expectation
// (tm_wday: Sunday=0)
Value builtin_time_weekday(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  time_t t = (time_t)arguments[0].value.number;
  struct tm *tmval = localtime(&t);
  if (!tmval) {
    r.value.number = 0;
    return r;
  }
  int w = tmval->tm_wday; // 0=Sun
  int mapped = (w == 0) ? 6 : (w - 1);
  r.value.number = mapped;
  return r;
}
Value builtin_time_add(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value out;
  if (arguments[0].type != VALUE_NUMBER || arguments[1].type != VALUE_NUMBER) {
    out.type = VALUE_NIL;
    return out;
  }
  out.type = VALUE_NUMBER;
  out.value.number = arguments[0].value.number + arguments[1].value.number;
  return out;
}
Value builtin_time_diff(Interpreter *interpreter, Value *arguments) {
  (void)interpreter;
  Value r;
  r.type = VALUE_NUMBER;
  if (arguments[0].type != VALUE_NUMBER || arguments[1].type != VALUE_NUMBER) {
    r.value.number = 0;
    return r;
  }
  r.value.number = arguments[0].value.number - arguments[1].value.number;
  return r;
}

void register_builtin(Interpreter *interpreter, char *name, int arity,
                      Value (*call)(Interpreter *interpreter,
                                    Value *arguments)) {
  if (interpreter->builtins == NULL) {
    interpreter->builtins =
        malloc(sizeof(BuiltinFunction) * 20); // initial capacity
    interpreter->builtin_count = 0;
  }
  // TODO: realloc if needed
  interpreter->builtins[interpreter->builtin_count].name = name;
  interpreter->builtins[interpreter->builtin_count].arity = arity;
  interpreter->builtins[interpreter->builtin_count].call = call;
  interpreter->builtin_count++;
}

void init_builtins(Interpreter *interpreter) {
  register_builtin(interpreter, "len", 1, builtin_len);
  register_builtin(interpreter, "fopen", 2, builtin_fopen);
  register_builtin(interpreter, "fclose", 1, builtin_fclose);
  register_builtin(interpreter, "fwrite", 2, builtin_fwrite);
  register_builtin(interpreter, "fread", 1, builtin_fread);
  register_builtin(interpreter, "freadline", 1, builtin_freadline);
  register_builtin(interpreter, "fwriteline", 2, builtin_fwriteline);
  register_builtin(interpreter, "fexists", 1, builtin_fexists);
  register_builtin(interpreter, "time_parse", 2, builtin_time_parse);
  register_builtin(interpreter, "time_format", 2, builtin_time_format);
  register_builtin(interpreter, "time_year", 1, builtin_time_year);
  register_builtin(interpreter, "time_month", 1, builtin_time_month);
  register_builtin(interpreter, "time_day", 1, builtin_time_day);
  register_builtin(interpreter, "time_hour", 1, builtin_time_hour);
  register_builtin(interpreter, "time_minute", 1, builtin_time_minute);
  register_builtin(interpreter, "time_second", 1, builtin_time_second);
  register_builtin(interpreter, "time_weekday", 1, builtin_time_weekday);
  register_builtin(interpreter, "time_add", 2, builtin_time_add);
  register_builtin(interpreter, "time_diff", 2, builtin_time_diff);
}

Interpreter *create_interpreter(char *filename) {
  Interpreter *interpreter = malloc(sizeof(Interpreter));
  interpreter->globals = create_environment(NULL);
  interpreter->environment = interpreter->globals;
  interpreter->filename = filename;
  interpreter->builtins = NULL;
  interpreter->builtin_count = 0;
  init_builtins(interpreter);
  interpreter->has_returned = false;
  interpreter->return_value.type = VALUE_NIL;
  return interpreter;
}

// Basic environment freeing. NOTE: This only frees the single environment
// instance passed in (and its chain via enclosing, if any). Environments that
// were created for blocks/functions and became unreachable during execution
// are not tracked and thus cannot be freed here (they leak currently). A more
// complete solution would keep a list of allocated environments.
static void free_environment(Environment *env) {
  while (env != NULL) {
    for (int i = 0; i < env->count; i++) {
      if (env->names[i]) {
        free(env->names[i]);
      }
      // Free dynamically allocated value payloads where we own memory.
      if (env->values) {
        Value *v = &env->values[i];
        switch (v->type) {
        case VALUE_STRING:
          if (v->value.string)
            free(v->value.string);
          break;
        case VALUE_LIST:
          if (v->value.elements) {
            for (int li = 0; li < v->list_size; li++) {
              Expr *elem_expr = v->value.elements[li];
              if (elem_expr && elem_expr->type == EXPR_LITERAL) {
                Value elem_val = elem_expr->literal.value;
                if (elem_val.type == VALUE_STRING && elem_val.value.string)
                  free(elem_val.value.string);
              }
              if (elem_expr)
                free(elem_expr);
            }
            free(v->value.elements);
          }
          break;
        default:
          break; // other types have no heap allocation here
        }
      }
    }
    char **names = env->names;
    Value *values = env->values;
    Environment *enclosing = env->enclosing;
    free(names);
    free(values);
    if (env != NULL)
      free(env);
    env = enclosing; // walk up chain
  }
}

// Forward declarations for cleanup helpers
typedef struct CleanupBundle {
  Token *tokens;
  int token_count;
  Stmt **statements; // NULL terminated
} CleanupBundle;

static void
free_expr(struct Expr *expr); // struct Expr forward (already typedef'd)
static void free_stmt(struct Stmt *stmt);

static void free_expr_array(struct Expr **arr, int count) {
  if (!arr)
    return;
  for (int i = 0; i < count; i++)
    if (arr[i])
      free_expr(arr[i]);
  free(arr);
}

static void free_tokens(Token *tokens, int count) {
  if (!tokens)
    return;
  for (int i = 0; i < count; i++) {
    if (tokens[i].lexeme)
      free(tokens[i].lexeme);
    if (tokens[i].type == MS_TOKEN_STRING && tokens[i].literal.string)
      free(tokens[i].literal.string);
  }
  free(tokens);
}

static void free_function(UserFunction *fn) {
  if (!fn)
    return;
  for (int i = 0; i < fn->body_count; i++)
    if (fn->body[i])
      free_stmt(fn->body[i]);
  free(fn->body);
  free(fn->params);
}

static void free_expr(struct Expr *expr) {
  if (!expr)
    return;
  switch (expr->type) {
  case EXPR_LITERAL:
    // String memory is freed when values stored in environments are cleaned.
    // Avoid double-free by not freeing here.
    break;
  case EXPR_VARIABLE:
    break;
  case EXPR_ASSIGN:
    free_expr(expr->assign.value);
    break;
  case EXPR_BINARY:
    free_expr(expr->binary.left);
    free_expr(expr->binary.right);
    break;
  case EXPR_UNARY:
    free_expr(expr->unary.right);
    break;
  case EXPR_CALL:
    free_expr(expr->call.callee);
    for (int i = 0; i < expr->call.arg_count; i++)
      free_expr(expr->call.arguments[i]);
    free(expr->call.arguments);
    break;
  case EXPR_GROUPING:
    free_expr(expr->grouping.expression);
    break;
  case EXPR_LOGICAL:
    free_expr(expr->logical.left);
    free_expr(expr->logical.right);
    break;
  case EXPR_LIST_LITERAL:
    for (int i = 0; i < expr->list_literal.element_count; i++)
      free_expr(expr->list_literal.elements[i]);
    free(expr->list_literal.elements);
    break;
  case EXPR_GET:
    free_expr(expr->get.object);
    free_expr(expr->get.index);
    break;
  case EXPR_SET:
    free_expr(expr->set.object);
    free_expr(expr->set.index);
    free_expr(expr->set.value);
    break;
  }
  free(expr);
}

static void free_stmt(struct Stmt *stmt) {
  if (!stmt)
    return;
  switch (stmt->type) {
  case STMT_EXPRESSION:
    free_expr(stmt->expression.expression);
    break;
  case STMT_PRINT:
    for (int i = 0; i < stmt->print.expr_count; i++)
      free_expr(stmt->print.expressions[i]);
    free(stmt->print.expressions);
    break;
  case STMT_VAR:
    free_expr(stmt->var.initializer);
    break;
  case STMT_ASSERT:
    free_expr(stmt->assert_stmt.condition);
    free_expr(stmt->assert_stmt.message);
    break;
  case STMT_IF:
    free_expr(stmt->if_stmt.condition);
    free_stmt(stmt->if_stmt.then_branch);
    free_stmt(stmt->if_stmt.else_branch);
    break;
  case STMT_WHILE:
    free_expr(stmt->while_stmt.condition);
    free_stmt(stmt->while_stmt.body);
    break;
  case STMT_FOR:
    free_stmt(stmt->for_stmt.initializer);
    free_expr(stmt->for_stmt.condition);
    free_expr(stmt->for_stmt.increment);
    free_stmt(stmt->for_stmt.body);
    break;
  case STMT_BLOCK:
    for (int i = 0; i < stmt->block.stmt_count; i++)
      free_stmt(stmt->block.statements[i]);
    free(stmt->block.statements);
    break;
  case STMT_FUNCTION:
    for (int i = 0; i < stmt->function.body_count; i++)
      if (stmt->function.body[i])
        free_stmt(stmt->function.body[i]);
    free(stmt->function.body);
    free(stmt->function.params);
    break;
  case STMT_RETURN:
    free_expr(stmt->return_stmt.value);
    break;
  case STMT_IMPORT:
    break;
  default:
    break;
  }
  free(stmt);
}

static void free_statements(Stmt **statements) {
  if (!statements)
    return;
  for (int i = 0; statements[i] != NULL; i++)
    free_stmt(statements[i]);
  free(statements);
}

void destroy_interpreter(Interpreter *interpreter) {
  if (!interpreter)
    return;
  if (interpreter->builtins)
    free(interpreter->builtins);
  free_environment(interpreter->globals);
  free(interpreter);
}

void interpret(Interpreter *interpreter, Stmt **statements) {
  for (int i = 0; statements[i] != NULL; i++) {
    execute_stmt(interpreter, statements[i]);
  }
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

void run(char *source, char *filename) {
  Lexer *lexer = create_lexer(source, filename);
  Token *tokens = scan_tokens(lexer);

  Parser *parser = create_parser(tokens, lexer->token_count, filename);
  Stmt **statements = parse(parser);

  Interpreter *interpreter = create_interpreter(filename);
  interpret(interpreter, statements);
  // Cleanup order: interpreter (envs), AST, tokens, lexer
  destroy_interpreter(interpreter);
  free_statements(statements);
  free_tokens(tokens, lexer->token_count); // frees internal token array
  free(parser);
  free(lexer); // lexer struct only; tokens already freed
}

void run_file(char *path) {
  printf("Mini Script Language Interpreter\n");
  printf("=================================\n");

  // Create display path
  char display_path[1024];
  strcpy(display_path, path);

  // Normalize path separators
  for (char *p = display_path; *p; p++) {
    if (*p == '\\')
      *p = '/';
  }

  // Find tests/ marker for relative display
  char *tests_marker = strstr(display_path, "tests/");
  if (tests_marker != NULL) {
    memmove(display_path, tests_marker, strlen(tests_marker) + 1);
  }

  printf("Executing: %s\n", display_path);
  printf("---------------------------------\n\n");

  char *source = read_file(path);
  if (source == NULL) {
    exit(1);
  }

  run(source, path);
  free(source);
}

void run_prompt() {
  printf("Mini Script REPL (type 'exit' to quit)\n");
  char line[1024];

  while (true) {
    printf("> ");
    if (fgets(line, sizeof(line), stdin) == NULL)
      break;

    // Remove newline
    line[strcspn(line, "\n")] = 0;

    if (strcmp(line, "exit") == 0)
      break;

    run(line, "<REPL>");
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    printf("Usage: mini_script [script]\n");
    return 64;
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    run_prompt();
  }

  return 0;
}
