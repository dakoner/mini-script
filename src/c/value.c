#include "mini_script.h"

/* Value functions */
Value* value_new(ValueType type) {
    Value* value = malloc(sizeof(Value));
    value->type = type;
    memset(&value->as, 0, sizeof(value->as));
    return value;
}

void value_free(Value* value) {
    if (!value) return;
    
    switch (value->type) {
        case VALUE_STRING:
            free(value->as.string);
            break;
        case VALUE_LIST:
            if (value->as.list) {
                for (size_t i = 0; i < value->as.list->count; i++) {
                    value_free(&value->as.list->elements[i]);
                }
                free(value->as.list->elements);
                free(value->as.list);
            }
            break;
        case VALUE_FUNCTION:
            if (value->as.function) {
                stmt_free(value->as.function->declaration);
                environment_free(value->as.function->closure);
                free(value->as.function);
            }
            break;
        case VALUE_BUILTIN:
            free(value->as.builtin_name);
            break;
        case VALUE_FILE_HANDLE:
            if (value->as.file_handle) {
                fclose(value->as.file_handle);
            }
            break;
        default:
            break;
    }
    free(value);
}

Value* value_copy(Value* value) {
    if (!value) return NULL;
    
    Value* copy = value_new(value->type);
    
    switch (value->type) {
        case VALUE_NIL:
            break;
        case VALUE_BOOLEAN:
            copy->as.boolean = value->as.boolean;
            break;
        case VALUE_NUMBER:
            copy->as.number = value->as.number;
            break;
        case VALUE_STRING:
            copy->as.string = malloc(strlen(value->as.string) + 1);
            strcpy(copy->as.string, value->as.string);
            break;
        case VALUE_LIST:
            copy->as.list = malloc(sizeof(ValueList));
            copy->as.list->capacity = value->as.list->capacity;
            copy->as.list->count = value->as.list->count;
            copy->as.list->elements = malloc(copy->as.list->capacity * sizeof(Value));
            for (size_t i = 0; i < copy->as.list->count; i++) {
                copy->as.list->elements[i] = *value_copy(&value->as.list->elements[i]);
            }
            break;
        case VALUE_FUNCTION:
            copy->as.function = malloc(sizeof(MiniScriptFunction));
            copy->as.function->declaration = value->as.function->declaration; // Shallow copy
            copy->as.function->closure = value->as.function->closure; // Shallow copy
            break;
        case VALUE_BUILTIN:
            copy->as.builtin_name = malloc(strlen(value->as.builtin_name) + 1);
            strcpy(copy->as.builtin_name, value->as.builtin_name);
            break;
        case VALUE_FILE_HANDLE:
            copy->as.file_handle = value->as.file_handle; // Shallow copy
            break;
    }
    
    return copy;
}

/* Token functions */
Token* token_new(TokenType type, const char* lexeme, LiteralValue* literal, size_t line) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->lexeme = malloc(strlen(lexeme) + 1);
    strcpy(token->lexeme, lexeme);
    token->literal = literal;
    token->line = line;
    return token;
}

void token_free(Token* token) {
    if (token) {
        free(token->lexeme);
        literal_free(token->literal);
        free(token);
    }
}

/* Statement functions */
Stmt* stmt_new(StmtType type) {
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = type;
    memset(&stmt->as, 0, sizeof(stmt->as));
    return stmt;
}

void stmt_free(Stmt* stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case STMT_BLOCK:
            for (size_t i = 0; i < stmt->as.block.statements.count; i++) {
                stmt_free(&stmt->as.block.statements.statements[i]);
            }
            free(stmt->as.block.statements.statements);
            break;
        case STMT_EXPRESSION:
            expr_free(stmt->as.expression.expression);
            break;
        case STMT_PRINT:
            for (size_t i = 0; i < stmt->as.print.count; i++) {
                expr_free(stmt->as.print.expressions[i]);
            }
            free(stmt->as.print.expressions);
            break;
        case STMT_FUNCTION:
            free(stmt->as.function.name.lexeme);
            for (size_t i = 0; i < stmt->as.function.param_count; i++) {
                free(stmt->as.function.params[i].lexeme);
            }
            free(stmt->as.function.params);
            for (size_t i = 0; i < stmt->as.function.body.count; i++) {
                stmt_free(&stmt->as.function.body.statements[i]);
            }
            free(stmt->as.function.body.statements);
            break;
        case STMT_IF:
            expr_free(stmt->as.if_stmt.condition);
            stmt_free(stmt->as.if_stmt.then_branch);
            stmt_free(stmt->as.if_stmt.else_branch);
            break;
        case STMT_RETURN:
            free(stmt->as.return_stmt.keyword.lexeme);
            expr_free(stmt->as.return_stmt.value);
            break;
        case STMT_WHILE:
            expr_free(stmt->as.while_stmt.condition);
            stmt_free(stmt->as.while_stmt.body);
            break;
        case STMT_IMPORT:
            free(stmt->as.import.path_token.lexeme);
            if (stmt->as.import.namespace_token) {
                free(stmt->as.import.namespace_token->lexeme);
                free(stmt->as.import.namespace_token);
            }
            break;
        case STMT_ASSERT:
            free(stmt->as.assert_stmt.keyword.lexeme);
            expr_free(stmt->as.assert_stmt.condition);
            expr_free(stmt->as.assert_stmt.message);
            break;
        case STMT_VAR:
            free(stmt->as.var.name.lexeme);
            expr_free(stmt->as.var.initializer);
            break;
    }
    free(stmt);
}

/* Expression functions */
Expr* expr_new(ExprType type) {
    Expr* expr = malloc(sizeof(Expr));
    expr->type = type;
    memset(&expr->as, 0, sizeof(expr->as));
    return expr;
}

void expr_free(Expr* expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case EXPR_ASSIGN:
            free(expr->as.assign.name.lexeme);
            expr_free(expr->as.assign.value);
            break;
        case EXPR_BINARY:
            expr_free(expr->as.binary.left);
            free(expr->as.binary.op.lexeme);
            expr_free(expr->as.binary.right);
            break;
        case EXPR_CALL:
            expr_free(expr->as.call.callee);
            free(expr->as.call.paren.lexeme);
            for (size_t i = 0; i < expr->as.call.arguments.count; i++) {
                expr_free(&expr->as.call.arguments.expressions[i]);
            }
            free(expr->as.call.arguments.expressions);
            break;
        case EXPR_GROUPING:
            expr_free(expr->as.grouping.expression);
            break;
        case EXPR_LITERAL:
            literal_free(&expr->as.literal.value);
            break;
        case EXPR_LIST_LITERAL:
            for (size_t i = 0; i < expr->as.list_literal.elements.count; i++) {
                expr_free(&expr->as.list_literal.elements.expressions[i]);
            }
            free(expr->as.list_literal.elements.expressions);
            break;
        case EXPR_GET:
            expr_free(expr->as.get.object);
            expr_free(expr->as.get.index);
            break;
        case EXPR_SET:
            expr_free(expr->as.set.object);
            expr_free(expr->as.set.index);
            expr_free(expr->as.set.value);
            break;
        case EXPR_LOGICAL:
            expr_free(expr->as.logical.left);
            free(expr->as.logical.op.lexeme);
            expr_free(expr->as.logical.right);
            break;
        case EXPR_UNARY:
            free(expr->as.unary.op.lexeme);
            expr_free(expr->as.unary.right);
            break;
        case EXPR_VARIABLE:
            free(expr->as.variable.name.lexeme);
            break;
    }
    free(expr);
}

/* Utility functions */
char* stringify_value(Value* value) {
    if (!value) return strdup("nil");
    
    char* result;
    char buffer[256];
    
    switch (value->type) {
        case VALUE_NIL:
            return strdup("nil");
        case VALUE_BOOLEAN:
            return strdup(value->as.boolean ? "true" : "false");
        case VALUE_NUMBER:
            snprintf(buffer, sizeof(buffer), "%.6g", value->as.number);
            return strdup(buffer);
        case VALUE_STRING:
            return strdup(value->as.string);
        case VALUE_LIST:
            // Simple list representation
            return strdup("[list]");
        case VALUE_FUNCTION:
            return strdup("<function>");
        case VALUE_BUILTIN:
            snprintf(buffer, sizeof(buffer), "<builtin %s>", value->as.builtin_name);
            return strdup(buffer);
        case VALUE_FILE_HANDLE:
            return strdup("<file>");
        default:
            return strdup("unknown");
    }
}

bool is_truthy(Value* value) {
    if (!value) return false;
    
    switch (value->type) {
        case VALUE_NIL:
            return false;
        case VALUE_BOOLEAN:
            return value->as.boolean;
        default:
            return true;
    }
}

bool values_equal(Value* a, Value* b) {
    if (!a || !b) return a == b;
    
    if (a->type != b->type) return false;
    
    switch (a->type) {
        case VALUE_NIL:
            return true;
        case VALUE_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        case VALUE_NUMBER:
            return a->as.number == b->as.number;
        case VALUE_STRING:
            return strcmp(a->as.string, b->as.string) == 0;
        case VALUE_FILE_HANDLE:
            return false; // File handles are never equal
        default:
            return false;
    }
}

/* Runtime error functions */
RuntimeError* runtime_error_new(const char* message, size_t line, const char* filename) {
    RuntimeError* error = malloc(sizeof(RuntimeError));
    error->message = malloc(strlen(message) + 1);
    strcpy(error->message, message);
    error->line = line;
    error->filename = malloc(strlen(filename) + 1);
    strcpy(error->filename, filename);
    error->return_value = NULL;
    return error;
}

RuntimeError* runtime_error_with_return(const char* message, size_t line, const char* filename, Value* return_value) {
    RuntimeError* error = runtime_error_new(message, line, filename);
    error->return_value = return_value;
    return error;
}

void runtime_error_free(RuntimeError* error) {
    if (error) {
        free(error->message);
        free(error->filename);
        if (error->return_value) {
            value_free(error->return_value);
        }
        free(error);
    }
}
