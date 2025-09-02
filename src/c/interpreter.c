#include "mini_script.h"
#include <math.h>

/* Builtin function implementations */
static Value* builtin_print(Interpreter* interpreter, Value** args, int arg_count) {
    for (int i = 0; i < arg_count; i++) {
        char* str = stringify_value(args[i]);
        printf("%s", str);
        if (i < arg_count - 1) printf(" ");
        free(str);
    }
    printf("\n");
    
    Value* result = value_new(VALUE_NIL);
    return result;
}

static Value* builtin_len(Interpreter* interpreter, Value** args, int arg_count) {
    if (arg_count != 1) {
        return NULL; // Error
    }
    
    Value* result = value_new(VALUE_NUMBER);
    
    switch (args[0]->type) {
        case VALUE_STRING:
            result->as.number = strlen(args[0]->as.string);
            break;
        case VALUE_LIST:
            result->as.number = args[0]->as.list->count;
            break;
        default:
            value_free(result);
            return NULL; // Error
    }
    
    return result;
}

static Value* builtin_time_now(Interpreter* interpreter, Value** args, int arg_count) {
    if (arg_count != 0) return NULL;
    
    Value* result = value_new(VALUE_NUMBER);
    result->as.number = (double)time(NULL);
    return result;
}

static Value* call_builtin_function(Interpreter* interpreter, const char* name, Value** args, int arg_count) {
    if (strcmp(name, "print") == 0) {
        return builtin_print(interpreter, args, arg_count);
    } else if (strcmp(name, "len") == 0) {
        return builtin_len(interpreter, args, arg_count);
    } else if (strcmp(name, "time_now") == 0) {
        return builtin_time_now(interpreter, args, arg_count);
    }
    
    return NULL; // Unknown builtin
}

/* Interpreter implementation */
Interpreter* interpreter_new(void) {
    Interpreter* interpreter = malloc(sizeof(Interpreter));
    interpreter->globals = environment_new(NULL);
    interpreter->environment = interpreter->globals;
    interpreter->modules_path = NULL;
    interpreter->modules_path_count = 0;
    
    interpreter_define_builtins(interpreter);
    
    return interpreter;
}

void interpreter_free(Interpreter* interpreter) {
    if (interpreter) {
        environment_free(interpreter->globals);
        for (size_t i = 0; i < interpreter->modules_path_count; i++) {
            free(interpreter->modules_path[i]);
        }
        free(interpreter->modules_path);
        free(interpreter);
    }
}

void interpreter_define_builtins(Interpreter* interpreter) {
    Value* print_builtin = value_new(VALUE_BUILTIN);
    print_builtin->as.builtin_name = strdup("print");
    environment_define(interpreter->globals, "print", print_builtin);
    
    Value* len_builtin = value_new(VALUE_BUILTIN);
    len_builtin->as.builtin_name = strdup("len");
    environment_define(interpreter->globals, "len", len_builtin);
    
    Value* time_now_builtin = value_new(VALUE_BUILTIN);
    time_now_builtin->as.builtin_name = strdup("time_now");
    environment_define(interpreter->globals, "time_now", time_now_builtin);
}

Value* interpreter_evaluate(Interpreter* interpreter, Expr* expr, RuntimeError** error) {
    if (!expr) {
        *error = runtime_error_new("Null expression", 0, "<interpreter>");
        return NULL;
    }
    
    switch (expr->type) {
        case EXPR_LITERAL: {
            Value* value = value_new(VALUE_NIL);
            switch (expr->as.literal.value.type) {
                case LITERAL_NIL:
                    break;
                case LITERAL_BOOLEAN:
                    value->type = VALUE_BOOLEAN;
                    value->as.boolean = expr->as.literal.value.value.boolean;
                    break;
                case LITERAL_NUMBER:
                    value->type = VALUE_NUMBER;
                    value->as.number = expr->as.literal.value.value.number;
                    break;
                case LITERAL_INTEGER:
                    value->type = VALUE_NUMBER;
                    value->as.number = (double)expr->as.literal.value.value.integer;
                    break;
                case LITERAL_STRING:
                    value->type = VALUE_STRING;
                    value->as.string = strdup(expr->as.literal.value.value.string);
                    break;
                case LITERAL_CHAR:
                    value->type = VALUE_STRING;
                    value->as.string = malloc(2);
                    value->as.string[0] = expr->as.literal.value.value.character;
                    value->as.string[1] = '\0';
                    break;
            }
            return value;
        }
        
        case EXPR_VARIABLE: {
            Value* stored_value = environment_get(interpreter->environment, &expr->as.variable.name, error);
            if (*error) return NULL;
            
            // Return a copy of the stored value
            return value_copy(stored_value);
        }
        
        case EXPR_ASSIGN: {
            Value* value = interpreter_evaluate(interpreter, expr->as.assign.value, error);
            if (*error) return NULL;
            
            Value* value_copy = value_new(value->type);
            *value_copy = *value;
            // Shallow copy for now
            
            environment_assign(interpreter->environment, &expr->as.assign.name, value_copy, error);
            if (*error) {
                value_free(value);
                return NULL;
            }
            
            return value;
        }
        
        case EXPR_BINARY: {
            Value* left = interpreter_evaluate(interpreter, expr->as.binary.left, error);
            if (*error) return NULL;
            
            Value* right = interpreter_evaluate(interpreter, expr->as.binary.right, error);
            if (*error) {
                value_free(left);
                return NULL;
            }
            
            Value* result = value_new(VALUE_NIL);
            
            switch (expr->as.binary.op.type) {
                case PLUS:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_NUMBER;
                        result->as.number = left->as.number + right->as.number;
                    } else if (left->type == VALUE_STRING && right->type == VALUE_STRING) {
                        result->type = VALUE_STRING;
                        size_t len = strlen(left->as.string) + strlen(right->as.string) + 1;
                        result->as.string = malloc(len);
                        strcpy(result->as.string, left->as.string);
                        strcat(result->as.string, right->as.string);
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be two numbers or two strings.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case MINUS:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_NUMBER;
                        result->as.number = left->as.number - right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case MULTIPLY:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_NUMBER;
                        result->as.number = left->as.number * right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case DIVIDE:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_NUMBER;
                        result->as.number = left->as.number / right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case GREATER:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_BOOLEAN;
                        result->as.boolean = left->as.number > right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case GREATER_EQUAL:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_BOOLEAN;
                        result->as.boolean = left->as.number >= right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case LESS:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_BOOLEAN;
                        result->as.boolean = left->as.number < right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case LESS_EQUAL:
                    if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
                        result->type = VALUE_BOOLEAN;
                        result->as.boolean = left->as.number <= right->as.number;
                    } else {
                        value_free(result);
                        value_free(left);
                        value_free(right);
                        *error = runtime_error_new("Operands must be numbers.", 
                                                 expr->as.binary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case EQUAL:
                    result->type = VALUE_BOOLEAN;
                    result->as.boolean = values_equal(left, right);
                    break;
                case NOT_EQUAL:
                    result->type = VALUE_BOOLEAN;
                    result->as.boolean = !values_equal(left, right);
                    break;
                default:
                    value_free(result);
                    value_free(left);
                    value_free(right);
                    *error = runtime_error_new("Unknown binary operator.", 
                                             expr->as.binary.op.line, "<interpreter>");
                    return NULL;
            }
            
            value_free(left);
            value_free(right);
            return result;
        }
        
        case EXPR_UNARY: {
            Value* right = interpreter_evaluate(interpreter, expr->as.unary.right, error);
            if (*error) return NULL;
            
            Value* result = value_new(VALUE_NIL);
            
            switch (expr->as.unary.op.type) {
                case MINUS:
                    if (right->type == VALUE_NUMBER) {
                        result->type = VALUE_NUMBER;
                        result->as.number = -right->as.number;
                    } else {
                        value_free(result);
                        value_free(right);
                        *error = runtime_error_new("Operand must be a number.", 
                                                 expr->as.unary.op.line, "<interpreter>");
                        return NULL;
                    }
                    break;
                case NOT:
                    result->type = VALUE_BOOLEAN;
                    result->as.boolean = !is_truthy(right);
                    break;
                default:
                    value_free(result);
                    value_free(right);
                    *error = runtime_error_new("Unknown unary operator.", 
                                             expr->as.unary.op.line, "<interpreter>");
                    return NULL;
            }
            
            value_free(right);
            return result;
        }
        
        case EXPR_CALL: {
            Value* callee = interpreter_evaluate(interpreter, expr->as.call.callee, error);
            if (*error) return NULL;
            
            // Evaluate arguments
            Value** arguments = malloc(expr->as.call.arguments.count * sizeof(Value*));
            for (size_t i = 0; i < expr->as.call.arguments.count; i++) {
                arguments[i] = interpreter_evaluate(interpreter, &expr->as.call.arguments.expressions[i], error);
                if (*error) {
                    // Clean up already evaluated arguments
                    for (size_t j = 0; j < i; j++) {
                        value_free(arguments[j]);
                    }
                    free(arguments);
                    value_free(callee);
                    return NULL;
                }
            }
            
            Value* result = NULL;
            
            if (callee->type == VALUE_BUILTIN) {
                result = call_builtin_function(interpreter, callee->as.builtin_name, 
                                              arguments, expr->as.call.arguments.count);
                if (!result) {
                    *error = runtime_error_new("Error calling builtin function.", 
                                             expr->as.call.paren.line, "<interpreter>");
                }
            } else {
                *error = runtime_error_new("Can only call functions and classes.", 
                                         expr->as.call.paren.line, "<interpreter>");
            }
            
            // Clean up
            for (size_t i = 0; i < expr->as.call.arguments.count; i++) {
                value_free(arguments[i]);
            }
            free(arguments);
            value_free(callee);
            
            return result;
        }
        
        case EXPR_GROUPING: {
            return interpreter_evaluate(interpreter, expr->as.grouping.expression, error);
        }
        
        case EXPR_LIST_LITERAL: {
            Value* list_value = value_new(VALUE_LIST);
            list_value->as.list = malloc(sizeof(ValueList));
            list_value->as.list->capacity = expr->as.list_literal.elements.count;
            list_value->as.list->count = 0;
            list_value->as.list->elements = malloc(list_value->as.list->capacity * sizeof(Value));
            
            for (size_t i = 0; i < expr->as.list_literal.elements.count; i++) {
                Value* element = interpreter_evaluate(interpreter, &expr->as.list_literal.elements.expressions[i], error);
                if (*error) {
                    value_free(list_value);
                    return NULL;
                }
                list_value->as.list->elements[list_value->as.list->count++] = *element;
                free(element); // Transfer ownership
            }
            
            return list_value;
        }
        
        default:
            *error = runtime_error_new("Unknown expression type.", 0, "<interpreter>");
            return NULL;
    }
}

void interpreter_execute(Interpreter* interpreter, Stmt* stmt, RuntimeError** error) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case STMT_EXPRESSION: {
            Value* value = interpreter_evaluate(interpreter, stmt->as.expression.expression, error);
            if (*error) return;
            value_free(value);
            break;
        }
        
        case STMT_PRINT: {
            for (size_t i = 0; i < stmt->as.print.count; i++) {
                Value* value = interpreter_evaluate(interpreter, stmt->as.print.expressions[i], error);
                if (*error) return;
                
                char* str = stringify_value(value);
                printf("%s", str);
                if (i < stmt->as.print.count - 1) printf(" ");
                free(str);
                value_free(value);
            }
            printf("\n");
            break;
        }
        
        case STMT_VAR: {
            Value* value = NULL;
            if (stmt->as.var.initializer != NULL) {
                value = interpreter_evaluate(interpreter, stmt->as.var.initializer, error);
                if (*error) return;
            } else {
                value = value_new(VALUE_NIL);
            }
            
            environment_define(interpreter->environment, stmt->as.var.name.lexeme, value);
            break;
        }
        
        case STMT_BLOCK: {
            Environment* previous = interpreter->environment;
            interpreter->environment = environment_new(previous);
            
            for (size_t i = 0; i < stmt->as.block.statements.count; i++) {
                interpreter_execute(interpreter, &stmt->as.block.statements.statements[i], error);
                if (*error) break;
            }
            
            environment_free(interpreter->environment);
            interpreter->environment = previous;
            break;
        }
        
        case STMT_IF: {
            Value* condition = interpreter_evaluate(interpreter, stmt->as.if_stmt.condition, error);
            if (*error) return;
            
            if (is_truthy(condition)) {
                interpreter_execute(interpreter, stmt->as.if_stmt.then_branch, error);
            } else if (stmt->as.if_stmt.else_branch != NULL) {
                interpreter_execute(interpreter, stmt->as.if_stmt.else_branch, error);
            }
            
            value_free(condition);
            break;
        }
        
        case STMT_WHILE: {
            while (true) {
                Value* condition = interpreter_evaluate(interpreter, stmt->as.while_stmt.condition, error);
                if (*error) return;
                
                bool is_true = is_truthy(condition);
                value_free(condition);
                
                if (!is_true) break;
                
                interpreter_execute(interpreter, stmt->as.while_stmt.body, error);
                if (*error) return;
            }
            break;
        }
        
        default:
            *error = runtime_error_new("Unknown statement type.", 0, "<interpreter>");
            break;
    }
}

void interpreter_interpret(Interpreter* interpreter, StmtList statements, RuntimeError** error) {
    for (size_t i = 0; i < statements.count; i++) {
        interpreter_execute(interpreter, &statements.statements[i], error);
        if (*error) return;
    }
}
