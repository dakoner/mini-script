#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
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
    MS_TOKEN_LPAREN, MS_TOKEN_RPAREN, MS_TOKEN_LBRACE, MS_TOKEN_RBRACE, 
    MS_TOKEN_LBRACKET, MS_TOKEN_RBRACKET, MS_TOKEN_COMMA, MS_TOKEN_DOT, 
    MS_TOKEN_MINUS, MS_TOKEN_PLUS, MS_TOKEN_SEMICOLON, MS_TOKEN_DIVIDE, MS_TOKEN_MULTIPLY,
    
    // One or two character tokens
    MS_TOKEN_NOT, MS_TOKEN_NOT_EQUAL, MS_TOKEN_ASSIGN, MS_TOKEN_EQUAL,
    MS_TOKEN_GREATER, MS_TOKEN_GREATER_EQUAL, MS_TOKEN_LESS, MS_TOKEN_LESS_EQUAL,
    MS_TOKEN_AND, MS_TOKEN_OR,
    
    // Literals
    MS_TOKEN_IDENTIFIER, MS_TOKEN_STRING, MS_TOKEN_NUMBER, MS_TOKEN_CHAR,
    
    // Keywords
    MS_TOKEN_PRINT, MS_TOKEN_ELSE, MS_TOKEN_FALSE, MS_TOKEN_FOR, MS_TOKEN_FUNCTION,
    MS_TOKEN_IF, MS_TOKEN_RETURN, MS_TOKEN_TRUE, MS_TOKEN_WHILE, MS_TOKEN_IMPORT, MS_TOKEN_FROM,
    MS_TOKEN_INT_TYPE, MS_TOKEN_FLOAT_TYPE, MS_TOKEN_CHAR_TYPE, MS_TOKEN_STRING_TYPE,
    MS_TOKEN_LIST, MS_TOKEN_MAP, MS_TOKEN_LOADLIB, MS_TOKEN_GETPROC, MS_TOKEN_FREELIB,
    MS_TOKEN_CALLEXT, MS_TOKEN_ASSERT, MS_TOKEN_VAR, MS_TOKEN_NIL,
    
    MS_TOKEN_EOF
} MSTokenType;

typedef struct {
    MSTokenType type;
    char* lexeme;
    union {
        double number;
        char* string;
        char character;
        bool boolean;
    } literal;
    int line;
} Token;

typedef struct {
    char* source;
    char* filename;
    Token* tokens;
    int token_count;
    int token_capacity;
    int start;
    int current;
    int line;
} Lexer;

// Token keyword mapping
typedef struct {
    char* text;
    MSTokenType type;
} Keyword;

static Keyword keywords[] = {
    {"print", MS_TOKEN_PRINT}, {"if", MS_TOKEN_IF}, {"else", MS_TOKEN_ELSE},
    {"while", MS_TOKEN_WHILE}, {"for", MS_TOKEN_FOR}, {"function", MS_TOKEN_FUNCTION},
    {"return", MS_TOKEN_RETURN}, {"true", MS_TOKEN_TRUE}, {"false", MS_TOKEN_FALSE},
    {"import", MS_TOKEN_IMPORT}, {"from", MS_TOKEN_FROM}, {"int", MS_TOKEN_INT_TYPE},
    {"float", MS_TOKEN_FLOAT_TYPE}, {"char", MS_TOKEN_CHAR_TYPE}, 
    {"string", MS_TOKEN_STRING_TYPE}, {"list", MS_TOKEN_LIST}, {"map", MS_TOKEN_MAP},
    {"loadlib", MS_TOKEN_LOADLIB}, {"getproc", MS_TOKEN_GETPROC}, 
    {"freelib", MS_TOKEN_FREELIB}, {"callext", MS_TOKEN_CALLEXT},
    {"assert", MS_TOKEN_ASSERT}, {"var", MS_TOKEN_VAR}, {"nil", MS_TOKEN_NIL},
    {NULL, MS_TOKEN_EOF}
};

// =============================================================================
// 2. ABSTRACT SYNTAX TREE (AST) NODES
// =============================================================================

typedef enum {
    EXPR_ASSIGN, EXPR_BINARY, EXPR_CALL, EXPR_GROUPING, EXPR_LITERAL,
    EXPR_LIST_LITERAL, EXPR_GET, EXPR_SET, EXPR_LOGICAL, EXPR_UNARY, EXPR_VARIABLE
} ExprType;

typedef enum {
    STMT_BLOCK, STMT_EXPRESSION, STMT_PRINT, STMT_FUNCTION, STMT_IF,
    STMT_RETURN, STMT_WHILE, STMT_FOR, STMT_IMPORT, STMT_ASSERT, STMT_VAR
} StmtType;

typedef struct Expr Expr;
typedef struct Stmt Stmt;

typedef struct {
    union {
        double number;
        char* string;
        char character;
        bool boolean;
        struct Expr** elements;  // For lists
        FILE* file_handle;       // For file handles
        struct Stmt* function_stmt;  // For user-defined functions
    } value;
    enum {
        VALUE_NIL, VALUE_BOOL, VALUE_NUMBER, VALUE_STRING, 
        VALUE_CHAR, VALUE_LIST, VALUE_FILE, VALUE_FUNCTION
    } type;
    int list_size;  // For lists
} Value;

struct Expr {
    ExprType type;
    union {
        struct {
            Token* name;
            Expr* value;
        } assign;
        struct {
            Expr* left;
            Token* operator;
            Expr* right;
        } binary;
        struct {
            Expr* callee;
            Token* paren;
            Expr** arguments;
            int arg_count;
        } call;
        struct {
            Expr* expression;
        } grouping;
        struct {
            Value value;
        } literal;
        struct {
            Expr** elements;
            int element_count;
        } list_literal;
        struct {
            Expr* object;
            Expr* index;
        } get;
        struct {
            Expr* object;
            Expr* index;
            Expr* value;
        } set;
        struct {
            Expr* left;
            Token* operator;
            Expr* right;
        } logical;
        struct {
            Token* operator;
            Expr* right;
        } unary;
        struct {
            Token* name;
        } variable;
    };
};

struct Stmt {
    StmtType type;
    union {
        struct {
            Stmt** statements;
            int stmt_count;
        } block;
        struct {
            Expr* expression;
        } expression;
        struct {
            Expr** expressions;
            int expr_count;
        } print;
        struct {
            Token* name;
            Token** params;
            int param_count;
            Stmt** body;
            int body_count;
        } function;
        struct {
            Expr* condition;
            Stmt* then_branch;
            Stmt* else_branch;
        } if_stmt;
        struct {
            Token* keyword;
            Expr* value;
        } return_stmt;
        struct {
            Expr* condition;
            Stmt* body;
        } while_stmt;
        struct {
            Stmt* initializer;
            Expr* condition;
            Expr* increment;
            Stmt* body;
        } for_stmt;
        struct {
            Token* path_token;
            Token* namespace;
        } import;
        struct {
            Token* keyword;
            Expr* condition;
            Expr* message;
        } assert_stmt;
        struct {
            Token* name;
            Expr* initializer;
        } var;
    };
};

// =============================================================================
// 3. PARSER
// =============================================================================

typedef struct {
    Token* tokens;
    int token_count;
    char* filename;
    int current;
} Parser;

// =============================================================================
// 4. BUILT-IN FUNCTIONS & INTERPRETER
// =============================================================================

typedef struct Environment Environment;

struct Environment {
    char** names;
    Value* values;
    int count;
    int capacity;
    Environment* enclosing;
};

typedef struct {
    int arity;
    Value (*call)(struct Interpreter* interpreter, Value* arguments);
    char* name;
} BuiltinFunction;

typedef struct {
    Token* name;
    Token** params;
    int param_count;
    Stmt** body;
    int body_count;
    Environment* closure;
} UserFunction;

typedef struct Interpreter {
    Environment* globals;
    Environment* environment;
    char* filename;
    BuiltinFunction* builtins;
    int builtin_count;
    Value return_value;
    bool has_returned;
    int current_line;
} Interpreter;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

void error_exit(const char* message, ...) {
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
    exit(1);
}

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        printf("Error: File not found at %s\n", path);
        return NULL;
    }
    
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    
    char* buffer = malloc(file_size + 1);
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

Lexer* create_lexer(char* source, char* filename) {
    Lexer* lexer = malloc(sizeof(Lexer));
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

bool is_at_end(Lexer* lexer) {
    return lexer->current >= strlen(lexer->source);
}

char advance(Lexer* lexer) {
    return lexer->source[lexer->current++];
}

char peek(Lexer* lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->source[lexer->current];
}

char peek_next(Lexer* lexer) {
    if (lexer->current + 1 >= strlen(lexer->source)) return '\0';
    return lexer->source[lexer->current + 1];
}

bool match(Lexer* lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (lexer->source[lexer->current] != expected) return false;
    lexer->current++;
    return true;
}

void add_token_simple(Lexer* lexer, MSTokenType type) {
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->token_capacity);
    }
    
    Token* token = &lexer->tokens[lexer->token_count++];
    token->type = type;
    int length = lexer->current - lexer->start;
    token->lexeme = malloc(length + 1);
    strncpy(token->lexeme, lexer->source + lexer->start, length);
    token->lexeme[length] = '\0';
    token->line = lexer->line;
}

void add_token_string(Lexer* lexer, MSTokenType type, char* literal) {
    add_token_simple(lexer, type);
    Token* token = &lexer->tokens[lexer->token_count - 1];
    token->literal.string = malloc(strlen(literal) + 1);
    strcpy(token->literal.string, literal);
}

void add_token_number(Lexer* lexer, MSTokenType type, double number) {
    add_token_simple(lexer, type);
    Token* token = &lexer->tokens[lexer->token_count - 1];
    token->literal.number = number;
}

void add_token_char(Lexer* lexer, MSTokenType type, char character) {
    add_token_simple(lexer, type);
    Token* token = &lexer->tokens[lexer->token_count - 1];
    token->literal.character = character;
}

void add_token_bool(Lexer* lexer, MSTokenType type, bool value) {
    add_token_simple(lexer, type);
    Token* token = &lexer->tokens[lexer->token_count - 1];
    token->literal.boolean = value;
}

void scan_string(Lexer* lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') lexer->line++;
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
    char* value = malloc(length + 1);
    strncpy(value, lexer->source + lexer->start + 1, length);
    value[length] = '\0';
    
    add_token_string(lexer, MS_TOKEN_STRING, value);
    free(value);
}

void scan_number(Lexer* lexer) {
    while (isdigit(peek(lexer))) advance(lexer);
    
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        advance(lexer); // Consume the .
        while (isdigit(peek(lexer))) advance(lexer);
    }
    
    int length = lexer->current - lexer->start;
    char* text = malloc(length + 1);
    strncpy(text, lexer->source + lexer->start, length);
    text[length] = '\0';
    
    double value = atof(text);
    add_token_number(lexer, MS_TOKEN_NUMBER, value);
    free(text);
}

MSTokenType get_keyword_type(char* text) {
    for (int i = 0; keywords[i].text != NULL; i++) {
        if (strcmp(text, keywords[i].text) == 0) {
            return keywords[i].type;
        }
    }
    return MS_TOKEN_IDENTIFIER;
}

void scan_identifier(Lexer* lexer) {
    while (isalnum(peek(lexer)) || peek(lexer) == '_') advance(lexer);
    
    int length = lexer->current - lexer->start;
    char* text = malloc(length + 1);
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

void scan_token(Lexer* lexer) {
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
            add_token_simple(lexer, match(lexer, '=') ? MS_TOKEN_NOT_EQUAL : MS_TOKEN_NOT);
            break;
        case '=':
            add_token_simple(lexer, match(lexer, '=') ? MS_TOKEN_EQUAL : MS_TOKEN_ASSIGN);
            break;
        case '<':
            add_token_simple(lexer, match(lexer, '=') ? MS_TOKEN_LESS_EQUAL : MS_TOKEN_LESS);
            break;
        case '>':
            add_token_simple(lexer, match(lexer, '=') ? MS_TOKEN_GREATER_EQUAL : MS_TOKEN_GREATER);
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
                while (peek(lexer) != '\n' && !is_at_end(lexer)) advance(lexer);
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
        case '\'':
            {
                char char_val = advance(lexer);
                if (advance(lexer) != '\'') {
                    printf("Lexer Error in %s at line %d: Unterminated character literal.\n", 
                           lexer->filename, lexer->line);
                } else {
                    add_token_char(lexer, MS_TOKEN_CHAR, char_val);
                }
            }
            break;
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

Token* scan_tokens(Lexer* lexer) {
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

Parser* create_parser(Token* tokens, int token_count, char* filename) {
    Parser* parser = malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->filename = filename;
    parser->current = 0;
    return parser;
}

bool parser_is_at_end(Parser* parser) {
    return parser->tokens[parser->current].type == MS_TOKEN_EOF;
}

Token* parser_peek(Parser* parser) {
    return &parser->tokens[parser->current];
}

Token* parser_previous(Parser* parser) {
    return &parser->tokens[parser->current - 1];
}

Token* parser_advance(Parser* parser) {
    if (!parser_is_at_end(parser)) parser->current++;
    return parser_previous(parser);
}

bool parser_check(Parser* parser, MSTokenType type) {
    if (parser_is_at_end(parser)) return false;
    return parser_peek(parser)->type == type;
}

bool parser_match(Parser* parser, int count, ...) {
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
Expr* expression(Parser* parser);
Stmt* statement(Parser* parser);
Value evaluate_expr(struct Interpreter* interpreter, Expr* expr);
Expr* finish_call(Parser* parser, Expr* callee);
void execute_stmt(struct Interpreter* interpreter, Stmt* stmt);
void runtime_error(struct Interpreter* interpreter, const char* format, ...);

void parse_error(Parser* parser, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    printf("Parse Error");
    if (parser && parser->filename && parser->current >= 0 && parser->current < parser->token_count) {
        printf(" at line %d in %s", parser->tokens[parser->current].line, parser->filename);
    }
    printf(": ");
    vprintf(format, args);
    printf("\n");
    
    va_end(args);
    exit(1);
}

Expr* create_literal_expr(Value value) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_LITERAL;
    expr->literal.value = value;
    return expr;
}

Expr* create_variable_expr(Token* name) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_VARIABLE;
    expr->variable.name = name;
    return expr;
}

Expr* create_assign_expr(Token* name, Expr* value) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_ASSIGN;
    expr->assign.name = name;
    expr->assign.value = value;
    return expr;
}

Expr* create_binary_expr(Expr* left, Token* operator, Expr* right) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->binary.left = left;
    expr->binary.operator = operator;
    expr->binary.right = right;
    return expr;
}

Expr* primary(Parser* parser) {
    if (parser_match(parser, 4, MS_TOKEN_NUMBER, MS_TOKEN_FALSE, MS_TOKEN_TRUE, MS_TOKEN_NIL)) {
        Token* token = parser_previous(parser);
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
        Token* token = parser_previous(parser);
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
        Expr* expr = expression(parser);
        if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
            parse_error(parser, "Expect ')' after expression");
        }
        
        Expr* grouping = malloc(sizeof(Expr));
        grouping->type = EXPR_GROUPING;
        grouping->grouping.expression = expr;
        return grouping;
    }
    
    if (parser_match(parser, 1, MS_TOKEN_LBRACKET)) {
        // Parse list literal
        Expr** elements = malloc(sizeof(Expr*) * 256);  // Max 256 elements for now
        int element_count = 0;
        
        if (!parser_check(parser, MS_TOKEN_RBRACKET)) {
            do {
                elements[element_count++] = expression(parser);
            } while (parser_match(parser, 1, MS_TOKEN_COMMA));
        }
        
        if (!parser_match(parser, 1, MS_TOKEN_RBRACKET)) {
            parse_error(parser, "Expect ']' after list elements");
        }
        
        Expr* list_literal = malloc(sizeof(Expr));
        list_literal->type = EXPR_LIST_LITERAL;
        list_literal->list_literal.elements = elements;
        list_literal->list_literal.element_count = element_count;
        return list_literal;
    }
    
    parse_error(parser, "Expect expression");
    // This line will never be reached, but the compiler doesn't know that
    return NULL;
}

Expr* call(Parser* parser) {
    Expr* expr = primary(parser);
    
    while (true) {
        if (parser_match(parser, 1, MS_TOKEN_LPAREN)) {
            expr = finish_call(parser, expr);
        } else if (parser_match(parser, 1, MS_TOKEN_LBRACKET)) {
            Expr* index = expression(parser);
                if (!parser_match(parser, 1, MS_TOKEN_RBRACKET)) {
                    parse_error(parser, "Expect ']' after index");
                }            Expr* get_expr = malloc(sizeof(Expr));
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

Expr* finish_call(Parser* parser, Expr* callee) {
    Expr** arguments = malloc(sizeof(Expr*) * 10);
    int arg_count = 0;
    
    if (!parser_check(parser, MS_TOKEN_RPAREN)) {
        do {
            arguments[arg_count++] = expression(parser);
        } while (parser_match(parser, 1, MS_TOKEN_COMMA));
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
        parse_error(parser, "Expect ')' after arguments");
    }
    
    Expr* call_expr = malloc(sizeof(Expr));
    call_expr->type = EXPR_CALL;
    call_expr->call.callee = callee;
    call_expr->call.arguments = arguments;
    call_expr->call.arg_count = arg_count;
    return call_expr;
}

Expr* unary(Parser* parser) {
    if (parser_match(parser, 2, MS_TOKEN_NOT, MS_TOKEN_MINUS)) {
        Token* operator = parser_previous(parser);
        Expr* right = unary(parser);
        
        Expr* expr = malloc(sizeof(Expr));
        expr->type = EXPR_UNARY;
        expr->unary.operator = operator;
        expr->unary.right = right;
        return expr;
    }
    
    return call(parser);
}

Expr* factor(Parser* parser) {
    Expr* expr = unary(parser);
    
    while (parser_match(parser, 2, MS_TOKEN_DIVIDE, MS_TOKEN_MULTIPLY)) {
        Token* operator = parser_previous(parser);
        Expr* right = unary(parser);
        expr = create_binary_expr(expr, operator, right);
    }
    
    return expr;
}

Expr* term(Parser* parser) {
    Expr* expr = factor(parser);
    
    while (parser_match(parser, 2, MS_TOKEN_MINUS, MS_TOKEN_PLUS)) {
        Token* operator = parser_previous(parser);
        Expr* right = factor(parser);
        expr = create_binary_expr(expr, operator, right);
    }
    
    return expr;
}

Expr* comparison(Parser* parser) {
    Expr* expr = term(parser);
    
    while (parser_match(parser, 4, MS_TOKEN_GREATER, MS_TOKEN_GREATER_EQUAL, MS_TOKEN_LESS, MS_TOKEN_LESS_EQUAL)) {
        Token* operator = parser_previous(parser);
        Expr* right = term(parser);
        expr = create_binary_expr(expr, operator, right);
    }
    
    return expr;
}

Expr* equality(Parser* parser) {
    Expr* expr = comparison(parser);
    
    while (parser_match(parser, 2, MS_TOKEN_NOT_EQUAL, MS_TOKEN_EQUAL)) {
        Token* operator = parser_previous(parser);
        Expr* right = comparison(parser);
        expr = create_binary_expr(expr, operator, right);
    }
    
    return expr;
}

Expr* logical_and(Parser* parser) {
    Expr* expr = equality(parser);
    
    while (parser_match(parser, 1, MS_TOKEN_AND)) {
        Token* operator = parser_previous(parser);
        Expr* right = equality(parser);
        
        Expr* logical = malloc(sizeof(Expr));
        logical->type = EXPR_LOGICAL;
        logical->logical.left = expr;
        logical->logical.operator = operator;
        logical->logical.right = right;
        expr = logical;
    }
    
    return expr;
}

Expr* logical_or(Parser* parser) {
    Expr* expr = logical_and(parser);
    
    while (parser_match(parser, 1, MS_TOKEN_OR)) {
        Token* operator = parser_previous(parser);
        Expr* right = logical_and(parser);
        
        Expr* logical = malloc(sizeof(Expr));
        logical->type = EXPR_LOGICAL;
        logical->logical.left = expr;
        logical->logical.operator = operator;
        logical->logical.right = right;
        expr = logical;
    }
    
    return expr;
}

Expr* assignment(Parser* parser) {
    Expr* expr = logical_or(parser);
    
    if (parser_match(parser, 1, MS_TOKEN_ASSIGN)) {
        Expr* value = assignment(parser);
        
        if (expr->type == EXPR_VARIABLE) {
            Token* name = expr->variable.name;
            return create_assign_expr(name, value);
        }
        
        parse_error(parser, "Invalid assignment target");
    }
    
    return expr;
}

Expr* expression(Parser* parser) {
    return assignment(parser);
}

Stmt* expression_statement(Parser* parser) {
    Expr* expr = expression(parser);
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        parse_error(parser, "Expect ';' after expression");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_EXPRESSION;
    stmt->expression.expression = expr;
    return stmt;
}

Stmt* print_statement(Parser* parser) {
    Expr** expressions = malloc(sizeof(Expr*) * 10);
    int expr_count = 0;
    
    expressions[expr_count++] = expression(parser);
    
    while (parser_match(parser, 1, MS_TOKEN_COMMA)) {
        expressions[expr_count++] = expression(parser);
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        parse_error(parser, "Expect ';' after value");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_PRINT;
    stmt->print.expressions = expressions;
    stmt->print.expr_count = expr_count;
    return stmt;
}

Stmt* var_statement(Parser* parser) {
    Token* name = NULL;
    if (!parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
        parse_error(parser, "Expect variable name");
    }
    name = parser_previous(parser);
    
    Expr* initializer = NULL;
    if (parser_match(parser, 1, MS_TOKEN_ASSIGN)) {
        initializer = expression(parser);
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        parse_error(parser, "Expect ';' after variable declaration");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_VAR;
    stmt->var.name = name;
    stmt->var.initializer = initializer;
    return stmt;
}

Stmt* assert_statement(Parser* parser) {
    Expr* condition = expression(parser);
    
    if (!parser_match(parser, 1, MS_TOKEN_COMMA)) {
        parse_error(parser, "Expect ',' after assert condition");
    }
    
    Expr* message = expression(parser);
    
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        parse_error(parser, "Expect ';' after assert message");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_ASSERT;
    stmt->assert_stmt.condition = condition;
    stmt->assert_stmt.message = message;
    return stmt;
}

Stmt* if_statement(Parser* parser) {
    if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
        parse_error(parser, "Expect '(' after 'if'");
    }
    
    Expr* condition = expression(parser);
    
    if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
        parse_error(parser, "Expect ')' after if condition");
    }
    
    Stmt* then_branch = statement(parser);
    Stmt* else_branch = NULL;
    
    if (parser_match(parser, 1, MS_TOKEN_ELSE)) {
        else_branch = statement(parser);
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_IF;
    stmt->if_stmt.condition = condition;
    stmt->if_stmt.then_branch = then_branch;
    stmt->if_stmt.else_branch = else_branch;
    return stmt;
}

Stmt* while_statement(Parser* parser) {
    if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
        parse_error(parser, "Expect '(' after 'while'");
    }
    Expr* condition = expression(parser);
    if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
        parse_error(parser, "Expect ')' after while condition");
    }
    Stmt* body = statement(parser);
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_WHILE;
    stmt->while_stmt.condition = condition;
    stmt->while_stmt.body = body;
    return stmt;
}

Stmt* for_statement(Parser* parser) {
    if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
        parse_error(parser, "Expect '(' after 'for'");
    }
    
    // Initializer (can be var declaration or expression statement)
    Stmt* initializer = NULL;
    if (parser_match(parser, 1, MS_TOKEN_VAR)) {
        initializer = var_statement(parser);
    } else {
        initializer = expression_statement(parser);
    }
    
    // Condition
    Expr* condition = expression(parser);
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        parse_error(parser, "Expect ';' after for loop condition");
    }
    
    // Increment
    Expr* increment = expression(parser);
    if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
        parse_error(parser, "Expect ')' after for clauses");
    }
    
    // Body
    Stmt* body = statement(parser);
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_FOR;
    stmt->for_stmt.initializer = initializer;
    stmt->for_stmt.condition = condition;
    stmt->for_stmt.increment = increment;
    stmt->for_stmt.body = body;
    return stmt;
}

Stmt* function_statement(Parser* parser) {
    if (!parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
        parse_error(parser, "Expect function name");
    }
    Token* name = parser_previous(parser);
    
    if (!parser_match(parser, 1, MS_TOKEN_LPAREN)) {
        parse_error(parser, "Expect '(' after function name");
    }
    
    // Parse parameters
    Token** params = malloc(sizeof(Token*) * 10);  // Max 10 params for now
    int param_count = 0;
    
    if (!parser_check(parser, MS_TOKEN_RPAREN)) {
        do {
            if (!parser_match(parser, 1, MS_TOKEN_IDENTIFIER)) {
                parse_error(parser, "Expect parameter name");
            }
            params[param_count++] = parser_previous(parser);
        } while (parser_match(parser, 1, MS_TOKEN_COMMA));
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_RPAREN)) {
        parse_error(parser, "Expect ')' after parameters");
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_LBRACE)) {
        parse_error(parser, "Expect '{' before function body");
    }
    
    // Parse function body
    Stmt** body = malloc(sizeof(Stmt*) * 50);  // Max 50 statements for now
    int body_count = 0;
    
    while (!parser_check(parser, MS_TOKEN_RBRACE) && !parser_is_at_end(parser)) {
        body[body_count++] = statement(parser);
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_RBRACE)) {
        parse_error(parser, "Expect '}' after function body");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_FUNCTION;
    stmt->function.name = name;
    stmt->function.params = params;
    stmt->function.param_count = param_count;
    stmt->function.body = body;
    stmt->function.body_count = body_count;
    return stmt;
}

Stmt* return_statement(Parser* parser) {
    Token* keyword = parser_previous(parser);
    
    Expr* value = NULL;
    if (!parser_check(parser, MS_TOKEN_SEMICOLON) && !parser_check(parser, MS_TOKEN_RBRACE)) {
        value = expression(parser);
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        // Semicolon is optional before }
        if (!parser_check(parser, MS_TOKEN_RBRACE)) {
            parse_error(parser, "Expect ';' after return value");
        }
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_RETURN;
    stmt->return_stmt.keyword = keyword;
    stmt->return_stmt.value = value;
    return stmt;
}

Stmt* block_statement(Parser* parser) {
    Stmt** statements = malloc(sizeof(Stmt*) * 100);
    int stmt_count = 0;
    
    while (!parser_check(parser, MS_TOKEN_RBRACE) && !parser_is_at_end(parser)) {
        statements[stmt_count++] = statement(parser);
    }
    
    if (!parser_match(parser, 1, MS_TOKEN_RBRACE)) {
        parse_error(parser, "Expect '}' after block");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_BLOCK;
    stmt->block.statements = statements;
    stmt->block.stmt_count = stmt_count;
    return stmt;
}

Stmt* import_statement(Parser* parser) {
    if (!parser_check(parser, MS_TOKEN_STRING)) {
        parse_error(parser, "Expect string literal for import path");
    }
    
    Token* path_token = parser_advance(parser);
    
    if (!parser_match(parser, 1, MS_TOKEN_SEMICOLON)) {
        parse_error(parser, "Expect ';' after import statement");
    }
    
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_IMPORT;
    stmt->import.path_token = path_token;
    stmt->import.namespace = NULL; // For basic import, no namespace
    return stmt;
}

Stmt* statement(Parser* parser) {
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

Stmt** parse(Parser* parser) {
    int capacity = 100;
    Stmt** statements = malloc(sizeof(Stmt*) * capacity);
    int stmt_count = 0;
    
    while (!parser_is_at_end(parser)) {
        // Check if we need to grow the array
        if (stmt_count >= capacity - 1) {  // -1 to leave room for NULL terminator
            capacity *= 2;
            statements = realloc(statements, sizeof(Stmt*) * capacity);
            if (!statements) {
                fprintf(stderr, "Error: Out of memory while parsing\n");
                exit(1);
            }
        }
        statements[stmt_count++] = statement(parser);
    }
    
    statements[stmt_count] = NULL; // Null terminator
    return statements;
}

// =============================================================================
// INTERPRETER IMPLEMENTATION
// =============================================================================

Environment* create_environment(Environment* enclosing) {
    Environment* env = malloc(sizeof(Environment));
    env->names = malloc(sizeof(char*) * 100);
    env->values = malloc(sizeof(Value) * 100);
    env->count = 0;
    env->capacity = 100;
    env->enclosing = enclosing;
    return env;
}

void env_define(Environment* env, char* name, Value value) {
    if (env->count >= env->capacity) {
        env->capacity *= 2;
        env->names = realloc(env->names, sizeof(char*) * env->capacity);
        env->values = realloc(env->values, sizeof(Value) * env->capacity);
    }
    
    env->names[env->count] = malloc(strlen(name) + 1);
    strcpy(env->names[env->count], name);
    env->values[env->count] = value;
    env->count++;
}

void env_assign(Environment* env, char* name, Value value) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) {
            env->values[i] = value;
            return;
        }
    }
    
    if (env->enclosing != NULL) {
        Environment* current = env;
        while (current != NULL) {
            for (int i = 0; i < current->count; i++) {
                if (strcmp(current->names[i], name) == 0) {
                    current->values[i] = value;
                    return;
                }
            }
            if (current->enclosing == NULL) break;
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
Value env_get(Environment* env, char* name) {
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

Value env_get_with_error(Interpreter* interpreter, Environment* env, char* name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) {
            return env->values[i];
        }
    }
    
    if (env->enclosing != NULL) {
        return env_get_with_error(interpreter, env->enclosing, name);
    }
    
    runtime_error(interpreter, "Undefined variable '%s'", name);
    // This line will never be reached, but the compiler doesn't know that
    Value dummy = {0};
    return dummy;
}

char* value_to_string(Value value) {
    static char buffer[256];
    
    switch (value.type) {
        case VALUE_NIL:
            strcpy(buffer, "nil");
            break;
        case VALUE_BOOL:
            strcpy(buffer, value.value.boolean ? "true" : "false");
            break;
        case VALUE_NUMBER:
            {
                double num = value.value.number;
                if (num == (int)num) {
                    sprintf(buffer, "%d", (int)num);
                } else {
                    sprintf(buffer, "%g", num);
                }
            }
            break;
        case VALUE_STRING:
            return value.value.string;
        case VALUE_CHAR:
            sprintf(buffer, "%c", value.value.character);
            break;
        case VALUE_FILE:
            sprintf(buffer, "<file handle>");
            break;
        default:
            strcpy(buffer, "unknown");
            break;
    }
    
    return buffer;
}

char* value_to_string_alloc(Value value) {
    switch (value.type) {
        case VALUE_NIL:
            {
                char* result = malloc(4);
                strcpy(result, "nil");
                return result;
            }
        case VALUE_BOOL:
            {
                char* result = malloc(6);
                strcpy(result, value.value.boolean ? "true" : "false");
                return result;
            }
        case VALUE_NUMBER:
            {
                char* result = malloc(32);
                double num = value.value.number;
                if (num == (int)num) {
                    sprintf(result, "%d", (int)num);
                } else {
                    sprintf(result, "%g", num);
                }
                return result;
            }
        case VALUE_STRING:
            {
                char* result = malloc(strlen(value.value.string) + 1);
                strcpy(result, value.value.string);
                return result;
            }
        case VALUE_CHAR:
            {
                char* result = malloc(2);
                sprintf(result, "%c", value.value.character);
                return result;
            }
        case VALUE_FILE:
            {
                char* result = malloc(15);
                strcpy(result, "<file handle>");
                return result;
            }
        default:
            {
                char* result = malloc(8);
                strcpy(result, "unknown");
                return result;
            }
    }
}

void runtime_error(Interpreter* interpreter, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    printf("Runtime Error");
    if (interpreter && interpreter->filename && interpreter->current_line > 0) {
        printf(" at line %d in %s", interpreter->current_line, interpreter->filename);
    }
    printf(": ");
    vprintf(format, args);
    printf("\n");
    
    va_end(args);
    exit(1);
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

Value evaluate_literal(Interpreter* interpreter, Expr* expr) {
    return expr->literal.value;
}

Value evaluate_variable(Interpreter* interpreter, Expr* expr) {
    interpreter->current_line = expr->variable.name->line;
    return env_get_with_error(interpreter, interpreter->environment, expr->variable.name->lexeme);
}

Value evaluate_binary(Interpreter* interpreter, Expr* expr) {
    interpreter->current_line = expr->binary.operator->line;
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
            char* left_str = value_to_string_alloc(left);
            char* right_str = value_to_string_alloc(right);
            result.value.string = malloc(strlen(left_str) + strlen(right_str) + 1);
            strcpy(result.value.string, left_str);
            strcat(result.value.string, right_str);
            
            if (left.type != VALUE_STRING) free(left_str);
            if (right.type != VALUE_STRING) free(right_str);
        }
    } else if (op == MS_TOKEN_MINUS) {
        result.value.number = left.value.number - right.value.number;
    } else if (op == MS_TOKEN_MULTIPLY) {
        result.value.number = left.value.number * right.value.number;
    } else if (op == MS_TOKEN_DIVIDE) {
        if (right.value.number == 0) {
            runtime_error(interpreter, "Division by zero");
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
        if (left.type != right.type) {
            result.value.boolean = false;
        } else if (left.type == VALUE_NIL) {
            result.value.boolean = true;  // nil == nil is always true
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
        if (left.type != right.type) {
            result.value.boolean = true;
        } else if (left.type == VALUE_NIL) {
            result.value.boolean = false;  // nil != nil is always false
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

Value evaluate_unary(Interpreter* interpreter, Expr* expr) {
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

Value evaluate_assign(Interpreter* interpreter, Expr* expr) {
    Value value = evaluate_expr(interpreter, expr->assign.value);
    env_assign(interpreter->environment, expr->assign.name->lexeme, value);
    return value;
}

Value evaluate_call(Interpreter* interpreter, Expr* expr) {
    // Use callee line number for error reporting since paren might not be set
    if (expr->call.callee->type == EXPR_VARIABLE) {
        interpreter->current_line = expr->call.callee->variable.name->line;
    }
    
    // For now, only handle built-in functions by name
    if (expr->call.callee->type == EXPR_VARIABLE) {
        char* func_name = expr->call.callee->variable.name->lexeme;
        
        if (strcmp(func_name, "len") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "len() expects exactly 1 argument");
            }
            
            Value arg = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value result;
            result.type = VALUE_NUMBER;
            
            if (arg.type == VALUE_STRING) {
                result.value.number = strlen(arg.value.string);
            } else if (arg.type == VALUE_LIST) {
                result.value.number = arg.list_size;
            } else {
                runtime_error(interpreter, "len() expects a string or list argument");
            }
            
            return result;
        } else if (strcmp(func_name, "fopen") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "fopen() expects exactly 2 arguments");
            }
            
            Value filename_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value mode_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (filename_val.type != VALUE_STRING || mode_val.type != VALUE_STRING) {
                runtime_error(interpreter, "fopen() expects string arguments");
            }
            
            FILE* handle = fopen(filename_val.value.string, mode_val.value.string);
            Value result;
            if (handle == NULL) {
                result.type = VALUE_NIL;
            } else {
                result.type = VALUE_FILE;
                result.value.file_handle = handle;
            }
            
            return result;
        } else if (strcmp(func_name, "fclose") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "fclose() expects exactly 1 argument");
            }
            
            Value handle_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (handle_val.type != VALUE_FILE) {
                runtime_error(interpreter, "fclose() expects a file handle");
            }
            
            int close_result = fclose(handle_val.value.file_handle);
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)close_result;
            return result;
        } else if (strcmp(func_name, "fwrite") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "fwrite() expects exactly 2 arguments");
            }
            
            Value handle_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value content_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (handle_val.type != VALUE_FILE) {
                runtime_error(interpreter, "fwrite() expects a file handle as first argument");
            }
            
            if (content_val.type != VALUE_STRING) {
                runtime_error(interpreter, "fwrite() expects a string as second argument");
            }
            
            size_t bytes_written = fprintf(handle_val.value.file_handle, "%s", content_val.value.string);
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)bytes_written;
            return result;
        } else if (strcmp(func_name, "fread") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "fread() expects exactly 1 argument");
            }
            
            Value handle_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (handle_val.type != VALUE_FILE) {
                runtime_error(interpreter, "fread() expects a file handle");
            }
            
            // Read entire file content
            fseek(handle_val.value.file_handle, 0, SEEK_END);
            long length = ftell(handle_val.value.file_handle);
            fseek(handle_val.value.file_handle, 0, SEEK_SET);
            
            char* buffer = malloc(length + 1);
            fread(buffer, 1, length, handle_val.value.file_handle);
            buffer[length] = '\0';
            
            Value result;
            result.type = VALUE_STRING;
            result.value.string = buffer;
            return result;
        } else if (strcmp(func_name, "freadline") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "freadline() expects exactly 1 argument");
            }
            
            Value handle_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (handle_val.type != VALUE_FILE) {
                runtime_error(interpreter, "freadline() expects a file handle");
            }
            
            char buffer[1024];
            if (fgets(buffer, sizeof(buffer), handle_val.value.file_handle) != NULL) {
                // Remove trailing newline characters (\r\n or \n)
                size_t len = strlen(buffer);
                while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
                    buffer[len-1] = '\0';
                    len--;
                }
                
                Value result;
                result.type = VALUE_STRING;
                result.value.string = strdup(buffer);
                return result;
            } else {
                Value result;
                result.type = VALUE_NIL;
                return result;
            }
        } else if (strcmp(func_name, "fwriteline") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "fwriteline() expects exactly 2 arguments");
            }
            
            Value handle_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value line_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (handle_val.type != VALUE_FILE) {
                runtime_error(interpreter, "fwriteline() expects a file handle as first argument");
            }
            
            if (line_val.type != VALUE_STRING) {
                runtime_error(interpreter, "fwriteline() expects a string as second argument");
            }
            
            fprintf(handle_val.value.file_handle, "%s\n", line_val.value.string);
            Value result;
            result.type = VALUE_NIL;
            return result;
        } else if (strcmp(func_name, "fexists") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "fexists() expects exactly 1 argument");
            }
            
            Value filename_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (filename_val.type != VALUE_STRING) {
                runtime_error(interpreter, "fexists() expects a string argument");
            }
            
            FILE* test_file = fopen(filename_val.value.string, "r");
            Value result;
            result.type = VALUE_BOOL;
            
            if (test_file != NULL) {
                fclose(test_file);
                result.value.boolean = true;
            } else {
                result.value.boolean = false;
            }
            
            return result;
        } else if (strcmp(func_name, "time_now") == 0) {
            if (expr->call.arg_count != 0) {
                runtime_error(interpreter, "time_now() expects no arguments");
            }
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)time(NULL);
            return result;
        } else if (strcmp(func_name, "time_format") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "time_format() expects exactly 2 arguments");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value format_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (timestamp_val.type != VALUE_NUMBER || format_val.type != VALUE_STRING) {
                runtime_error(interpreter, "time_format() expects number and string arguments");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            char buffer[256];
            strftime(buffer, sizeof(buffer), format_val.value.string, tm_info);
            
            Value result;
            result.type = VALUE_STRING;
            result.value.string = strdup(buffer);
            return result;
        } else if (strcmp(func_name, "time_parse") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "time_parse() expects exactly 2 arguments");
            }
            
            Value datestr_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value format_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (datestr_val.type != VALUE_STRING || format_val.type != VALUE_STRING) {
                runtime_error(interpreter, "time_parse() expects string arguments");
            }
            
            struct tm tm_info = {0};
            if (strptime(datestr_val.value.string, format_val.value.string, &tm_info) == NULL) {
                Value result;
                result.type = VALUE_NIL;
                return result;
            }
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)timegm(&tm_info);
            return result;
        } else if (strcmp(func_name, "time_year") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_year() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_year() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)(tm_info->tm_year + 1900);
            return result;
        } else if (strcmp(func_name, "time_month") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_month() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_month() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)(tm_info->tm_mon + 1);
            return result;
        } else if (strcmp(func_name, "time_day") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_day() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_day() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)tm_info->tm_mday;
            return result;
        } else if (strcmp(func_name, "time_hour") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_hour() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_hour() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)tm_info->tm_hour;
            return result;
        } else if (strcmp(func_name, "time_minute") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_minute() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_minute() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)tm_info->tm_min;
            return result;
        } else if (strcmp(func_name, "time_second") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_second() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_second() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = (double)tm_info->tm_sec;
            return result;
        } else if (strcmp(func_name, "time_weekday") == 0) {
            if (expr->call.arg_count != 1) {
                runtime_error(interpreter, "time_weekday() expects exactly 1 argument");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            if (timestamp_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_weekday() expects a number argument");
            }
            
            time_t timestamp = (time_t)timestamp_val.value.number;
            struct tm *tm_info = gmtime(&timestamp);
            
            Value result;
            result.type = VALUE_NUMBER;
            // Convert from Sunday=0 to Monday=0 format
            result.value.number = (double)((tm_info->tm_wday + 6) % 7);
            return result;
        } else if (strcmp(func_name, "time_add") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "time_add() expects exactly 2 arguments");
            }
            
            Value timestamp_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value seconds_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (timestamp_val.type != VALUE_NUMBER || seconds_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_add() expects number arguments");
            }
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = timestamp_val.value.number + seconds_val.value.number;
            return result;
        } else if (strcmp(func_name, "time_diff") == 0) {
            if (expr->call.arg_count != 2) {
                runtime_error(interpreter, "time_diff() expects exactly 2 arguments");
            }
            
            Value time1_val = evaluate_expr(interpreter, expr->call.arguments[0]);
            Value time2_val = evaluate_expr(interpreter, expr->call.arguments[1]);
            
            if (time1_val.type != VALUE_NUMBER || time2_val.type != VALUE_NUMBER) {
                runtime_error(interpreter, "time_diff() expects number arguments");
            }
            
            Value result;
            result.type = VALUE_NUMBER;
            result.value.number = time1_val.value.number - time2_val.value.number;
            return result;
        } else {
            // Check if it's a user-defined function
            Value func_value = env_get(interpreter->environment, func_name);
            if (func_value.type == VALUE_FUNCTION) {
                // Call user-defined function
                Stmt* func_stmt = func_value.value.function_stmt;
                
                // Check argument count
                if (expr->call.arg_count != func_stmt->function.param_count) {
                    runtime_error(interpreter, "Function '%s' expects %d arguments, got %d",
                           func_name, func_stmt->function.param_count, expr->call.arg_count);
                }
                
                // Create new environment for function scope
                Environment* previous = interpreter->environment;
                Environment* func_env = create_environment(interpreter->environment);
                interpreter->environment = func_env;
                
                // Bind parameters to arguments
                for (int i = 0; i < func_stmt->function.param_count; i++) {
                    Value arg_value = evaluate_expr(interpreter, expr->call.arguments[i]);
                    env_define(func_env, func_stmt->function.params[i]->lexeme, arg_value);
                }
                
                // Execute function body and check for returns
                interpreter->has_returned = false;
                for (int i = 0; i < func_stmt->function.body_count; i++) {
                    execute_stmt(interpreter, func_stmt->function.body[i]);
                    if (interpreter->has_returned) {
                        break;
                    }
                }
                
                // Get return value
                Value result;
                if (interpreter->has_returned) {
                    result = interpreter->return_value;
                } else {
                    result.type = VALUE_NIL;
                }
                
                // Restore previous environment
                interpreter->environment = previous;
                
                return result;
            } else {
                runtime_error(interpreter, "Unknown function '%s'", func_name);
            }
        }
    }
    
    runtime_error(interpreter, "Can only call functions");
    // This line will never be reached, but the compiler doesn't know that
    Value dummy = {0};
    return dummy;
}

Value evaluate_logical(Interpreter* interpreter, Expr* expr) {
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

Value evaluate_grouping(Interpreter* interpreter, Expr* expr) {
    return evaluate_expr(interpreter, expr->grouping.expression);
}

Value evaluate_list_literal(Interpreter* interpreter, Expr* expr) {
    Value list_value;
    list_value.type = VALUE_LIST;
    list_value.list_size = expr->list_literal.element_count;
    list_value.value.elements = malloc(sizeof(Expr*) * list_value.list_size);
    
    // Evaluate each element and store the result
    for (int i = 0; i < list_value.list_size; i++) {
        Value element_value = evaluate_expr(interpreter, expr->list_literal.elements[i]);
        // For now, store as literal expressions - a more complete implementation
        // would need a different storage approach for runtime values
        Expr* literal_expr = malloc(sizeof(Expr));
        literal_expr->type = EXPR_LITERAL;
        literal_expr->literal.value = element_value;
        list_value.value.elements[i] = literal_expr;
    }
    
    return list_value;
}

Value evaluate_get(Interpreter* interpreter, Expr* expr) {
    Value object = evaluate_expr(interpreter, expr->get.object);
    
    if (object.type != VALUE_LIST) {
        runtime_error(interpreter, "Only lists can be indexed");
    }
    
    Value index_value = evaluate_expr(interpreter, expr->get.index);
    if (index_value.type != VALUE_NUMBER) {
        runtime_error(interpreter, "List index must be a number");
    }
    
    int index = (int)index_value.value.number;
    if (index < 0 || index >= object.list_size) {
        runtime_error(interpreter, "List index out of bounds");
    }
    
    // Return the value from the stored literal expression
    return object.value.elements[index]->literal.value;
}

Value evaluate_set(Interpreter* interpreter, Expr* expr) {
    Value object = evaluate_expr(interpreter, expr->set.object);
    
    if (object.type != VALUE_LIST) {
        runtime_error(interpreter, "Only lists can be indexed for assignment");
    }
    
    Value index_value = evaluate_expr(interpreter, expr->set.index);
    if (index_value.type != VALUE_NUMBER) {
        runtime_error(interpreter, "List index must be a number");
    }
    
    int index = (int)index_value.value.number;
    if (index < 0 || index >= object.list_size) {
        runtime_error(interpreter, "List index out of bounds");
    }
    
    Value new_value = evaluate_expr(interpreter, expr->set.value);
    
    // Update the value in the list
    Expr* literal_expr = malloc(sizeof(Expr));
    literal_expr->type = EXPR_LITERAL;
    literal_expr->literal.value = new_value;
    object.value.elements[index] = literal_expr;
    
    return new_value;
}

Value evaluate_expr(Interpreter* interpreter, Expr* expr) {
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
            runtime_error(interpreter, "Unknown expression type");
            // This line will never be reached, but the compiler doesn't know that
            Value dummy = {0};
            return dummy;
    }
}

void execute_expression_stmt(Interpreter* interpreter, Stmt* stmt) {
    evaluate_expr(interpreter, stmt->expression.expression);
}

void execute_print_stmt(Interpreter* interpreter, Stmt* stmt) {
    for (int i = 0; i < stmt->print.expr_count; i++) {
        Value value = evaluate_expr(interpreter, stmt->print.expressions[i]);
        printf("%s", value_to_string(value));
        if (i < stmt->print.expr_count - 1) printf(" ");
    }
    printf("\n");
}

void execute_var_stmt(Interpreter* interpreter, Stmt* stmt) {
    Value value;
    value.type = VALUE_NIL;
    
    if (stmt->var.initializer != NULL) {
        value = evaluate_expr(interpreter, stmt->var.initializer);
    }
    
    env_define(interpreter->environment, stmt->var.name->lexeme, value);
}

void execute_assert_stmt(Interpreter* interpreter, Stmt* stmt) {
    Value condition = evaluate_expr(interpreter, stmt->assert_stmt.condition);
    
    if (!is_truthy(condition)) {
        Value message = evaluate_expr(interpreter, stmt->assert_stmt.message);
        runtime_error(interpreter, "Assertion failed: %s", value_to_string(message));
    }
}

void execute_if_stmt(Interpreter* interpreter, Stmt* stmt) {
    Value condition = evaluate_expr(interpreter, stmt->if_stmt.condition);
    
    if (is_truthy(condition)) {
        execute_stmt(interpreter, stmt->if_stmt.then_branch);
    } else if (stmt->if_stmt.else_branch != NULL) {
        execute_stmt(interpreter, stmt->if_stmt.else_branch);
    }
}

void execute_while_stmt(Interpreter* interpreter, Stmt* stmt) {
    while (true) {
        Value condition = evaluate_expr(interpreter, stmt->while_stmt.condition);
        if (!is_truthy(condition)) break;
        execute_stmt(interpreter, stmt->while_stmt.body);
    }
}

void execute_for_stmt(Interpreter* interpreter, Stmt* stmt) {
    // Create new scope for the for loop
    Environment* previous = interpreter->environment;
    Environment* for_env = create_environment(previous);
    interpreter->environment = for_env;
    
    // Execute initializer
    if (stmt->for_stmt.initializer != NULL) {
        execute_stmt(interpreter, stmt->for_stmt.initializer);
    }
    
    // Loop with condition and increment
    while (true) {
        // Check condition
        Value condition = evaluate_expr(interpreter, stmt->for_stmt.condition);
        if (!is_truthy(condition)) break;
        
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

void execute_function_stmt(Interpreter* interpreter, Stmt* stmt) {
    // Store function definition in environment as a special VALUE_FUNCTION
    Value func_value;
    func_value.type = VALUE_FUNCTION;
    // For simplicity, we'll store the function statement itself in the value
    // This is a hack for a basic implementation
    func_value.value.function_stmt = stmt;
    
    env_define(interpreter->environment, stmt->function.name->lexeme, func_value);
}

void execute_import_stmt(Interpreter* interpreter, Stmt* stmt) {
    // Get the file path from the string token
    char* base_path = stmt->import.path_token->literal.string;
    
    // Create the full path with .ms extension if not present
    char* file_path;
    if (strstr(base_path, ".ms") == NULL) {
        file_path = malloc(strlen(base_path) + 4); // +3 for ".ms" +1 for null terminator
        strcpy(file_path, base_path);
        strcat(file_path, ".ms");
    } else {
        file_path = malloc(strlen(base_path) + 1);
        strcpy(file_path, base_path);
    }
    
    // Check if file exists
    if (access(file_path, F_OK) != 0) {
        runtime_error(interpreter, "Module file '%s' not found", file_path);
    }
    
    // Read the file content
    FILE* file = fopen(file_path, "r");
    if (!file) {
        runtime_error(interpreter, "Failed to open module file '%s'", file_path);
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    
    // Allocate buffer and read file
    char* content = malloc(file_size + 1);
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    // Create lexer and parser for the imported file
    Lexer* lexer = create_lexer(content, file_path);
    scan_tokens(lexer);
    
    Parser* parser = create_parser(lexer->tokens, lexer->token_count, file_path);
    Stmt** statements = parse(parser);
    
    // Execute the imported file in the current environment
    // This will add all variables and functions to the current scope
    for (int i = 0; statements[i] != NULL; i++) {
        execute_stmt(interpreter, statements[i]);
        if (interpreter->has_returned) {
            break; // Handle early returns in imported modules
        }
    }
    
    // Clean up
    free(content);
    free(file_path);
    // Note: We should also free lexer, parser, and statements, but for simplicity we'll skip that
}

void execute_return_stmt(Interpreter* interpreter, Stmt* stmt) {
    if (stmt->return_stmt.value != NULL) {
        interpreter->return_value = evaluate_expr(interpreter, stmt->return_stmt.value);
    } else {
        interpreter->return_value.type = VALUE_NIL;
    }
    interpreter->has_returned = true;
}

void execute_block_stmt(Interpreter* interpreter, Stmt* stmt) {
    Environment* previous = interpreter->environment;
    Environment* block_env = create_environment(previous);
    interpreter->environment = block_env;
    
    for (int i = 0; i < stmt->block.stmt_count; i++) {
        execute_stmt(interpreter, stmt->block.statements[i]);
    }
    
    interpreter->environment = previous;
    // Note: In a production implementation, we'd want to free block_env
}

void execute_stmt(Interpreter* interpreter, Stmt* stmt) {
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
            runtime_error(interpreter, "Unknown statement type");
    }
}

Interpreter* create_interpreter(char* filename) {
    Interpreter* interpreter = malloc(sizeof(Interpreter));
    interpreter->globals = create_environment(NULL);
    interpreter->environment = interpreter->globals;
    interpreter->filename = filename;
    interpreter->builtins = NULL;
    interpreter->builtin_count = 0;
    interpreter->has_returned = false;
    interpreter->return_value.type = VALUE_NIL;
    interpreter->current_line = 0;
    return interpreter;
}

void interpret(Interpreter* interpreter, Stmt** statements) {
    for (int i = 0; statements[i] != NULL; i++) {
        execute_stmt(interpreter, statements[i]);
    }
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

void run(char* source, char* filename) {
    Lexer* lexer = create_lexer(source, filename);
    Token* tokens = scan_tokens(lexer);
    
    Parser* parser = create_parser(tokens, lexer->token_count, filename);
    Stmt** statements = parse(parser);
    
    Interpreter* interpreter = create_interpreter(filename);
    interpret(interpreter, statements);
}

void run_file(char* path) {
    printf("Mini Script Language Interpreter\n");
    printf("=================================\n");
    
    // Create display path
    char display_path[1024];
    strcpy(display_path, path);
    
    // Normalize path separators
    for (char* p = display_path; *p; p++) {
        if (*p == '\\') *p = '/';
    }
    
    // Find tests/ marker for relative display
    char* tests_marker = strstr(display_path, "tests/");
    if (tests_marker != NULL) {
        // Use memmove to handle overlapping memory safely
        memmove(display_path, tests_marker, strlen(tests_marker) + 1);
    }
    
    printf("Executing: %s\n", display_path);
    printf("---------------------------------\n\n");
    
    char* source = read_file(path);
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
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        if (strcmp(line, "exit") == 0) break;
        
        run(line, "<REPL>");
    }
}

int main(int argc, char* argv[]) {
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
