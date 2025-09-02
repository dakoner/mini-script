#include "mini_script.h"
#include <stdarg.h>

/* Local strdup replacement */
static char* ms_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

static bool check(Parser* parser, TokenType type) {
    if (parser->current >= parser->count) return false;
    return parser->tokens[parser->current].type == type;
}

static Token* advance(Parser* parser) {
    if (parser->current < parser->count) {
        return &parser->tokens[parser->current++];
    }
    return &parser->tokens[parser->count - 1]; // Return EOF token
}

static bool is_at_end(Parser* parser) {
    return parser->current >= parser->count || parser->tokens[parser->current].type == EOF_TOKEN;
}

static Token* previous(Parser* parser) {
    return &parser->tokens[parser->current - 1];
}

static bool match(Parser* parser, int count, ...) {
    va_list args;
    va_start(args, count);
    
    for (int i = 0; i < count; i++) {
        TokenType type = va_arg(args, TokenType);
        if (check(parser, type)) {
            advance(parser);
            va_end(args);
            return true;
        }
    }
    
    va_end(args);
    return false;
}

static Token* consume(Parser* parser, TokenType type, const char* message, RuntimeError** error) {
    if (check(parser, type)) {
        return advance(parser);
    }
    
    Token* token = &parser->tokens[parser->current];
    *error = runtime_error_new(message, token->line, "<parser>");
    return NULL;
}

// Forward declarations
static Expr* expression(Parser* parser, RuntimeError** error);
static Stmt* statement(Parser* parser, RuntimeError** error);
static Stmt* declaration(Parser* parser, RuntimeError** error);

static Expr* primary(Parser* parser, RuntimeError** error) {
    if (match(parser, 1, FALSE)) {
        Expr* expr = expr_new(EXPR_LITERAL);
        expr->as.literal.value.type = LITERAL_BOOLEAN;
        expr->as.literal.value.value.boolean = false;
        return expr;
    }
    
    if (match(parser, 1, TRUE)) {
        Expr* expr = expr_new(EXPR_LITERAL);
        expr->as.literal.value.type = LITERAL_BOOLEAN;
        expr->as.literal.value.value.boolean = true;
        return expr;
    }
    
    if (match(parser, 1, NIL)) {
        Expr* expr = expr_new(EXPR_LITERAL);
        expr->as.literal.value.type = LITERAL_NIL;
        return expr;
    }
    
    if (match(parser, 2, NUMBER, STRING)) {
        Token* token = previous(parser);
        Expr* expr = expr_new(EXPR_LITERAL);
        if (token->literal) {
            expr->as.literal.value.type = token->literal->type;
            switch (token->literal->type) {
                case LITERAL_STRING:
                    if (token->literal->value.string) {
                        size_t len = strlen(token->literal->value.string);
                        expr->as.literal.value.value.string = malloc(len + 1);
                        memcpy(expr->as.literal.value.value.string, token->literal->value.string, len + 1);
                        expr->as.literal.value.owns_string = true;
                    } else {
                        expr->as.literal.value.value.string = NULL;
                        expr->as.literal.value.owns_string = false;
                    }
                    break;
                case LITERAL_NUMBER:
                    expr->as.literal.value.value.number = token->literal->value.number;
                    break;
                case LITERAL_INTEGER:
                    expr->as.literal.value.value.integer = token->literal->value.integer;
                    break;
                case LITERAL_BOOLEAN:
                    expr->as.literal.value.value.boolean = token->literal->value.boolean;
                    break;
                case LITERAL_CHAR:
                    expr->as.literal.value.value.character = token->literal->value.character;
                    break;
                case LITERAL_NIL:
                    break;
            }
        } else {
            expr->as.literal.value.type = LITERAL_NIL;
        }
        return expr;
    }
    
    if (match(parser, 1, IDENTIFIER)) {
        Token* name = previous(parser);
        Expr* expr = expr_new(EXPR_VARIABLE);
        expr->as.variable.name = *name;
    expr->as.variable.name.lexeme = ms_strdup(name->lexeme);
        return expr;
    }
    
    if (match(parser, 1, LEFT_PAREN)) {
        Expr* expr = expression(parser, error);
        if (*error) return NULL;
        
        consume(parser, RIGHT_PAREN, "Expected ')' after expression.", error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* grouping = expr_new(EXPR_GROUPING);
        grouping->as.grouping.expression = expr;
        return grouping;
    }
    
    if (match(parser, 1, LEFT_BRACKET)) {
        Expr* expr = expr_new(EXPR_LIST_LITERAL);
        expr->as.list_literal.elements.expressions = NULL;
        expr->as.list_literal.elements.count = 0;
        expr->as.list_literal.elements.capacity = 0;
        
        if (!check(parser, RIGHT_BRACKET)) {
            do {
                if (expr->as.list_literal.elements.count >= expr->as.list_literal.elements.capacity) {
                    expr->as.list_literal.elements.capacity = 
                        expr->as.list_literal.elements.capacity == 0 ? 8 : expr->as.list_literal.elements.capacity * 2;
                    expr->as.list_literal.elements.expressions = 
                        realloc(expr->as.list_literal.elements.expressions, 
                               expr->as.list_literal.elements.capacity * sizeof(Expr));
                }
                
                Expr* element = expression(parser, error);
                if (*error) {
                    expr_free(expr);
                    return NULL;
                }
                expr->as.list_literal.elements.expressions[expr->as.list_literal.elements.count++] = *element;
                free(element); // Transfer ownership to array
            } while (match(parser, 1, COMMA));
        }
        
        consume(parser, RIGHT_BRACKET, "Expected ']' after list elements.", error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        return expr;
    }
    
    Token* token = &parser->tokens[parser->current];
    *error = runtime_error_new("Expected expression.", token->line, "<parser>");
    return NULL;
}

static Expr* finish_call(Parser* parser, Expr* callee, RuntimeError** error) {
    Expr* expr = expr_new(EXPR_CALL);
    expr->as.call.callee = callee;
    expr->as.call.paren = parser->tokens[parser->current - 1]; // LEFT_PAREN
    expr->as.call.paren.lexeme = ms_strdup(expr->as.call.paren.lexeme);
    expr->as.call.arguments.expressions = NULL;
    expr->as.call.arguments.count = 0;
    expr->as.call.arguments.capacity = 0;
    
    if (!check(parser, RIGHT_PAREN)) {
        do {
            if (expr->as.call.arguments.count >= expr->as.call.arguments.capacity) {
                expr->as.call.arguments.capacity = 
                    expr->as.call.arguments.capacity == 0 ? 8 : expr->as.call.arguments.capacity * 2;
                expr->as.call.arguments.expressions = 
                    realloc(expr->as.call.arguments.expressions, 
                           expr->as.call.arguments.capacity * sizeof(Expr));
            }
            
            Expr* arg = expression(parser, error);
            if (*error) {
                expr_free(expr);
                return NULL;
            }
            expr->as.call.arguments.expressions[expr->as.call.arguments.count++] = *arg;
            free(arg); // Transfer ownership
        } while (match(parser, 1, COMMA));
    }
    
    consume(parser, RIGHT_PAREN, "Expected ')' after arguments.", error);
    if (*error) {
        expr_free(expr);
        return NULL;
    }
    
    return expr;
}

static Expr* call(Parser* parser, RuntimeError** error) {
    Expr* expr = primary(parser, error);
    if (*error) return NULL;
    
    while (true) {
        if (match(parser, 1, LEFT_PAREN)) {
            expr = finish_call(parser, expr, error);
            if (*error) return NULL;
        } else if (match(parser, 1, LEFT_BRACKET)) {
            Expr* index = expression(parser, error);
            if (*error) {
                expr_free(expr);
                return NULL;
            }
            
            consume(parser, RIGHT_BRACKET, "Expected ']' after index.", error);
            if (*error) {
                expr_free(expr);
                expr_free(index);
                return NULL;
            }
            
            Expr* get_expr = expr_new(EXPR_GET);
            get_expr->as.get.object = expr;
            get_expr->as.get.index = index;
            expr = get_expr;
        } else {
            break;
        }
    }
    
    return expr;
}

static Expr* unary(Parser* parser, RuntimeError** error) {
    if (match(parser, 2, NOT, MINUS)) {
        Token* op = previous(parser);
        Expr* right = unary(parser, error);
        if (*error) return NULL;
        
        Expr* expr = expr_new(EXPR_UNARY);
        expr->as.unary.op = *op;
    expr->as.unary.op.lexeme = ms_strdup(op->lexeme);
        expr->as.unary.right = right;
        return expr;
    }
    
    return call(parser, error);
}

static Expr* factor(Parser* parser, RuntimeError** error) {
    Expr* expr = unary(parser, error);
    if (*error) return NULL;
    
    while (match(parser, 2, DIVIDE, MULTIPLY)) {
        Token* op = previous(parser);
        Expr* right = unary(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* binary = expr_new(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.op = *op;
    binary->as.binary.op.lexeme = ms_strdup(op->lexeme);
        binary->as.binary.right = right;
        expr = binary;
    }
    
    return expr;
}

static Expr* term(Parser* parser, RuntimeError** error) {
    Expr* expr = factor(parser, error);
    if (*error) return NULL;
    
    while (match(parser, 2, MINUS, PLUS)) {
        Token* op = previous(parser);
        Expr* right = factor(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* binary = expr_new(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.op = *op;
    binary->as.binary.op.lexeme = ms_strdup(op->lexeme);
        binary->as.binary.right = right;
        expr = binary;
    }
    
    return expr;
}

static Expr* comparison(Parser* parser, RuntimeError** error) {
    Expr* expr = term(parser, error);
    if (*error) return NULL;
    
    while (match(parser, 4, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        Token* op = previous(parser);
        Expr* right = term(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* binary = expr_new(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.op = *op;
    binary->as.binary.op.lexeme = ms_strdup(op->lexeme);
        binary->as.binary.right = right;
        expr = binary;
    }
    
    return expr;
}

static Expr* equality(Parser* parser, RuntimeError** error) {
    Expr* expr = comparison(parser, error);
    if (*error) return NULL;
    
    while (match(parser, 2, NOT_EQUAL, EQUAL)) {
        Token* op = previous(parser);
        Expr* right = comparison(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* binary = expr_new(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.op = *op;
    binary->as.binary.op.lexeme = ms_strdup(op->lexeme);
        binary->as.binary.right = right;
        expr = binary;
    }
    
    return expr;
}

static Expr* and_expr(Parser* parser, RuntimeError** error) {
    Expr* expr = equality(parser, error);
    if (*error) return NULL;
    
    while (match(parser, 1, AND)) {
        Token* op = previous(parser);
        Expr* right = equality(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* logical = expr_new(EXPR_LOGICAL);
        logical->as.logical.left = expr;
        logical->as.logical.op = *op;
    logical->as.logical.op.lexeme = ms_strdup(op->lexeme);
        logical->as.logical.right = right;
        expr = logical;
    }
    
    return expr;
}

static Expr* or_expr(Parser* parser, RuntimeError** error) {
    Expr* expr = and_expr(parser, error);
    if (*error) return NULL;
    
    while (match(parser, 1, OR)) {
        Token* op = previous(parser);
        Expr* right = and_expr(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        Expr* logical = expr_new(EXPR_LOGICAL);
        logical->as.logical.left = expr;
        logical->as.logical.op = *op;
    logical->as.logical.op.lexeme = ms_strdup(op->lexeme);
        logical->as.logical.right = right;
        expr = logical;
    }
    
    return expr;
}

static Expr* assignment(Parser* parser, RuntimeError** error) {
    Expr* expr = or_expr(parser, error);
    if (*error) return NULL;
    
    if (match(parser, 1, ASSIGN)) {
        Token* equals = previous(parser);
        Expr* value = assignment(parser, error);
        if (*error) {
            expr_free(expr);
            return NULL;
        }
        
        if (expr->type == EXPR_VARIABLE) {
            Token name = expr->as.variable.name;
            expr_free(expr);
            
            Expr* assign = expr_new(EXPR_ASSIGN);
            assign->as.assign.name = name;
            assign->as.assign.name.lexeme = ms_strdup(name.lexeme);
            assign->as.assign.value = value;
            return assign;
        } else if (expr->type == EXPR_GET) {
            Expr* get = expr;
            Expr* set = expr_new(EXPR_SET);
            set->as.set.object = get->as.get.object;
            set->as.set.index = get->as.get.index;
            set->as.set.value = value;
            
            // Clear the get expression without freeing its components
            get->as.get.object = NULL;
            get->as.get.index = NULL;
            expr_free(get);
            
            return set;
        }
        
        *error = runtime_error_new("Invalid assignment target.", equals->line, "<parser>");
        expr_free(expr);
        expr_free(value);
        return NULL;
    }
    
    return expr;
}

static Expr* expression(Parser* parser, RuntimeError** error) {
    return assignment(parser, error);
}

Parser* parser_new(Token* tokens, size_t count) {
    Parser* parser = malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->current = 0;
    parser->count = count;
    return parser;
}

void parser_free(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

StmtList parser_parse(Parser* parser, RuntimeError** error) {
    StmtList statements;
    statements.statements = NULL;
    statements.count = 0;
    statements.capacity = 0;
    
    while (!is_at_end(parser) && !*error) {
        Stmt* stmt = declaration(parser, error);
        if (*error) {
            // Clean up already parsed statements
            for (size_t i = 0; i < statements.count; i++) {
                stmt_free(&statements.statements[i]);
            }
            free(statements.statements);
            statements.statements = NULL;
            statements.count = 0;
            statements.capacity = 0;
            return statements;
        }
        
        if (statements.count >= statements.capacity) {
            statements.capacity = statements.capacity == 0 ? 8 : statements.capacity * 2;
            statements.statements = realloc(statements.statements, statements.capacity * sizeof(Stmt));
        }
        
        statements.statements[statements.count++] = *stmt;
        free(stmt); // Transfer ownership to array
    }
    
    return statements;
}

// Implementation continues in the next part due to length...

static Stmt* print_statement(Parser* parser, RuntimeError** error) {
    Stmt* stmt = stmt_new(STMT_PRINT);
    stmt->as.print.expressions = NULL;
    stmt->as.print.count = 0;
    
    // Parse first expression
    Expr* expr = expression(parser, error);
    if (*error) {
        stmt_free(stmt);
        return NULL;
    }
    
    stmt->as.print.expressions = malloc(sizeof(Expr*));
    stmt->as.print.expressions[0] = expr;
    stmt->as.print.count = 1;
    
    // Parse additional expressions separated by commas
    while (match(parser, 1, COMMA)) {
        Expr* additional = expression(parser, error);
        if (*error) {
            stmt_free(stmt);
            return NULL;
        }
        
        stmt->as.print.expressions = realloc(stmt->as.print.expressions, 
                                           (stmt->as.print.count + 1) * sizeof(Expr*));
        stmt->as.print.expressions[stmt->as.print.count++] = additional;
    }
    
    consume(parser, SEMICOLON, "Expected ';' after print statement.", error);
    if (*error) {
        stmt_free(stmt);
        return NULL;
    }
    
    return stmt;
}

static Stmt* expression_statement(Parser* parser, RuntimeError** error) {
    Expr* expr = expression(parser, error);
    if (*error) return NULL;
    
    consume(parser, SEMICOLON, "Expected ';' after expression.", error);
    if (*error) {
        expr_free(expr);
        return NULL;
    }
    
    Stmt* stmt = stmt_new(STMT_EXPRESSION);
    stmt->as.expression.expression = expr;
    return stmt;
}

static Stmt* if_statement(Parser* parser, RuntimeError** error) {
    consume(parser, LEFT_PAREN, "Expected '(' after 'if'.", error);
    if (*error) return NULL;
    
    Expr* condition = expression(parser, error);
    if (*error) return NULL;
    
    consume(parser, RIGHT_PAREN, "Expected ')' after if condition.", error);
    if (*error) {
        expr_free(condition);
        return NULL;
    }
    
    Stmt* then_branch = statement(parser, error);
    if (*error) {
        expr_free(condition);
        return NULL;
    }
    
    Stmt* else_branch = NULL;
    if (match(parser, 1, ELSE)) {
        else_branch = statement(parser, error);
        if (*error) {
            expr_free(condition);
            stmt_free(then_branch);
            return NULL;
        }
    }
    
    Stmt* stmt = stmt_new(STMT_IF);
    stmt->as.if_stmt.condition = condition;
    stmt->as.if_stmt.then_branch = then_branch;
    stmt->as.if_stmt.else_branch = else_branch;
    return stmt;
}

static Stmt* while_statement(Parser* parser, RuntimeError** error) {
    consume(parser, LEFT_PAREN, "Expected '(' after 'while'.", error);
    if (*error) return NULL;
    
    Expr* condition = expression(parser, error);
    if (*error) return NULL;
    
    consume(parser, RIGHT_PAREN, "Expected ')' after while condition.", error);
    if (*error) {
        expr_free(condition);
        return NULL;
    }
    
    Stmt* body = statement(parser, error);
    if (*error) {
        expr_free(condition);
        return NULL;
    }
    
    Stmt* stmt = stmt_new(STMT_WHILE);
    stmt->as.while_stmt.condition = condition;
    stmt->as.while_stmt.body = body;
    return stmt;
}

static Stmt* return_statement(Parser* parser, RuntimeError** error) {
    Token* keyword = previous(parser);
    
    Expr* value = NULL;
    if (!check(parser, SEMICOLON)) {
        value = expression(parser, error);
        if (*error) return NULL;
    }
    
    consume(parser, SEMICOLON, "Expected ';' after return value.", error);
    if (*error) {
        expr_free(value);
        return NULL;
    }
    
    Stmt* stmt = stmt_new(STMT_RETURN);
    stmt->as.return_stmt.keyword = *keyword;
    stmt->as.return_stmt.keyword.lexeme = ms_strdup(keyword->lexeme);
    stmt->as.return_stmt.value = value;
    return stmt;
}

static Stmt* block_statement(Parser* parser, RuntimeError** error) {
    Stmt* stmt = stmt_new(STMT_BLOCK);
    stmt->as.block.statements.statements = NULL;
    stmt->as.block.statements.count = 0;
    stmt->as.block.statements.capacity = 0;
    
    while (!check(parser, RIGHT_BRACE) && !is_at_end(parser)) {
        Stmt* declaration_stmt = declaration(parser, error);
        if (*error) {
            stmt_free(stmt);
            return NULL;
        }
        
        if (stmt->as.block.statements.count >= stmt->as.block.statements.capacity) {
            stmt->as.block.statements.capacity = 
                stmt->as.block.statements.capacity == 0 ? 8 : stmt->as.block.statements.capacity * 2;
            stmt->as.block.statements.statements = 
                realloc(stmt->as.block.statements.statements, 
                       stmt->as.block.statements.capacity * sizeof(Stmt));
        }
        
        stmt->as.block.statements.statements[stmt->as.block.statements.count++] = *declaration_stmt;
        free(declaration_stmt); // Transfer ownership
    }
    
    consume(parser, RIGHT_BRACE, "Expected '}' after block.", error);
    if (*error) {
        stmt_free(stmt);
        return NULL;
    }
    
    return stmt;
}

static Stmt* statement(Parser* parser, RuntimeError** error) {
    if (match(parser, 1, IF)) return if_statement(parser, error);
    if (match(parser, 1, PRINT)) return print_statement(parser, error);
    if (match(parser, 1, RETURN)) return return_statement(parser, error);
    if (match(parser, 1, WHILE)) return while_statement(parser, error);
    if (match(parser, 1, LEFT_BRACE)) return block_statement(parser, error);
    
    return expression_statement(parser, error);
}

static Stmt* function_declaration(Parser* parser, const char* kind, RuntimeError** error) {
    Token* name = consume(parser, IDENTIFIER, "Expected function name.", error);
    if (*error) return NULL;
    
    consume(parser, LEFT_PAREN, "Expected '(' after function name.", error);
    if (*error) return NULL;
    
    Token* params = NULL;
    size_t param_count = 0;
    
    if (!check(parser, RIGHT_PAREN)) {
        do {
            params = realloc(params, (param_count + 1) * sizeof(Token));
            Token* param = consume(parser, IDENTIFIER, "Expected parameter name.", error);
            if (*error) {
                free(params);
                return NULL;
            }
            params[param_count++] = *param;
            params[param_count - 1].lexeme = ms_strdup(param->lexeme);
        } while (match(parser, 1, COMMA));
    }
    
    consume(parser, RIGHT_PAREN, "Expected ')' after parameters.", error);
    if (*error) {
        for (size_t i = 0; i < param_count; i++) {
            free(params[i].lexeme);
        }
        free(params);
        return NULL;
    }
    
    consume(parser, LEFT_BRACE, "Expected '{' before function body.", error);
    if (*error) {
        for (size_t i = 0; i < param_count; i++) {
            free(params[i].lexeme);
        }
        free(params);
        return NULL;
    }
    
    Stmt* body_stmt = block_statement(parser, error);
    if (*error) {
        for (size_t i = 0; i < param_count; i++) {
            free(params[i].lexeme);
        }
        free(params);
        return NULL;
    }
    
    Stmt* stmt = stmt_new(STMT_FUNCTION);
    stmt->as.function.name = *name;
    stmt->as.function.name.lexeme = ms_strdup(name->lexeme);
    stmt->as.function.params = params;
    stmt->as.function.param_count = param_count;
    stmt->as.function.body = body_stmt->as.block.statements;
    
    // Transfer ownership of statements from block
    body_stmt->as.block.statements.statements = NULL;
    body_stmt->as.block.statements.count = 0;
    body_stmt->as.block.statements.capacity = 0;
    stmt_free(body_stmt);
    
    return stmt;
}

static Stmt* var_declaration(Parser* parser, RuntimeError** error) {
    Token* name = consume(parser, IDENTIFIER, "Expected variable name.", error);
    if (*error) return NULL;
    
    Expr* initializer = NULL;
    if (match(parser, 1, ASSIGN)) {
        initializer = expression(parser, error);
        if (*error) return NULL;
    }
    
    consume(parser, SEMICOLON, "Expected ';' after variable declaration.", error);
    if (*error) {
        expr_free(initializer);
        return NULL;
    }
    
    Stmt* stmt = stmt_new(STMT_VAR);
    stmt->as.var.name = *name;
    stmt->as.var.name.lexeme = ms_strdup(name->lexeme);
    stmt->as.var.initializer = initializer;
    return stmt;
}

static Stmt* declaration(Parser* parser, RuntimeError** error) {
    if (match(parser, 1, FUNCTION)) return function_declaration(parser, "function", error);
    if (match(parser, 1, VAR)) return var_declaration(parser, error);
    
    return statement(parser, error);
}
