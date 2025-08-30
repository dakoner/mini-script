#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
// Windows doesn't have strptime, provide a simple implementation
char* strptime_simple(const char* s, const char* format, struct tm* tm) {
    // Simple implementation for basic formats
    if (strcmp(format, "%Y-%m-%d") == 0) {
        if (sscanf(s, "%d-%d-%d", &tm->tm_year, &tm->tm_mon, &tm->tm_mday) == 3) {
            tm->tm_year -= 1900;
            tm->tm_mon -= 1;
            return (char*)s + strlen(s);
        }
    } else if (strcmp(format, "%Y-%m-%d %H:%M:%S") == 0) {
        if (sscanf(s, "%d-%d-%d %d:%d:%d", 
                   &tm->tm_year, &tm->tm_mon, &tm->tm_mday,
                   &tm->tm_hour, &tm->tm_min, &tm->tm_sec) == 6) {
            tm->tm_year -= 1900;
            tm->tm_mon -= 1;
            return (char*)s + strlen(s);
        }
    }
    return NULL;
}
#define strptime strptime_simple
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
    TOKEN_CALLEXT,
    TOKEN_IMPORT
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
    TYPE_FILE_HANDLE,
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
        FILE* file_handle;
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
    char* body_source;
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
    NODE_IMPORT,
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
void execute_import(const char* module_path);
void execute_module_content(const char* content);
Value call_user_function(Function* func, ASTNode** args, int arg_count);
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
        } else if (strcmp(interpreter.current_token.lexeme, "import") == 0) {
            interpreter.current_token.type = TOKEN_IMPORT;
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
    
    if (interpreter.current_token.type == TOKEN_IMPORT) {
        ASTNode* node = create_node(NODE_IMPORT);
        next_token();
        
        if (interpreter.current_token.type != TOKEN_STRING) {
            error("Expected string literal after 'import'");
        }
        
        // Store the module name/path in the identifier field
        strcpy(node->identifier, interpreter.current_token.lexeme);
        next_token();
        
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
    
    // Capture function body source
    if (interpreter.current_token.type != TOKEN_LBRACE) {
        error("Expected '{' to start function body");
    }
    
    // Skip the opening brace
    interpreter.pos++;
    int start_pos = interpreter.pos;
    int brace_count = 1; // We already counted the opening brace
    
    // Find the matching closing brace
    while (brace_count > 0 && interpreter.pos < strlen(interpreter.source)) {
        if (interpreter.source[interpreter.pos] == '{') {
            brace_count++;
        } else if (interpreter.source[interpreter.pos] == '}') {
            brace_count--;
        }
        interpreter.pos++;
    }
    
    // Don't include the closing brace
    int body_length = interpreter.pos - start_pos - 1;
    func->body_source = malloc(body_length + 1);
    strncpy(func->body_source, &interpreter.source[start_pos], body_length);
    func->body_source[body_length] = '\0';
    
    // Update token to the position after the function body
    next_token();
    
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
                } else if (strcmp(node->identifier, "fopen") == 0) {
                    if (node->arg_count < 1 || node->arg_count > 2) {
                        error("fopen() expects 1 or 2 arguments (filename, mode)");
                    }
                    Value filename_arg = evaluate(node->args[0]);
                    if (filename_arg.type != TYPE_STRING) {
                        error("fopen() expects filename as string");
                    }
                    
                    char* mode = "r"; // default mode
                    if (node->arg_count == 2) {
                        Value mode_arg = evaluate(node->args[1]);
                        if (mode_arg.type != TYPE_STRING) {
                            error("fopen() expects mode as string");
                        }
                        mode = mode_arg.string_val;
                    }
                    
                    FILE* file = fopen(filename_arg.string_val, mode);
                    result.type = TYPE_FILE_HANDLE;
                    result.file_handle = file;
                    
                } else if (strcmp(node->identifier, "fclose") == 0) {
                    if (node->arg_count != 1) {
                        error("fclose() expects 1 argument (file_handle)");
                    }
                    Value handle_arg = evaluate(node->args[0]);
                    if (handle_arg.type != TYPE_FILE_HANDLE) {
                        error("fclose() expects a file handle");
                    }
                    
                    int close_result = 0;
                    if (handle_arg.file_handle != NULL) {
                        close_result = fclose(handle_arg.file_handle);
                    }
                    result.type = TYPE_INT;
                    result.int_val = close_result;
                    
                } else if (strcmp(node->identifier, "fread") == 0) {
                    if (node->arg_count != 1) {
                        error("fread() expects 1 argument (file_handle)");
                    }
                    Value handle_arg = evaluate(node->args[0]);
                    if (handle_arg.type != TYPE_FILE_HANDLE) {
                        error("fread() expects a file handle");
                    }
                    
                    if (handle_arg.file_handle == NULL) {
                        error("Invalid file handle");
                    }
                    
                    // Get file size
                    fseek(handle_arg.file_handle, 0, SEEK_END);
                    long file_size = ftell(handle_arg.file_handle);
                    fseek(handle_arg.file_handle, 0, SEEK_SET);
                    
                    // Allocate buffer and read
                    char* buffer = malloc(file_size + 1);
                    size_t bytes_read = fread(buffer, 1, file_size, handle_arg.file_handle);
                    buffer[bytes_read] = '\0';
                    
                    result.type = TYPE_STRING;
                    result.string_val = buffer;
                    
                } else if (strcmp(node->identifier, "fwrite") == 0) {
                    if (node->arg_count != 2) {
                        error("fwrite() expects 2 arguments (file_handle, content)");
                    }
                    Value handle_arg = evaluate(node->args[0]);
                    Value content_arg = evaluate(node->args[1]);
                    
                    if (handle_arg.type != TYPE_FILE_HANDLE) {
                        error("fwrite() expects a file handle as first argument");
                    }
                    if (content_arg.type != TYPE_STRING) {
                        error("fwrite() expects content as string");
                    }
                    
                    if (handle_arg.file_handle == NULL) {
                        error("Invalid file handle");
                    }
                    
                    size_t bytes_written = fwrite(content_arg.string_val, 1, 
                                                strlen(content_arg.string_val), 
                                                handle_arg.file_handle);
                    result.type = TYPE_INT;
                    result.int_val = (int)bytes_written;
                    
                } else if (strcmp(node->identifier, "fexists") == 0) {
                    if (node->arg_count != 1) {
                        error("fexists() expects 1 argument (filename)");
                    }
                    Value filename_arg = evaluate(node->args[0]);
                    if (filename_arg.type != TYPE_STRING) {
                        error("fexists() expects filename as string");
                    }
                    
                    FILE* test_file = fopen(filename_arg.string_val, "r");
                    result.type = TYPE_BOOL;
                    result.bool_val = (test_file != NULL);
                    if (test_file != NULL) {
                        fclose(test_file);
                    }
                    
                } else if (strcmp(node->identifier, "freadline") == 0) {
                    if (node->arg_count != 1) {
                        error("freadline() expects 1 argument (file_handle)");
                    }
                    Value handle_arg = evaluate(node->args[0]);
                    if (handle_arg.type != TYPE_FILE_HANDLE) {
                        error("freadline() expects a file handle");
                    }
                    
                    if (handle_arg.file_handle == NULL) {
                        error("Invalid file handle");
                    }
                    
                    char buffer[1024];
                    if (fgets(buffer, sizeof(buffer), handle_arg.file_handle) != NULL) {
                        // Remove trailing newline if present
                        size_t len = strlen(buffer);
                        if (len > 0 && buffer[len-1] == '\n') {
                            buffer[len-1] = '\0';
                        }
                        
                        char* line = malloc(strlen(buffer) + 1);
                        strcpy(line, buffer);
                        result.type = TYPE_STRING;
                        result.string_val = line;
                    } else {
                        result.type = TYPE_STRING;
                        result.string_val = malloc(1);
                        result.string_val[0] = '\0';
                    }
                    
                } else if (strcmp(node->identifier, "fwriteline") == 0) {
                    if (node->arg_count != 2) {
                        error("fwriteline() expects 2 arguments (file_handle, line)");
                    }
                    Value handle_arg = evaluate(node->args[0]);
                    Value line_arg = evaluate(node->args[1]);
                    
                    if (handle_arg.type != TYPE_FILE_HANDLE) {
                        error("fwriteline() expects a file handle as first argument");
                    }
                    if (line_arg.type != TYPE_STRING) {
                        error("fwriteline() expects line as string");
                    }
                    
                    if (handle_arg.file_handle == NULL) {
                        error("Invalid file handle");
                    }
                    
                    int chars_written = fprintf(handle_arg.file_handle, "%s\n", line_arg.string_val);
                    result.type = TYPE_INT;
                    result.int_val = chars_written;
                    
                } else if (strcmp(node->identifier, "fsize") == 0) {
                    if (node->arg_count != 1) {
                        error("fsize() expects 1 argument (filename)");
                    }
                    Value filename_arg = evaluate(node->args[0]);
                    if (filename_arg.type != TYPE_STRING) {
                        error("fsize() expects filename as string");
                    }
                    
                    FILE* file = fopen(filename_arg.string_val, "r");
                    long file_size = 0;
                    if (file != NULL) {
                        fseek(file, 0, SEEK_END);
                        file_size = ftell(file);
                        fclose(file);
                    }
                    result.type = TYPE_INT;
                    result.int_val = (int)file_size;
                    
                } else if (strcmp(node->identifier, "time_now") == 0) {
                    if (node->arg_count != 0) {
                        error("time_now() expects no arguments");
                    }
                    
                    time_t current_time = time(NULL);
                    result.type = TYPE_INT;
                    result.int_val = (int)current_time;
                    
                } else if (strcmp(node->identifier, "time_format") == 0) {
                    if (node->arg_count < 1 || node->arg_count > 2) {
                        error("time_format() expects 1 or 2 arguments (timestamp, format)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_format() expects timestamp as integer");
                    }
                    
                    char* format = "%Y-%m-%d %H:%M:%S"; // default format
                    if (node->arg_count == 2) {
                        Value format_arg = evaluate(node->args[1]);
                        if (format_arg.type != TYPE_STRING) {
                            error("time_format() expects format as string");
                        }
                        format = format_arg.string_val;
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    char* buffer = malloc(256);
                    strftime(buffer, 256, format, timeinfo);
                    
                    result.type = TYPE_STRING;
                    result.string_val = buffer;
                    
                } else if (strcmp(node->identifier, "time_parse") == 0) {
                    if (node->arg_count != 2) {
                        error("time_parse() expects 2 arguments (date_string, format)");
                    }
                    
                    Value date_str_arg = evaluate(node->args[0]);
                    Value format_arg = evaluate(node->args[1]);
                    
                    if (date_str_arg.type != TYPE_STRING || format_arg.type != TYPE_STRING) {
                        error("time_parse() expects string arguments");
                    }
                    
                    struct tm timeinfo = {0};
                    char* parse_result = strptime(date_str_arg.string_val, format_arg.string_val, &timeinfo);
                    
                    if (parse_result == NULL) {
                        result.type = TYPE_INT;
                        result.int_val = 0; // Failed to parse
                    } else {
                        time_t timestamp = mktime(&timeinfo);
                        result.type = TYPE_INT;
                        result.int_val = (int)timestamp;
                    }
                    
                } else if (strcmp(node->identifier, "time_year") == 0) {
                    if (node->arg_count != 1) {
                        error("time_year() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_year() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_year + 1900;
                    
                } else if (strcmp(node->identifier, "time_month") == 0) {
                    if (node->arg_count != 1) {
                        error("time_month() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_month() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_mon + 1; // 1-12 instead of 0-11
                    
                } else if (strcmp(node->identifier, "time_day") == 0) {
                    if (node->arg_count != 1) {
                        error("time_day() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_day() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_mday;
                    
                } else if (strcmp(node->identifier, "time_hour") == 0) {
                    if (node->arg_count != 1) {
                        error("time_hour() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_hour() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_hour;
                    
                } else if (strcmp(node->identifier, "time_minute") == 0) {
                    if (node->arg_count != 1) {
                        error("time_minute() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_minute() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_min;
                    
                } else if (strcmp(node->identifier, "time_second") == 0) {
                    if (node->arg_count != 1) {
                        error("time_second() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_second() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_sec;
                    
                } else if (strcmp(node->identifier, "time_weekday") == 0) {
                    if (node->arg_count != 1) {
                        error("time_weekday() expects 1 argument (timestamp)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    if (timestamp_arg.type != TYPE_INT) {
                        error("time_weekday() expects timestamp as integer");
                    }
                    
                    time_t timestamp = (time_t)timestamp_arg.int_val;
                    struct tm* timeinfo = localtime(&timestamp);
                    
                    result.type = TYPE_INT;
                    result.int_val = timeinfo->tm_wday; // 0=Sunday, 1=Monday, etc.
                    
                } else if (strcmp(node->identifier, "time_add") == 0) {
                    if (node->arg_count != 2) {
                        error("time_add() expects 2 arguments (timestamp, seconds)");
                    }
                    
                    Value timestamp_arg = evaluate(node->args[0]);
                    Value seconds_arg = evaluate(node->args[1]);
                    
                    if (timestamp_arg.type != TYPE_INT || seconds_arg.type != TYPE_INT) {
                        error("time_add() expects integer arguments");
                    }
                    
                    result.type = TYPE_INT;
                    result.int_val = timestamp_arg.int_val + seconds_arg.int_val;
                    
                } else if (strcmp(node->identifier, "time_diff") == 0) {
                    if (node->arg_count != 2) {
                        error("time_diff() expects 2 arguments (timestamp1, timestamp2)");
                    }
                    
                    Value time1_arg = evaluate(node->args[0]);
                    Value time2_arg = evaluate(node->args[1]);
                    
                    if (time1_arg.type != TYPE_INT || time2_arg.type != TYPE_INT) {
                        error("time_diff() expects integer arguments");
                    }
                    
                    result.type = TYPE_INT;
                    result.int_val = time1_arg.int_val - time2_arg.int_val;
                    
                } else if (strcmp(node->identifier, "sleep") == 0) {
                    if (node->arg_count != 1) {
                        error("sleep() expects 1 argument (seconds)");
                    }
                    
                    Value seconds_arg = evaluate(node->args[0]);
                    if (seconds_arg.type != TYPE_INT) {
                        error("sleep() expects seconds as integer");
                    }
                    
                    if (seconds_arg.int_val > 0) {
#ifdef _WIN32
                        Sleep(seconds_arg.int_val * 1000); // Windows Sleep takes milliseconds
#else
                        sleep(seconds_arg.int_val); // Unix sleep takes seconds
#endif
                    }
                    
                    result.type = TYPE_INT;
                    result.int_val = 0;
                    
                } else {
                    error("Unknown function: %s", node->identifier);
                }
            } else {
                // User-defined function call
                result = call_user_function(func, node->args, node->arg_count);
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
        
        case NODE_IMPORT: {
            execute_import(node->identifier);
            break;
        }
        
        default:
            // Expression statement
            evaluate(node);
            break;
    }
}

// Function to execute module content with minimal state interference
void execute_module_content(const char* content) {
    // Save only the essential parsing context
    char* original_source = interpreter.source;
    int original_pos = interpreter.pos;
    int original_line = interpreter.line;
    Token original_token = interpreter.current_token;
    
    // Backup current function and variable counts
    int original_func_count = interpreter.func_count;
    int original_var_count = interpreter.var_count;
    
    // Set up for module parsing - don't touch other interpreter state
    interpreter.source = (char*)content;
    interpreter.pos = 0;
    interpreter.line = 1;
    
    // Parse first token
    next_token();
    
    // Execute module statements
    while (interpreter.current_token.type != TOKEN_EOF) {
        if (interpreter.current_token.type == TOKEN_FUNCTION) {
            parse_function();
        } else {
            ASTNode* stmt = parse_statement();
            if (stmt) {
                execute_statement(stmt);
            }
        }
    }
    
    // Restore the parsing context exactly as it was
    interpreter.source = original_source;
    interpreter.pos = original_pos;
    interpreter.line = original_line;
    interpreter.current_token = original_token;
    
    // Note: Keep the functions and variables that were added during module execution
    // They remain in interpreter.functions[] and interpreter.variables[]
}

void execute_import(const char* module_path) {
    char full_path[1024];
    FILE* file = NULL;
    
    // First try the module path as-is (relative to current directory)
    strcpy(full_path, module_path);
    file = fopen(full_path, "r");
    
    if (!file) {
        // Try with .ms extension if not present
        if (strstr(module_path, ".ms") == NULL) {
            sprintf(full_path, "%s.ms", module_path);
            file = fopen(full_path, "r");
        }
    }
    
    // If still not found, try MODULESPATH environment variable
    if (!file) {
        char* modules_path = getenv("MODULESPATH");
        if (modules_path) {
            char* path_copy = malloc(strlen(modules_path) + 1);
            strcpy(path_copy, modules_path);
            
            char* path_token = strtok(path_copy, ";");
            while (path_token && !file) {
                // Try the exact module path
                sprintf(full_path, "%s\\%s", path_token, module_path);
                file = fopen(full_path, "r");
                
                // Try with .ms extension
                if (!file && strstr(module_path, ".ms") == NULL) {
                    sprintf(full_path, "%s\\%s.ms", path_token, module_path);
                    file = fopen(full_path, "r");
                }
                
                path_token = strtok(NULL, ";");
            }
            
            free(path_copy);
        }
    }
    
    if (!file) {
        error("Cannot find module: %s", module_path);
        return;
    }
    
    // Read the file content
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* module_source = malloc(file_size + 1);
    size_t bytes_read = fread(module_source, 1, file_size, file);
    module_source[file_size] = '\0';
    fclose(file);
    
    // Execute the module content
    execute_module_content(module_source);
    
    free(module_source);
}

// Function to execute function body without any parser state interference
void execute_function_body(const char* body_source) {
    // Create a temporary buffer for the complete function code
    size_t body_len = strlen(body_source);
    char* temp_code = malloc(body_len + 20); // Extra space for wrapping
    
    // We'll execute the body as a complete mini-program
    // Just execute the statements directly without changing main parser state
    strcpy(temp_code, body_source);
    
    // Store the current function execution context
    char* old_source = interpreter.source;
    int old_pos = interpreter.pos;
    int old_line = interpreter.line;
    Token old_token = interpreter.current_token;
    
    // Execute in a temporary context
    interpreter.source = temp_code;
    interpreter.pos = 0;
    interpreter.line = 1;
    next_token();
    
    // Execute statements until return or end
    while (interpreter.current_token.type != TOKEN_EOF && !interpreter.has_return) {
        ASTNode* stmt = parse_statement();
        if (stmt) {
            execute_statement(stmt);
        }
    }
    
    // Restore the exact parser state - this time with no token copying issues
    interpreter.source = old_source;
    interpreter.pos = old_pos;
    interpreter.line = old_line;
    interpreter.current_token = old_token;
    
    free(temp_code);
}

Value call_user_function(Function* func, ASTNode** args, int arg_count) {
    Value result;
    result.type = TYPE_INT;
    result.int_val = 0;
    
    // Check parameter count
    if (arg_count != func->param_count) {
        error("Function %s expects %d arguments, got %d", 
              func->name, func->param_count, arg_count);
    }
    
    // Save current variable state
    int old_var_count = interpreter.var_count;
    Variable old_vars[MAX_VARIABLES];
    for (int i = 0; i < interpreter.var_count; i++) {
        old_vars[i] = interpreter.variables[i];
    }
    
    // Push function parameters as local variables
    interpreter.stack_depth++;
    if (interpreter.stack_depth >= MAX_STACK_DEPTH) {
        error("Stack overflow");
    }
    interpreter.stack_vars[interpreter.stack_depth - 1] = 0;
    
    // Set up function parameters
    for (int i = 0; i < func->param_count; i++) {
        Value arg_value = evaluate(args[i]);
        
        // Add parameter as local variable
        int idx = interpreter.stack_vars[interpreter.stack_depth - 1]++;
        strcpy(interpreter.stack[interpreter.stack_depth - 1][idx].name, 
               func->params[i].name);
        interpreter.stack[interpreter.stack_depth - 1][idx].value = arg_value;
    }
    
    // Save function execution state
    int old_has_return = interpreter.has_return;
    Value old_return_value = interpreter.return_value;
    
    // Execute function body
    interpreter.has_return = 0;
    execute_function_body(func->body_source);
    
    // Get return value
    if (interpreter.has_return) {
        result = interpreter.return_value;
    }
    
    // Restore function execution state
    interpreter.has_return = old_has_return;
    interpreter.return_value = old_return_value;
    
    // Pop stack
    interpreter.stack_depth--;
    
    return result;
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
        case TYPE_FILE_HANDLE:
            printf("<file_handle:0x%p>", (void*)val->file_handle);
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
    printf("  -h, --help     Show this help message\n\n");
    printf("Examples:\n");
    printf("  %s script.ms             # Run script.ms\n", program_name);
    printf("  %s scripts/examples.ms   # Run all built-in examples\n", program_name);
    printf("  %s examples/1_arithmetic.ms  # Run arithmetic example\n", program_name);
    printf("  %s dll_demo.ms           # Run DLL demonstration\n", program_name);
    printf("\n");
    printf("Built-in Examples:\n");
    printf("  scripts/examples.ms                 # All examples\n");
    printf("  examples/1_arithmetic.ms            # Basic arithmetic\n");
    printf("  examples/2_control_flow.ms          # If/else statements\n");
    printf("  examples/3_loops.ms                 # While and for loops\n");
    printf("  examples/4_lists.ms                 # List operations\n");
    printf("  examples/5_strings.ms               # String manipulation\n");
    printf("  examples/6_boolean_logic.ms         # Boolean operations\n");
    printf("  examples/7_complex_expressions.ms   # Complex math\n");
    printf("\n");
    printf("File Extensions:\n");
    printf("  .ms     Mini Script files\n");
    printf("  .txt    Plain text script files\n");
    printf("\n");
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
