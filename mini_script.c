#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_TOKEN_LEN 256
#define MAX_IDENTIFIER_LEN 64
#define MAX_STRING_LEN 512
#define MAX_VARIABLES 1000
#define MAX_FUNCTIONS 100
#define MAX_PARAMS 10
#define MAX_STACK_DEPTH 100
#define MAX_LIST_SIZE 100
#define MAX_MAP_SIZE 100
#define MAX_DLL_MODULES 50
#define MAX_DLL_FUNCTIONS 200

// Token types
typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_FUNCTION,
    TOKEN_RETURN,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR_TYPE,
    TOKEN_STRING_TYPE,
    TOKEN_LIST,
    TOKEN_MAP,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_LOADLIB,
    TOKEN_GETPROC,
    TOKEN_FREELIB,
    TOKEN_CALLEXT
} ScriptTokenType;

// Value types
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_LIST,
    TYPE_MAP,
    TYPE_BOOL,
    TYPE_DLL_HANDLE,
    TYPE_DLL_FUNCTION
} ValueType;

// Forward declarations
struct Value;
struct MapEntry;

// List structure
typedef struct {
    struct Value* elements;
    int size;
    int capacity;
} List;

// Map entry structure
typedef struct MapEntry {
    char* key;
    struct Value* value;
    struct MapEntry* next;
} MapEntry;

// Map structure
typedef struct {
    MapEntry* buckets[MAX_MAP_SIZE];
    int size;
} Map;

#ifdef _WIN32
// DLL Module structure
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    HMODULE handle;
} DllModule;

// DLL Function structure
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    char module_name[MAX_IDENTIFIER_LEN];
    FARPROC proc_address;
    int param_count;
    ValueType param_types[MAX_PARAMS];
    ValueType return_type;
} DllFunction;
#endif

// Value structure
typedef struct Value {
    ValueType type;
    union {
        int int_val;
        float float_val;
        char char_val;
        char* string_val;
        List* list_val;
        Map* map_val;
        int bool_val;
#ifdef _WIN32
        HMODULE dll_handle;
        FARPROC dll_function;
#endif
    };
} Value;

// Token structure
typedef struct {
    ScriptTokenType type;
    char lexeme[MAX_TOKEN_LEN];
    Value value;
    int line;
} Token;

// Variable structure
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    Value value;
} Variable;

// Function parameter structure
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    ValueType type;
} Parameter;

// Function structure
typedef struct {
    char name[MAX_IDENTIFIER_LEN];
    Parameter params[MAX_PARAMS];
    int param_count;
    char* body;
    ValueType return_type;
} Function;

// AST Node types
typedef enum {
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_ASSIGN,
    NODE_CALL,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_BLOCK,
    NODE_RETURN,
    NODE_LIST_ACCESS,
    NODE_MAP_ACCESS
} NodeType;

// AST Node structure
typedef struct ASTNode {
    NodeType type;
    Value value;
    char identifier[MAX_IDENTIFIER_LEN];
    ScriptTokenType operator;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* condition;
    struct ASTNode* then_branch;
    struct ASTNode* else_branch;
    struct ASTNode* init;
    struct ASTNode* update;
    struct ASTNode* body;
    struct ASTNode** statements;
    int statement_count;
    struct ASTNode** args;
    int arg_count;
} ASTNode;

// Interpreter state
typedef struct {
    char* source;
    int pos;
    int line;
    Token current_token;
    Variable variables[MAX_VARIABLES];
    int var_count;
    Function functions[MAX_FUNCTIONS];
    int func_count;
    Variable stack[MAX_STACK_DEPTH][MAX_VARIABLES];
    int stack_vars[MAX_STACK_DEPTH];
    int stack_depth;
    Value return_value;
    int has_return;
#ifdef _WIN32
    DllModule dll_modules[MAX_DLL_MODULES];
    int dll_module_count;
    DllFunction dll_functions[MAX_DLL_FUNCTIONS];
    int dll_function_count;
#endif
} Interpreter;

// Global interpreter instance
Interpreter interpreter;

// Function prototypes
void error(const char* format, ...);
void init_interpreter(const char* source);
void next_token();
Value* create_value(ValueType type);
void free_value(Value* val);
List* create_list();
void list_append(List* list, Value* val);
Value* list_get(List* list, int index);
Map* create_map();
void map_set(Map* map, const char* key, Value* val);
Value* map_get(Map* map, const char* key);
unsigned int hash(const char* key);
ASTNode* parse_expression();
ASTNode* parse_statement();
ASTNode* parse_block();
ASTNode* parse_function();
Value evaluate(ASTNode* node);
void execute_statement(ASTNode* node);
Variable* find_variable(const char* name);
Function* find_function(const char* name);
void print_value(Value* val);
#ifdef _WIN32
HMODULE load_dll(const char* dll_name);
FARPROC get_dll_function(const char* dll_name, const char* func_name);
void free_dll(const char* dll_name);
Value call_external_function(const char* func_name, ASTNode** args, int arg_count);
DllModule* find_dll_module(const char* name);
DllFunction* find_dll_function(const char* name);
#endif

// Error handling
void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Check if we're very close to the end of the source
    // If so, it might just be trailing garbage, so don't exit
    if (interpreter.pos >= strlen(interpreter.source) - 10) {
        // We're near the end, this might just be trailing characters
        // Set position to end to trigger EOF
        interpreter.pos = strlen(interpreter.source);
        interpreter.current_token.type = TOKEN_EOF;
        va_end(args);
        return;
    }
    
    printf("Error at line %d: ", interpreter.line);
    vprintf(format, args);
    printf("\n");
    va_end(args);
    exit(1);
}

// Initialize interpreter
void init_interpreter(const char* source) {
    interpreter.source = strdup(source);
    interpreter.pos = 0;
    interpreter.line = 1;
    interpreter.var_count = 0;
    interpreter.func_count = 0;
    interpreter.stack_depth = 0;
    interpreter.has_return = 0;
#ifdef _WIN32
    interpreter.dll_module_count = 0;
    interpreter.dll_function_count = 0;
#endif
    next_token();
}

// Skip whitespace and comments
void skip_whitespace() {
    while (interpreter.pos < strlen(interpreter.source)) {
        char c = interpreter.source[interpreter.pos];
        if (c == ' ' || c == '\t' || c == '\r') {
            interpreter.pos++;
        } else if (c == '\n') {
            interpreter.line++;
            interpreter.pos++;
        } else if (c == '/' && interpreter.source[interpreter.pos + 1] == '/') {
            // Skip single line comment
            while (interpreter.pos < strlen(interpreter.source) && 
                   interpreter.source[interpreter.pos] != '\n') {
                interpreter.pos++;
            }
        } else {
            break;
        }
    }
}

// Read next token
void next_token() {
    skip_whitespace();
    
    if (interpreter.pos >= strlen(interpreter.source)) {
        interpreter.current_token.type = TOKEN_EOF;
        return;
    }
    
    char c = interpreter.source[interpreter.pos];
    interpreter.current_token.line = interpreter.line;
    
    // Single character tokens
    switch (c) {
        case '+': interpreter.current_token.type = TOKEN_PLUS; interpreter.pos++; return;
        case '-': interpreter.current_token.type = TOKEN_MINUS; interpreter.pos++; return;
        case '*': interpreter.current_token.type = TOKEN_MULTIPLY; interpreter.pos++; return;
        case '/': interpreter.current_token.type = TOKEN_DIVIDE; interpreter.pos++; return;
        case ';': interpreter.current_token.type = TOKEN_SEMICOLON; interpreter.pos++; return;
        case ',': interpreter.current_token.type = TOKEN_COMMA; interpreter.pos++; return;
        case '(': interpreter.current_token.type = TOKEN_LPAREN; interpreter.pos++; return;
        case ')': interpreter.current_token.type = TOKEN_RPAREN; interpreter.pos++; return;
        case '{': interpreter.current_token.type = TOKEN_LBRACE; interpreter.pos++; return;
        case '}': interpreter.current_token.type = TOKEN_RBRACE; interpreter.pos++; return;
        case '[': interpreter.current_token.type = TOKEN_LBRACKET; interpreter.pos++; return;
        case ']': interpreter.current_token.type = TOKEN_RBRACKET; interpreter.pos++; return;
    }
    
    // Two character tokens
    if (c == '=' && interpreter.source[interpreter.pos + 1] == '=') {
        interpreter.current_token.type = TOKEN_EQUAL;
        interpreter.pos += 2;
        return;
    }
    if (c == '!' && interpreter.source[interpreter.pos + 1] == '=') {
        interpreter.current_token.type = TOKEN_NOT_EQUAL;
        interpreter.pos += 2;
        return;
    }
    if (c == '<' && interpreter.source[interpreter.pos + 1] == '=') {
        interpreter.current_token.type = TOKEN_LESS_EQUAL;
        interpreter.pos += 2;
        return;
    }
    if (c == '>' && interpreter.source[interpreter.pos + 1] == '=') {
        interpreter.current_token.type = TOKEN_GREATER_EQUAL;
        interpreter.pos += 2;
        return;
    }
    if (c == '&' && interpreter.source[interpreter.pos + 1] == '&') {
        interpreter.current_token.type = TOKEN_AND;
        interpreter.pos += 2;
        return;
    }
    if (c == '|' && interpreter.source[interpreter.pos + 1] == '|') {
        interpreter.current_token.type = TOKEN_OR;
        interpreter.pos += 2;
        return;
    }
    
    // Single character comparison operators
    if (c == '=') { interpreter.current_token.type = TOKEN_ASSIGN; interpreter.pos++; return; }
    if (c == '<') { interpreter.current_token.type = TOKEN_LESS; interpreter.pos++; return; }
    if (c == '>') { interpreter.current_token.type = TOKEN_GREATER; interpreter.pos++; return; }
    if (c == '!') { interpreter.current_token.type = TOKEN_NOT; interpreter.pos++; return; }
    
    // String literals
    if (c == '"') {
        int start = ++interpreter.pos;
        while (interpreter.pos < strlen(interpreter.source) && 
               interpreter.source[interpreter.pos] != '"') {
            interpreter.pos++;
        }
        if (interpreter.pos >= strlen(interpreter.source)) {
            error("Unterminated string");
        }
        int len = interpreter.pos - start;
        strncpy(interpreter.current_token.lexeme, &interpreter.source[start], len);
        interpreter.current_token.lexeme[len] = '\0';
        interpreter.current_token.type = TOKEN_STRING;
        interpreter.pos++; // Skip closing quote
        return;
    }
    
    // Character literals
    if (c == '\'') {
        interpreter.pos++;
        if (interpreter.pos >= strlen(interpreter.source)) {
            error("Unterminated character");
        }
        interpreter.current_token.value.type = TYPE_CHAR;
        interpreter.current_token.value.char_val = interpreter.source[interpreter.pos++];
        if (interpreter.pos >= strlen(interpreter.source) || 
            interpreter.source[interpreter.pos] != '\'') {
            error("Unterminated character");
        }
        interpreter.current_token.type = TOKEN_CHAR;
        interpreter.pos++;
        return;
    }
    
    // Numbers
    if (isdigit(c)) {
        int start = interpreter.pos;
        int has_dot = 0;
        while (interpreter.pos < strlen(interpreter.source) && 
               (isdigit(interpreter.source[interpreter.pos]) || 
                (!has_dot && interpreter.source[interpreter.pos] == '.'))) {
            if (interpreter.source[interpreter.pos] == '.') has_dot = 1;
            interpreter.pos++;
        }
        int len = interpreter.pos - start;
        strncpy(interpreter.current_token.lexeme, &interpreter.source[start], len);
        interpreter.current_token.lexeme[len] = '\0';
        interpreter.current_token.type = TOKEN_NUMBER;
        if (has_dot) {
            interpreter.current_token.value.type = TYPE_FLOAT;
            interpreter.current_token.value.float_val = atof(interpreter.current_token.lexeme);
        } else {
            interpreter.current_token.value.type = TYPE_INT;
            interpreter.current_token.value.int_val = atoi(interpreter.current_token.lexeme);
        }
        return;
    }
    
    // Identifiers and keywords
    if (isalpha(c) || c == '_') {
        int start = interpreter.pos;
        while (interpreter.pos < strlen(interpreter.source) && 
               (isalnum(interpreter.source[interpreter.pos]) || 
                interpreter.source[interpreter.pos] == '_')) {
            interpreter.pos++;
        }
        int len = interpreter.pos - start;
        strncpy(interpreter.current_token.lexeme, &interpreter.source[start], len);
        interpreter.current_token.lexeme[len] = '\0';
        
        // Check for keywords
        if (strcmp(interpreter.current_token.lexeme, "if") == 0) {
            interpreter.current_token.type = TOKEN_IF;
        } else if (strcmp(interpreter.current_token.lexeme, "else") == 0) {
            interpreter.current_token.type = TOKEN_ELSE;
        } else if (strcmp(interpreter.current_token.lexeme, "while") == 0) {
            interpreter.current_token.type = TOKEN_WHILE;
        } else if (strcmp(interpreter.current_token.lexeme, "for") == 0) {
            interpreter.current_token.type = TOKEN_FOR;
        } else if (strcmp(interpreter.current_token.lexeme, "function") == 0) {
            interpreter.current_token.type = TOKEN_FUNCTION;
        } else if (strcmp(interpreter.current_token.lexeme, "return") == 0) {
            interpreter.current_token.type = TOKEN_RETURN;
        } else if (strcmp(interpreter.current_token.lexeme, "int") == 0) {
            interpreter.current_token.type = TOKEN_INT;
        } else if (strcmp(interpreter.current_token.lexeme, "float") == 0) {
            interpreter.current_token.type = TOKEN_FLOAT;
        } else if (strcmp(interpreter.current_token.lexeme, "char") == 0) {
            interpreter.current_token.type = TOKEN_CHAR_TYPE;
        } else if (strcmp(interpreter.current_token.lexeme, "string") == 0) {
            interpreter.current_token.type = TOKEN_STRING_TYPE;
        } else if (strcmp(interpreter.current_token.lexeme, "list") == 0) {
            interpreter.current_token.type = TOKEN_LIST;
        } else if (strcmp(interpreter.current_token.lexeme, "map") == 0) {
            interpreter.current_token.type = TOKEN_MAP;
        } else if (strcmp(interpreter.current_token.lexeme, "true") == 0) {
            interpreter.current_token.type = TOKEN_TRUE;
            interpreter.current_token.value.type = TYPE_BOOL;
            interpreter.current_token.value.bool_val = 1;
        } else if (strcmp(interpreter.current_token.lexeme, "false") == 0) {
            interpreter.current_token.type = TOKEN_FALSE;
            interpreter.current_token.value.type = TYPE_BOOL;
            interpreter.current_token.value.bool_val = 0;
        } else if (strcmp(interpreter.current_token.lexeme, "loadlib") == 0) {
            interpreter.current_token.type = TOKEN_LOADLIB;
        } else if (strcmp(interpreter.current_token.lexeme, "getproc") == 0) {
            interpreter.current_token.type = TOKEN_GETPROC;
        } else if (strcmp(interpreter.current_token.lexeme, "freelib") == 0) {
            interpreter.current_token.type = TOKEN_FREELIB;
        } else if (strcmp(interpreter.current_token.lexeme, "callext") == 0) {
            interpreter.current_token.type = TOKEN_CALLEXT;
        } else {
            interpreter.current_token.type = TOKEN_IDENTIFIER;
        }
        return;
    }
    
    // Check if we're near the end of file and encountering unexpected characters
    if (interpreter.pos >= strlen(interpreter.source) - 100) {
        // Near end of file, treat any unexpected character as EOF
        interpreter.current_token.type = TOKEN_EOF;
        return;
    }
    
    error("Unexpected character: %c (ASCII %d)", c, (int)c);
}

// Value creation and management
Value* create_value(ValueType type) {
    Value* val = malloc(sizeof(Value));
    val->type = type;
    switch (type) {
        case TYPE_INT: val->int_val = 0; break;
        case TYPE_FLOAT: val->float_val = 0.0; break;
        case TYPE_CHAR: val->char_val = '\0'; break;
        case TYPE_STRING: val->string_val = strdup(""); break;
        case TYPE_LIST: val->list_val = create_list(); break;
        case TYPE_MAP: val->map_val = create_map(); break;
        case TYPE_BOOL: val->bool_val = 0; break;
#ifdef _WIN32
        case TYPE_DLL_HANDLE: val->dll_handle = NULL; break;
        case TYPE_DLL_FUNCTION: val->dll_function = NULL; break;
#endif
    }
    return val;
}

void free_value(Value* val) {
    if (!val) return;
    
    switch (val->type) {
        case TYPE_STRING:
            if (val->string_val) free(val->string_val);
            break;
        case TYPE_LIST:
            if (val->list_val) {
                for (int i = 0; i < val->list_val->size; i++) {
                    free_value(&val->list_val->elements[i]);
                }
                free(val->list_val->elements);
                free(val->list_val);
            }
            break;
        case TYPE_MAP:
            if (val->map_val) {
                for (int i = 0; i < MAX_MAP_SIZE; i++) {
                    MapEntry* entry = val->map_val->buckets[i];
                    while (entry) {
                        MapEntry* next = entry->next;
                        free(entry->key);
                        free_value(entry->value);
                        free(entry->value);
                        free(entry);
                        entry = next;
                    }
                }
                free(val->map_val);
            }
            break;
#ifdef _WIN32
        case TYPE_DLL_HANDLE:
            // Don't free DLL handles here - they're managed by the interpreter
            break;
        case TYPE_DLL_FUNCTION:
            // Don't free function pointers here - they're managed by the interpreter
            break;
#endif
        default:
            break;
    }
}

// List operations
List* create_list() {
    List* list = malloc(sizeof(List));
    list->elements = malloc(sizeof(Value) * 10);
    list->size = 0;
    list->capacity = 10;
    return list;
}

void list_append(List* list, Value* val) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->elements = realloc(list->elements, sizeof(Value) * list->capacity);
    }
    list->elements[list->size] = *val;
    list->size++;
}

Value* list_get(List* list, int index) {
    if (index < 0 || index >= list->size) {
        error("List index out of bounds");
    }
    return &list->elements[index];
}

// Map operations
unsigned int hash(const char* key) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % MAX_MAP_SIZE;
}

Map* create_map() {
    Map* map = malloc(sizeof(Map));
    for (int i = 0; i < MAX_MAP_SIZE; i++) {
        map->buckets[i] = NULL;
    }
    map->size = 0;
    return map;
}

void map_set(Map* map, const char* key, Value* val) {
    unsigned int index = hash(key);
    MapEntry* entry = map->buckets[index];
    
    // Check if key already exists
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            free_value(entry->value);
            *(entry->value) = *val;
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry
    entry = malloc(sizeof(MapEntry));
    entry->key = strdup(key);
    entry->value = malloc(sizeof(Value));
    *(entry->value) = *val;
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
    map->size++;
}

Value* map_get(Map* map, const char* key) {
    unsigned int index = hash(key);
    MapEntry* entry = map->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

// Variable and function management
Variable* find_variable(const char* name) {
    // Check current stack frame first
    if (interpreter.stack_depth > 0) {
        for (int i = 0; i < interpreter.stack_vars[interpreter.stack_depth - 1]; i++) {
            if (strcmp(interpreter.stack[interpreter.stack_depth - 1][i].name, name) == 0) {
                return &interpreter.stack[interpreter.stack_depth - 1][i];
            }
        }
    }
    
    // Check global variables
    for (int i = 0; i < interpreter.var_count; i++) {
        if (strcmp(interpreter.variables[i].name, name) == 0) {
            return &interpreter.variables[i];
        }
    }
    return NULL;
}

Function* find_function(const char* name) {
    for (int i = 0; i < interpreter.func_count; i++) {
        if (strcmp(interpreter.functions[i].name, name) == 0) {
            return &interpreter.functions[i];
        }
    }
    return NULL;
}

// AST Node creation
ASTNode* create_node(NodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    return node;
}

// Parsing functions
ASTNode* parse_primary() {
    ASTNode* node = create_node(NODE_LITERAL);
    
    if (interpreter.current_token.type == TOKEN_NUMBER) {
        node->value = interpreter.current_token.value;
        next_token();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_STRING) {
        node->value.type = TYPE_STRING;
        node->value.string_val = strdup(interpreter.current_token.lexeme);
        next_token();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_CHAR) {
        node->value = interpreter.current_token.value;
        next_token();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_TRUE || 
        interpreter.current_token.type == TOKEN_FALSE) {
        node->value = interpreter.current_token.value;
        next_token();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_IDENTIFIER) {
        strcpy(node->identifier, interpreter.current_token.lexeme);
        next_token();
        
        // Check for function call
        if (interpreter.current_token.type == TOKEN_LPAREN) {
            node->type = NODE_CALL;
            next_token(); // consume '('
            
            // Parse arguments
            node->args = malloc(sizeof(ASTNode*) * MAX_PARAMS);
            node->arg_count = 0;
            
            if (interpreter.current_token.type != TOKEN_RPAREN) {
                do {
                    node->args[node->arg_count++] = parse_expression();
                    if (interpreter.current_token.type == TOKEN_COMMA) {
                        next_token();
                    } else {
                        break;
                    }
                } while (1);
            }
            
            if (interpreter.current_token.type != TOKEN_RPAREN) {
                error("Expected ')' after function arguments");
            }
            next_token();
            return node;
        } else {
            node->type = NODE_IDENTIFIER;
            return node;
        }
    }
    
    if (interpreter.current_token.type == TOKEN_LPAREN) {
        next_token();
        node = parse_expression();
        if (interpreter.current_token.type != TOKEN_RPAREN) {
            error("Expected ')'");
        }
        next_token();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_LBRACKET) {
        // List literal
        node->type = NODE_LITERAL;
        node->value.type = TYPE_LIST;
        node->value.list_val = create_list();
        
        next_token(); // consume '['
        
        if (interpreter.current_token.type != TOKEN_RBRACKET) {
            do {
                ASTNode* element = parse_expression();
                Value val = evaluate(element);
                list_append(node->value.list_val, &val);
                
                if (interpreter.current_token.type == TOKEN_COMMA) {
                    next_token();
                } else {
                    break;
                }
            } while (1);
        }
        
        if (interpreter.current_token.type != TOKEN_RBRACKET) {
            error("Expected ']'");
        }
        next_token();
        return node;
    }
    
    error("Expected expression");
    return NULL;
}

ASTNode* parse_unary() {
    if (interpreter.current_token.type == TOKEN_MINUS || 
        interpreter.current_token.type == TOKEN_NOT) {
        ASTNode* node = create_node(NODE_UNARY_OP);
        node->operator = interpreter.current_token.type;
        next_token();
        node->left = parse_unary();
        return node;
    }
    
    return parse_primary();
}

ASTNode* parse_multiplicative() {
    ASTNode* left = parse_unary();
    
    while (interpreter.current_token.type == TOKEN_MULTIPLY || 
           interpreter.current_token.type == TOKEN_DIVIDE) {
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->operator = interpreter.current_token.type;
        node->left = left;
        next_token();
        node->right = parse_unary();
        left = node;
    }
    
    return left;
}

ASTNode* parse_additive() {
    ASTNode* left = parse_multiplicative();
    
    while (interpreter.current_token.type == TOKEN_PLUS || 
           interpreter.current_token.type == TOKEN_MINUS) {
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->operator = interpreter.current_token.type;
        node->left = left;
        next_token();
        node->right = parse_multiplicative();
        left = node;
    }
    
    return left;
}

ASTNode* parse_relational() {
    ASTNode* left = parse_additive();
    
    while (interpreter.current_token.type == TOKEN_LESS ||
           interpreter.current_token.type == TOKEN_GREATER ||
           interpreter.current_token.type == TOKEN_LESS_EQUAL ||
           interpreter.current_token.type == TOKEN_GREATER_EQUAL) {
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->operator = interpreter.current_token.type;
        node->left = left;
        next_token();
        node->right = parse_additive();
        left = node;
    }
    
    return left;
}

ASTNode* parse_equality() {
    ASTNode* left = parse_relational();
    
    while (interpreter.current_token.type == TOKEN_EQUAL ||
           interpreter.current_token.type == TOKEN_NOT_EQUAL) {
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->operator = interpreter.current_token.type;
        node->left = left;
        next_token();
        node->right = parse_relational();
        left = node;
    }
    
    return left;
}

ASTNode* parse_logical_and() {
    ASTNode* left = parse_equality();
    
    while (interpreter.current_token.type == TOKEN_AND) {
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->operator = interpreter.current_token.type;
        node->left = left;
        next_token();
        node->right = parse_equality();
        left = node;
    }
    
    return left;
}

ASTNode* parse_logical_or() {
    ASTNode* left = parse_logical_and();
    
    while (interpreter.current_token.type == TOKEN_OR) {
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->operator = interpreter.current_token.type;
        node->left = left;
        next_token();
        node->right = parse_logical_and();
        left = node;
    }
    
    return left;
}

ASTNode* parse_assignment() {
    ASTNode* left = parse_logical_or();
    
    if (interpreter.current_token.type == TOKEN_ASSIGN) {
        ASTNode* node = create_node(NODE_ASSIGN);
        node->left = left;
        next_token();
        node->right = parse_assignment();
        return node;
    }
    
    return left;
}

ASTNode* parse_expression() {
    return parse_assignment();
}

ASTNode* parse_statement() {
    if (interpreter.current_token.type == TOKEN_IF) {
        ASTNode* node = create_node(NODE_IF);
        next_token();
        
        if (interpreter.current_token.type != TOKEN_LPAREN) {
            error("Expected '(' after 'if'");
        }
        next_token();
        
        node->condition = parse_expression();
        
        if (interpreter.current_token.type != TOKEN_RPAREN) {
            error("Expected ')' after if condition");
        }
        next_token();
        
        node->then_branch = parse_statement();
        
        if (interpreter.current_token.type == TOKEN_ELSE) {
            next_token();
            node->else_branch = parse_statement();
        }
        
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_WHILE) {
        ASTNode* node = create_node(NODE_WHILE);
        next_token();
        
        if (interpreter.current_token.type != TOKEN_LPAREN) {
            error("Expected '(' after 'while'");
        }
        next_token();
        
        node->condition = parse_expression();
        
        if (interpreter.current_token.type != TOKEN_RPAREN) {
            error("Expected ')' after while condition");
        }
        next_token();
        
        node->body = parse_statement();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_FOR) {
        ASTNode* node = create_node(NODE_FOR);
        next_token();
        
        if (interpreter.current_token.type != TOKEN_LPAREN) {
            error("Expected '(' after 'for'");
        }
        next_token();
        
        // Init
        if (interpreter.current_token.type != TOKEN_SEMICOLON) {
            node->init = parse_expression();
        }
        if (interpreter.current_token.type != TOKEN_SEMICOLON) {
            error("Expected ';' after for init");
        }
        next_token();
        
        // Condition
        if (interpreter.current_token.type != TOKEN_SEMICOLON) {
            node->condition = parse_expression();
        }
        if (interpreter.current_token.type != TOKEN_SEMICOLON) {
            error("Expected ';' after for condition");
        }
        next_token();
        
        // Update
        if (interpreter.current_token.type != TOKEN_RPAREN) {
            node->update = parse_expression();
        }
        if (interpreter.current_token.type != TOKEN_RPAREN) {
            error("Expected ')' after for update");
        }
        next_token();
        
        node->body = parse_statement();
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_RETURN) {
        ASTNode* node = create_node(NODE_RETURN);
        next_token();
        
        if (interpreter.current_token.type != TOKEN_SEMICOLON) {
            node->left = parse_expression();
        }
        
        if (interpreter.current_token.type == TOKEN_SEMICOLON) {
            next_token();
        }
        
        return node;
    }
    
    if (interpreter.current_token.type == TOKEN_LBRACE) {
        return parse_block();
    }
    
    // Expression statement
    ASTNode* node = parse_expression();
    if (interpreter.current_token.type == TOKEN_SEMICOLON) {
        next_token();
    }
    return node;
}

ASTNode* parse_block() {
    ASTNode* node = create_node(NODE_BLOCK);
    node->statements = malloc(sizeof(ASTNode*) * 100);
    node->statement_count = 0;
    
    if (interpreter.current_token.type != TOKEN_LBRACE) {
        error("Expected '{'");
    }
    next_token();
    
    while (interpreter.current_token.type != TOKEN_RBRACE && 
           interpreter.current_token.type != TOKEN_EOF) {
        node->statements[node->statement_count++] = parse_statement();
    }
    
    if (interpreter.current_token.type != TOKEN_RBRACE) {
        error("Expected '}'");
    }
    next_token();
    
    return node;
}

// Function parsing
ASTNode* parse_function() {
    if (interpreter.current_token.type != TOKEN_FUNCTION) {
        error("Expected 'function'");
    }
    next_token();
    
    if (interpreter.current_token.type != TOKEN_IDENTIFIER) {
        error("Expected function name");
    }
    
    Function* func = &interpreter.functions[interpreter.func_count++];
    strcpy(func->name, interpreter.current_token.lexeme);
    next_token();
    
    if (interpreter.current_token.type != TOKEN_LPAREN) {
        error("Expected '(' after function name");
    }
    next_token();
    
    func->param_count = 0;
    
    // Parse parameters
    if (interpreter.current_token.type != TOKEN_RPAREN) {
        do {
            // Type
            ValueType type;
            if (interpreter.current_token.type == TOKEN_INT) {
                type = TYPE_INT;
            } else if (interpreter.current_token.type == TOKEN_FLOAT) {
                type = TYPE_FLOAT;
            } else if (interpreter.current_token.type == TOKEN_CHAR_TYPE) {
                type = TYPE_CHAR;
            } else if (interpreter.current_token.type == TOKEN_STRING_TYPE) {
                type = TYPE_STRING;
            } else {
                error("Expected parameter type");
            }
            next_token();
            
            // Name
            if (interpreter.current_token.type != TOKEN_IDENTIFIER) {
                error("Expected parameter name");
            }
            
            func->params[func->param_count].type = type;
            strcpy(func->params[func->param_count].name, interpreter.current_token.lexeme);
            func->param_count++;
            next_token();
            
            if (interpreter.current_token.type == TOKEN_COMMA) {
                next_token();
            } else {
                break;
            }
        } while (1);
    }
    
    if (interpreter.current_token.type != TOKEN_RPAREN) {
        error("Expected ')' after parameters");
    }
    next_token();
    
    // Function body - we'll store it as a string for now
    int start = interpreter.pos;
    parse_block(); // This advances the parser
    int end = interpreter.pos;
    
    func->body = malloc(end - start + 1);
    strncpy(func->body, &interpreter.source[start], end - start);
    func->body[end - start] = '\0';
    
    return NULL; // Function declarations don't return nodes
}

// Value evaluation
Value evaluate(ASTNode* node) {
    Value result;
    result.type = TYPE_INT;
    result.int_val = 0;
    
    switch (node->type) {
        case NODE_LITERAL:
            return node->value;
            
        case NODE_IDENTIFIER: {
            Variable* var = find_variable(node->identifier);
            if (!var) {
                error("Undefined variable: %s", node->identifier);
            }
            return var->value;
        }
        
        case NODE_BINARY_OP: {
            Value left = evaluate(node->left);
            Value right = evaluate(node->right);
            
            // Type promotion and operations
            switch (node->operator) {
                case TOKEN_PLUS:
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.type = TYPE_INT;
                        result.int_val = left.int_val + right.int_val;
                    } else if (left.type == TYPE_STRING && right.type == TYPE_STRING) {
                        result.type = TYPE_STRING;
                        result.string_val = malloc(strlen(left.string_val) + strlen(right.string_val) + 1);
                        strcpy(result.string_val, left.string_val);
                        strcat(result.string_val, right.string_val);
                    } else {
                        result.type = TYPE_FLOAT;
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.float_val = l + r;
                    }
                    break;
                    
                case TOKEN_MINUS:
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.type = TYPE_INT;
                        result.int_val = left.int_val - right.int_val;
                    } else {
                        result.type = TYPE_FLOAT;
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.float_val = l - r;
                    }
                    break;
                    
                case TOKEN_MULTIPLY:
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.type = TYPE_INT;
                        result.int_val = left.int_val * right.int_val;
                    } else {
                        result.type = TYPE_FLOAT;
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.float_val = l * r;
                    }
                    break;
                    
                case TOKEN_DIVIDE:
                    result.type = TYPE_FLOAT;
                    {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        if (r == 0) error("Division by zero");
                        result.float_val = l / r;
                    }
                    break;
                    
                case TOKEN_EQUAL:
                    result.type = TYPE_BOOL;
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.bool_val = left.int_val == right.int_val;
                    } else if (left.type == TYPE_STRING && right.type == TYPE_STRING) {
                        result.bool_val = strcmp(left.string_val, right.string_val) == 0;
                    } else {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.bool_val = l == r;
                    }
                    break;
                    
                case TOKEN_LESS:
                    result.type = TYPE_BOOL;
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.bool_val = left.int_val < right.int_val;
                    } else {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.bool_val = l < r;
                    }
                    break;
                    
                case TOKEN_GREATER:
                    result.type = TYPE_BOOL;
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.bool_val = left.int_val > right.int_val;
                    } else {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.bool_val = l > r;
                    }
                    break;
                    
                case TOKEN_LESS_EQUAL:
                    result.type = TYPE_BOOL;
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.bool_val = left.int_val <= right.int_val;
                    } else {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.bool_val = l <= r;
                    }
                    break;
                    
                case TOKEN_GREATER_EQUAL:
                    result.type = TYPE_BOOL;
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.bool_val = left.int_val >= right.int_val;
                    } else {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.bool_val = l >= r;
                    }
                    break;
                    
                case TOKEN_NOT_EQUAL:
                    result.type = TYPE_BOOL;
                    if (left.type == TYPE_INT && right.type == TYPE_INT) {
                        result.bool_val = left.int_val != right.int_val;
                    } else if (left.type == TYPE_STRING && right.type == TYPE_STRING) {
                        result.bool_val = strcmp(left.string_val, right.string_val) != 0;
                    } else {
                        float l = (left.type == TYPE_INT) ? left.int_val : left.float_val;
                        float r = (right.type == TYPE_INT) ? right.int_val : right.float_val;
                        result.bool_val = l != r;
                    }
                    break;
                    
                case TOKEN_AND:
                    result.type = TYPE_BOOL;
                    result.bool_val = (left.bool_val || left.int_val) && (right.bool_val || right.int_val);
                    break;
                    
                case TOKEN_OR:
                    result.type = TYPE_BOOL;
                    result.bool_val = (left.bool_val || left.int_val) || (right.bool_val || right.int_val);
                    break;
                    
                default:
                    error("Unknown binary operator");
            }
            break;
        }
        
        case NODE_UNARY_OP: {
            Value operand = evaluate(node->left);
            
            switch (node->operator) {
                case TOKEN_MINUS:
                    if (operand.type == TYPE_INT) {
                        result.type = TYPE_INT;
                        result.int_val = -operand.int_val;
                    } else {
                        result.type = TYPE_FLOAT;
                        result.float_val = -operand.float_val;
                    }
                    break;
                    
                case TOKEN_NOT:
                    result.type = TYPE_BOOL;
                    result.bool_val = !(operand.bool_val || operand.int_val);
                    break;
                    
                default:
                    error("Unknown unary operator");
            }
            break;
        }
        
        case NODE_CALL: {
            Function* func = find_function(node->identifier);
            if (!func) {
                // Built-in functions
                if (strcmp(node->identifier, "print") == 0) {
                    for (int i = 0; i < node->arg_count; i++) {
                        Value arg = evaluate(node->args[i]);
                        print_value(&arg);
                        if (i < node->arg_count - 1) printf(" ");
                    }
                    printf("\n");
                    result.type = TYPE_INT;
                    result.int_val = 0;
                } else if (strcmp(node->identifier, "len") == 0) {
                    if (node->arg_count != 1) {
                        error("len() expects 1 argument");
                    }
                    Value arg = evaluate(node->args[0]);
                    result.type = TYPE_INT;
                    if (arg.type == TYPE_LIST) {
                        result.int_val = arg.list_val->size;
                    } else if (arg.type == TYPE_STRING) {
                        result.int_val = strlen(arg.string_val);
                    } else {
                        error("len() only works on lists and strings");
                    }
#ifdef _WIN32
                } else if (strcmp(node->identifier, "loadlib") == 0) {
                    if (node->arg_count != 1) {
                        error("loadlib() expects 1 argument");
                    }
                    Value arg = evaluate(node->args[0]);
                    if (arg.type != TYPE_STRING) {
                        error("loadlib() expects a string argument");
                    }
                    
                    HMODULE handle = load_dll(arg.string_val);
                    result.type = TYPE_DLL_HANDLE;
                    result.dll_handle = handle;
                    
                } else if (strcmp(node->identifier, "getproc") == 0) {
                    if (node->arg_count != 2) {
                        error("getproc() expects 2 arguments (dll_name, function_name)");
                    }
                    Value dll_arg = evaluate(node->args[0]);
                    Value func_arg = evaluate(node->args[1]);
                    
                    if (dll_arg.type != TYPE_STRING || func_arg.type != TYPE_STRING) {
                        error("getproc() expects string arguments");
                    }
                    
                    FARPROC proc = get_dll_function(dll_arg.string_val, func_arg.string_val);
                    result.type = TYPE_DLL_FUNCTION;
                    result.dll_function = proc;
                    
                } else if (strcmp(node->identifier, "freelib") == 0) {
                    if (node->arg_count != 1) {
                        error("freelib() expects 1 argument");
                    }
                    Value arg = evaluate(node->args[0]);
                    if (arg.type != TYPE_STRING) {
                        error("freelib() expects a string argument");
                    }
                    
                    free_dll(arg.string_val);
                    result.type = TYPE_INT;
                    result.int_val = 0;
                    
                } else if (strcmp(node->identifier, "callext") == 0) {
                    if (node->arg_count < 1) {
                        error("callext() expects at least 1 argument (function_name, ...)");
                    }
                    Value func_arg = evaluate(node->args[0]);
                    if (func_arg.type != TYPE_STRING) {
                        error("callext() expects function name as first argument");
                    }
                    
                    // Call the external function with remaining arguments
                    result = call_external_function(func_arg.string_val, 
                                                  &node->args[1], 
                                                  node->arg_count - 1);
#endif
                } else {
                    error("Unknown function: %s", node->identifier);
                }
            } else {
                // User-defined function - implement later
                error("User-defined functions not fully implemented");
            }
            break;
        }
        
        case NODE_ASSIGN: {
            // Handle assignment in expression context
            if (node->left->type != NODE_IDENTIFIER) {
                error("Invalid assignment target");
            }
            
            Value val = evaluate(node->right);
            Variable* var = find_variable(node->left->identifier);
            
            if (!var) {
                // Create new variable
                if (interpreter.stack_depth > 0) {
                    // Local variable
                    int idx = interpreter.stack_vars[interpreter.stack_depth - 1]++;
                    strcpy(interpreter.stack[interpreter.stack_depth - 1][idx].name, 
                           node->left->identifier);
                    interpreter.stack[interpreter.stack_depth - 1][idx].value = val;
                } else {
                    // Global variable
                    strcpy(interpreter.variables[interpreter.var_count].name, 
                           node->left->identifier);
                    interpreter.variables[interpreter.var_count].value = val;
                    interpreter.var_count++;
                }
            } else {
                var->value = val;
            }
            
            return val; // Return the assigned value
        }
        
        default:
            error("Cannot evaluate node type");
    }
    
    return result;
}

// Statement execution
void execute_statement(ASTNode* node) {
    switch (node->type) {
        case NODE_ASSIGN: {
            if (node->left->type != NODE_IDENTIFIER) {
                error("Invalid assignment target");
            }
            
            Value val = evaluate(node->right);
            Variable* var = find_variable(node->left->identifier);
            
            if (!var) {
                // Create new variable
                if (interpreter.stack_depth > 0) {
                    // Local variable
                    int idx = interpreter.stack_vars[interpreter.stack_depth - 1]++;
                    strcpy(interpreter.stack[interpreter.stack_depth - 1][idx].name, 
                           node->left->identifier);
                    interpreter.stack[interpreter.stack_depth - 1][idx].value = val;
                } else {
                    // Global variable
                    strcpy(interpreter.variables[interpreter.var_count].name, 
                           node->left->identifier);
                    interpreter.variables[interpreter.var_count].value = val;
                    interpreter.var_count++;
                }
            } else {
                var->value = val;
            }
            break;
        }
        
        case NODE_IF: {
            Value condition = evaluate(node->condition);
            int is_true = (condition.type == TYPE_BOOL) ? condition.bool_val :
                         (condition.type == TYPE_INT) ? condition.int_val != 0 :
                         (condition.type == TYPE_FLOAT) ? condition.float_val != 0.0 : 0;
            
            if (is_true) {
                execute_statement(node->then_branch);
            } else if (node->else_branch) {
                execute_statement(node->else_branch);
            }
            break;
        }
        
        case NODE_WHILE: {
            while (1) {
                Value condition = evaluate(node->condition);
                int is_true = (condition.type == TYPE_BOOL) ? condition.bool_val :
                             (condition.type == TYPE_INT) ? condition.int_val != 0 :
                             (condition.type == TYPE_FLOAT) ? condition.float_val != 0.0 : 0;
                
                if (!is_true) break;
                
                execute_statement(node->body);
                
                if (interpreter.has_return) break;
            }
            break;
        }
        
        case NODE_FOR: {
            if (node->init) {
                execute_statement(node->init);
            }
            
            while (1) {
                if (node->condition) {
                    Value condition = evaluate(node->condition);
                    int is_true = (condition.type == TYPE_BOOL) ? condition.bool_val :
                                 (condition.type == TYPE_INT) ? condition.int_val != 0 :
                                 (condition.type == TYPE_FLOAT) ? condition.float_val != 0.0 : 0;
                    
                    if (!is_true) break;
                }
                
                execute_statement(node->body);
                
                if (interpreter.has_return) break;
                
                if (node->update) {
                    evaluate(node->update);
                }
            }
            break;
        }
        
        case NODE_BLOCK: {
            for (int i = 0; i < node->statement_count; i++) {
                execute_statement(node->statements[i]);
                if (interpreter.has_return) break;
            }
            break;
        }
        
        case NODE_RETURN: {
            if (node->left) {
                interpreter.return_value = evaluate(node->left);
            } else {
                interpreter.return_value.type = TYPE_INT;
                interpreter.return_value.int_val = 0;
            }
            interpreter.has_return = 1;
            break;
        }
        
        default:
            // Expression statement
            evaluate(node);
            break;
    }
}

// Print value utility
void print_value(Value* val) {
    switch (val->type) {
        case TYPE_INT:
            printf("%d", val->int_val);
            break;
        case TYPE_FLOAT:
            printf("%.2f", val->float_val);
            break;
        case TYPE_CHAR:
            printf("%c", val->char_val);
            break;
        case TYPE_STRING:
            printf("%s", val->string_val);
            break;
        case TYPE_BOOL:
            printf("%s", val->bool_val ? "true" : "false");
            break;
        case TYPE_LIST:
            printf("[");
            for (int i = 0; i < val->list_val->size; i++) {
                print_value(&val->list_val->elements[i]);
                if (i < val->list_val->size - 1) printf(", ");
            }
            printf("]");
            break;
        case TYPE_MAP:
            printf("{map}");
            break;
#ifdef _WIN32
        case TYPE_DLL_HANDLE:
            printf("<dll_handle:0x%p>", (void*)val->dll_handle);
            break;
        case TYPE_DLL_FUNCTION:
            printf("<dll_function:0x%p>", (void*)val->dll_function);
            break;
#endif
    }
}

#ifdef _WIN32
// DLL Management Functions

DllModule* find_dll_module(const char* name) {
    for (int i = 0; i < interpreter.dll_module_count; i++) {
        if (strcmp(interpreter.dll_modules[i].name, name) == 0) {
            return &interpreter.dll_modules[i];
        }
    }
    return NULL;
}

DllFunction* find_dll_function(const char* name) {
    for (int i = 0; i < interpreter.dll_function_count; i++) {
        if (strcmp(interpreter.dll_functions[i].name, name) == 0) {
            return &interpreter.dll_functions[i];
        }
    }
    return NULL;
}

HMODULE load_dll(const char* dll_name) {
    // Check if already loaded
    DllModule* existing = find_dll_module(dll_name);
    if (existing) {
        return existing->handle;
    }
    
    // Convert to wide string for LoadLibrary
    int len = strlen(dll_name) + 1;
    wchar_t* wide_name = malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, dll_name, -1, wide_name, len);
    
    HMODULE handle = LoadLibrary(wide_name);
    free(wide_name);
    
    if (handle != NULL) {
        // Store the module
        DllModule* module = &interpreter.dll_modules[interpreter.dll_module_count++];
        strcpy(module->name, dll_name);
        module->handle = handle;
        
        printf("Loaded DLL: %s\n", dll_name);
    } else {
        DWORD win_error = GetLastError();
        error("Failed to load DLL '%s'. Error code: %lu", dll_name, win_error);
    }
    
    return handle;
}

FARPROC get_dll_function(const char* dll_name, const char* func_name) {
    DllModule* module = find_dll_module(dll_name);
    if (!module) {
        error("DLL '%s' not loaded", dll_name);
    }
    
    FARPROC proc = GetProcAddress(module->handle, func_name);
    if (proc != NULL) {
        // Store the function
        DllFunction* func = &interpreter.dll_functions[interpreter.dll_function_count++];
        strcpy(func->name, func_name);
        strcpy(func->module_name, dll_name);
        func->proc_address = proc;
        func->param_count = 0; // Will be set when called
        func->return_type = TYPE_INT; // Default return type
        
        printf("Got function address for: %s from %s\n", func_name, dll_name);
    } else {
        error("Function '%s' not found in DLL '%s'", func_name, dll_name);
    }
    
    return proc;
}

void free_dll(const char* dll_name) {
    DllModule* module = find_dll_module(dll_name);
    if (module) {
        FreeLibrary(module->handle);
        printf("Freed DLL: %s\n", dll_name);
        
        // Remove from array (simple implementation - just mark as invalid)
        module->handle = NULL;
        module->name[0] = '\0';
    }
}

Value call_external_function(const char* func_name, ASTNode** args, int arg_count) {
    Value result;
    result.type = TYPE_INT;
    result.int_val = 0;
    
    DllFunction* func = find_dll_function(func_name);
    if (!func) {
        error("External function '%s' not found", func_name);
    }
    
    // Simple implementation for MessageBoxA-style functions
    // This is a demonstration - in a real implementation you'd need
    // more sophisticated type conversion and calling conventions
    
    if (strcmp(func_name, "MessageBoxA") == 0 && arg_count == 4) {
        // Expected signature: int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
        Value arg0 = evaluate(args[0]); // hWnd (usually NULL)
        Value arg1 = evaluate(args[1]); // lpText
        Value arg2 = evaluate(args[2]); // lpCaption  
        Value arg3 = evaluate(args[3]); // uType
        
        // Convert arguments
        HWND hWnd = (HWND)(arg0.type == TYPE_INT ? (void*)(intptr_t)arg0.int_val : NULL);
        LPCSTR lpText = (arg1.type == TYPE_STRING ? arg1.string_val : "");
        LPCSTR lpCaption = (arg2.type == TYPE_STRING ? arg2.string_val : "");
        UINT uType = (arg3.type == TYPE_INT ? (UINT)arg3.int_val : 0);
        
        // Call the function
        typedef int (WINAPI *MessageBoxAFunc)(HWND, LPCSTR, LPCSTR, UINT);
        MessageBoxAFunc msgbox = (MessageBoxAFunc)func->proc_address;
        
        int return_val = msgbox(hWnd, lpText, lpCaption, uType);
        
        result.type = TYPE_INT;
        result.int_val = return_val;
    } else {
        error("Unsupported external function call: %s with %d arguments", func_name, arg_count);
    }
    
    return result;
}
#endif

// Main interpreter function
void interpret(const char* source) {
    init_interpreter(source);
    
    while (interpreter.current_token.type != TOKEN_EOF) {
        if (interpreter.current_token.type == TOKEN_FUNCTION) {
            parse_function();
        } else {
            ASTNode* stmt = parse_statement();
            execute_statement(stmt);
        }
    }
}

// File reading utility
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer and read file
    char* content = malloc(length + 1);
    if (!content) {
        printf("Error: Could not allocate memory for file '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    fread(content, 1, length, file);
    content[length] = '\0';
    fclose(file);
    
    return content;
}

// Print usage information
void print_usage(const char* program_name) {
    printf("Mini Script Language Interpreter\n");
    printf("=================================\n\n");
    printf("Usage: %s [script_file]\n\n", program_name);
    printf("Arguments:\n");
    printf("  script_file    Path to the Mini Script file to execute (.ms extension recommended)\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -examples      Run built-in example demonstrations\n\n");
    printf("Examples:\n");
    printf("  %s script.ms        # Run script.ms\n", program_name);
    printf("  %s example.ms       # Run example.ms\n", program_name);
    printf("  %s dll_demo.ms      # Run DLL demonstration\n", program_name);
    printf("  %s -examples        # Run built-in examples\n", program_name);
    printf("\n");
    printf("File Extensions:\n");
    printf("  .ms     Mini Script files\n");
    printf("  .txt    Plain text script files\n");
    printf("\n");
}

// Run built-in examples
void run_examples() {
    printf("Mini Script Language - Built-in Examples\n");
    printf("=========================================\n\n");
    
    // Example 1: Basic arithmetic and variables
    printf("Example 1: Basic arithmetic and variables\n");
    printf("------------------------------------------\n");
    const char* prog1 = 
        "x = 10;\n"
        "y = 20;\n"
        "z = x + y;\n"
        "print(\"Variables: x =\", x, \", y =\", y);\n"
        "print(\"Sum:\", z);\n"
        "print(\"Product:\", x * y);\n"
        "print(\"Division:\", y / 4.0);\n";
    
    interpret(prog1);
    printf("\n");
    
    // Example 2: Control flow
    printf("Example 2: Control flow\n");
    printf("-----------------------\n");
    const char* prog2 = 
        "n = 15;\n"
        "print(\"Testing number:\", n);\n"
        "if (n > 10) {\n"
        "    print(\"Number is greater than 10\");\n"
        "    if (n > 20) {\n"
        "        print(\"Number is also greater than 20\");\n"
        "    } else {\n"
        "        print(\"Number is between 10 and 20\");\n"
        "    }\n"
        "} else {\n"
        "    print(\"Number is 10 or less\");\n"
        "}\n";
    
    interpret(prog2);
    printf("\n");
    
    // Example 3: Loops
    printf("Example 3: Loops\n");
    printf("----------------\n");
    const char* prog3 = 
        "print(\"While loop countdown:\");\n"
        "i = 5;\n"
        "while (i > 0) {\n"
        "    print(\"Countdown:\", i);\n"
        "    i = i - 1;\n"
        "}\n"
        "print(\"Blast off!\");\n"
        "\n"
        "print(\"For loop (squares):\");\n"
        "for (j = 1; j <= 5; j = j + 1) {\n"
        "    square = j * j;\n"
        "    print(j, \"squared is\", square);\n"
        "}\n";
    
    interpret(prog3);
    printf("\n");
    
    // Example 4: Lists
    printf("Example 4: Lists\n");
    printf("----------------\n");
    const char* prog4 = 
        "numbers = [1, 2, 3, 4, 5];\n"
        "print(\"Numbers list:\", numbers);\n"
        "print(\"List length:\", len(numbers));\n"
        "\n"
        "mixed = [42, \"hello\", 3.14, true];\n"
        "print(\"Mixed types:\", mixed);\n"
        "print(\"Mixed length:\", len(mixed));\n";
    
    interpret(prog4);
    printf("\n");
    
    // Example 5: Strings
    printf("Example 5: Strings\n");
    printf("------------------\n");
    const char* prog5 = 
        "greeting = \"Hello\";\n"
        "target = \"Mini Script\";\n"
        "exclamation = \"!\";\n"
        "message = greeting + \" \" + target + exclamation;\n"
        "print(\"Message:\", message);\n"
        "print(\"Length:\", len(message));\n"
        "\n"
        "name = \"Alice\";\n"
        "age = 30;\n"
        "intro = \"My name is \" + name;\n"
        "print(intro);\n";
    
    interpret(prog5);
    printf("\n");
    
    // Example 6: Boolean logic
    printf("Example 6: Boolean logic\n");
    printf("------------------------\n");
    const char* prog6 =
        "a = true;\n"
        "b = false;\n"
        "print(\"a =\", a, \", b =\", b);\n"
        "print(\"a && b =\", a && b);\n"
        "print(\"a || b =\", a || b);\n"
        "print(\"!a =\", !a);\n"
        "\n"
        "x = 5;\n"
        "y = 10;\n"
        "print(\"x =\", x, \", y =\", y);\n"
        "print(\"x < y =\", x < y);\n"
        "print(\"x == y =\", x == y);\n"
        "print(\"x != y =\", x != y);\n";
    
    interpret(prog6);
    printf("\n");
    
    // Example 7: Complex expressions
    printf("Example 7: Complex expressions\n");
    printf("------------------------------\n");
    const char* prog7 =
        "print(\"Mathematical expressions:\");\n"
        "result1 = 2 + 3 * 4;\n"
        "result2 = (2 + 3) * 4;\n"
        "print(\"2 + 3 * 4 =\", result1);\n"
        "print(\"(2 + 3) * 4 =\", result2);\n"
        "\n"
        "print(\"Logical expressions:\");\n"
        "score = 85;\n"
        "passed = score >= 60;\n"
        "excellent = score >= 90;\n"
        "print(\"Score:\", score);\n"
        "print(\"Passed:\", passed);\n"
        "print(\"Excellent:\", excellent);\n"
        "print(\"Good but not excellent:\", passed && !excellent);\n";
    
    interpret(prog7);
    printf("\n");
    
    printf("=================================\n");
    printf("Built-in examples completed!\n");
    printf("=================================\n");
}

// Main function
int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Handle help arguments
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    
    // Handle examples argument
    if (strcmp(argv[1], "-examples") == 0) {
        run_examples();
        return 0;
    }
    
    // Read and execute script file
    char* script_content = read_file(argv[1]);
    if (!script_content) {
        return 1;
    }
    
    printf("Mini Script Language Interpreter\n");
    printf("=================================\n");
    printf("Executing: %s\n", argv[1]);
    printf("---------------------------------\n\n");
    
    // Execute the script
    interpret(script_content);
    
    // Clean up
    free(script_content);
    
    return 0;
}
