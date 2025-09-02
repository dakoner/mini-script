#include "mini_script.h"
#include <math.h>

/* Local strdup replacement */
static char *ms_strdup(const char *s) {
  if (!s)
    return NULL;
  size_t len = strlen(s);
  char *copy = malloc(len + 1);
  if (!copy)
    return NULL;
  memcpy(copy, s, len + 1);
  return copy;
}

/* Builtin function implementations */
static Value *builtin_print(Interpreter *interpreter, Value **args,
                            int arg_count) {
  for (int i = 0; i < arg_count; i++) {
    char *str = stringify_value(args[i]);
    printf("%s", str);
    if (i < arg_count - 1)
      printf(" ");
    free(str);
  }
  printf("\n");

  Value *result = value_new(VALUE_NIL);
  return result;
}

static Value *builtin_len(Interpreter *interpreter, Value **args,
                          int arg_count) {
  if (arg_count != 1) {
    return NULL; // Error
  }

  Value *result = value_new(VALUE_NUMBER);

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

static Value *builtin_time_now(Interpreter *interpreter, Value **args,
                               int arg_count) {
  if (arg_count != 0)
    return NULL;

  Value *result = value_new(VALUE_NUMBER);
  result->as.number = (double)time(NULL);
  return result;
}

static Value *call_builtin_function(Interpreter *interpreter, const char *name,
                                    Value **args, int arg_count) {
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
Interpreter *interpreter_new(void) {
  Interpreter *interpreter = malloc(sizeof(Interpreter));
  interpreter->globals = environment_new(NULL);
  interpreter->environment = interpreter->globals;
  interpreter->modules_path = NULL;
  interpreter->modules_path_count = 0;

  interpreter_define_builtins(interpreter);

  return interpreter;
}

void interpreter_free(Interpreter *interpreter) {
  if (interpreter) {
    environment_free(interpreter->globals);
    for (size_t i = 0; i < interpreter->modules_path_count; i++) {
      free(interpreter->modules_path[i]);
    }
    free(interpreter->modules_path);
    free(interpreter);
  }
}

void interpreter_define_builtins(Interpreter *interpreter) {
  Value *print_builtin = value_new(VALUE_BUILTIN);
  print_builtin->as.builtin_name = ms_strdup("print");
  environment_define(interpreter->globals, "print", print_builtin);

  Value *len_builtin = value_new(VALUE_BUILTIN);
  len_builtin->as.builtin_name = ms_strdup("len");
  environment_define(interpreter->globals, "len", len_builtin);

  Value *time_now_builtin = value_new(VALUE_BUILTIN);
  time_now_builtin->as.builtin_name = ms_strdup("time_now");
  environment_define(interpreter->globals, "time_now", time_now_builtin);
}

Value *interpreter_evaluate(Interpreter *interpreter, Expr *expr,
                            RuntimeError **error) {
  if (!expr) {
    *error = runtime_error_new("Null expression", 0, "<interpreter>");
    return NULL;
  }

  switch (expr->type) {
  case EXPR_LITERAL: {
    Value *value = value_new(VALUE_NIL);
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
      value->as.string = ms_strdup(expr->as.literal.value.value.string);
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
    Value *stored_value = environment_get(interpreter->environment,
                                          &expr->as.variable.name, error);
    if (*error)
      return NULL;
    return value_copy(stored_value); /* caller owns copy */
  }

  case EXPR_ASSIGN: {
    Value *rhs =
        interpreter_evaluate(interpreter, expr->as.assign.value, error);
    if (*error)
      return NULL;
    environment_assign(interpreter->environment, &expr->as.assign.name, rhs,
                       error);
    if (*error) {
      value_free(rhs);
      return NULL;
    }
    /* rhs now owned by environment, return a copy to caller */
    return value_copy(rhs);
  }

  case EXPR_BINARY: {
    Value *left =
        interpreter_evaluate(interpreter, expr->as.binary.left, error);
    if (*error)
      return NULL;

    Value *right =
        interpreter_evaluate(interpreter, expr->as.binary.right, error);
    if (*error) {
      value_free(left);
      return NULL;
    }

    Value *result = value_new(VALUE_NIL);

    switch (expr->as.binary.op.type) {
    case PLUS:
      if (left->type == VALUE_NUMBER && right->type == VALUE_NUMBER) {
        result->type = VALUE_NUMBER;
        result->as.number = left->as.number + right->as.number;
      } else if (left->type == VALUE_STRING || right->type == VALUE_STRING) {
        /* Coerce both sides to string */
        char *lstr = left->type == VALUE_STRING ? ms_strdup(left->as.string)
                                                : stringify_value(left);
        char *rstr = right->type == VALUE_STRING ? ms_strdup(right->as.string)
                                                 : stringify_value(right);
        size_t len = strlen(lstr) + strlen(rstr) + 1;
        result->type = VALUE_STRING;
        result->as.string = malloc(len);
        strcpy(result->as.string, lstr);
        strcat(result->as.string, rstr);
        free(lstr);
        free(rstr);
      } else {
        value_free(result);
        value_free(left);
        value_free(right);
        *error =
            runtime_error_new("Operands must be two numbers or two strings.",
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

  case EXPR_GET: {
    Value *object =
        interpreter_evaluate(interpreter, expr->as.get.object, error);
    if (*error)
      return NULL;
    Value *index_val =
        interpreter_evaluate(interpreter, expr->as.get.index, error);
    if (*error) {
      value_free(object);
      return NULL;
    }
    if (object->type != VALUE_LIST || index_val->type != VALUE_NUMBER) {
      value_free(object);
      value_free(index_val);
      *error =
          runtime_error_new("Invalid index operation.", 0, "<interpreter>");
      return NULL;
    }
    long idx = (long)index_val->as.number;
    value_free(index_val);
    if (idx < 0 || (size_t)idx >= object->as.list->count) {
      value_free(object);
      *error =
          runtime_error_new("List index out of range.", 0, "<interpreter>");
      return NULL;
    }
    /* Return copy of element */
    Value *copy = value_copy(&object->as.list->elements[idx]);
    value_free(object);
    return copy;
  }

  case EXPR_SET: {
    Value *object =
        interpreter_evaluate(interpreter, expr->as.set.object, error);
    if (*error)
      return NULL;
    Value *index_val =
        interpreter_evaluate(interpreter, expr->as.set.index, error);
    if (*error) {
      value_free(object);
      return NULL;
    }
    Value *value_rhs =
        interpreter_evaluate(interpreter, expr->as.set.value, error);
    if (*error) {
      value_free(object);
      value_free(index_val);
      return NULL;
    }
    if (object->type != VALUE_LIST || index_val->type != VALUE_NUMBER) {
      value_free(object);
      value_free(index_val);
      value_free(value_rhs);
      *error = runtime_error_new("Invalid set operation.", 0, "<interpreter>");
      return NULL;
    }
    long idx = (long)index_val->as.number;
    value_free(index_val);
    if (idx < 0 || (size_t)idx >= object->as.list->count) {
      value_free(object);
      value_free(value_rhs);
      *error =
          runtime_error_new("List index out of range.", 0, "<interpreter>");
      return NULL;
    }
    /* Replace element */
    value_free(&object->as.list->elements[idx]);
    object->as.list->elements[idx] = *value_copy(value_rhs);
    Value *ret = value_copy(&object->as.list->elements[idx]);
    value_free(object);
    value_free(value_rhs);
    return ret;
  }

  case EXPR_UNARY: {
    Value *right =
        interpreter_evaluate(interpreter, expr->as.unary.right, error);
    if (*error)
      return NULL;

    Value *result = value_new(VALUE_NIL);

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
    Value *callee =
        interpreter_evaluate(interpreter, expr->as.call.callee, error);
    if (*error)
      return NULL;

    // Evaluate arguments
    Value **arguments = malloc(expr->as.call.arguments.count * sizeof(Value *));
    for (size_t i = 0; i < expr->as.call.arguments.count; i++) {
      arguments[i] = interpreter_evaluate(
          interpreter, expr->as.call.arguments.expressions[i], error);
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

    Value *result = NULL;

    if (callee->type == VALUE_BUILTIN) {
      result = call_builtin_function(interpreter, callee->as.builtin_name,
                                     arguments, expr->as.call.arguments.count);
      if (!result) {
        *error = runtime_error_new("Error calling builtin function.",
                                   expr->as.call.paren.line, "<interpreter>");
      }
    } else if (callee->type == VALUE_FUNCTION) {
      // Check parameter count
      if (expr->as.call.arguments.count != callee->as.function->declaration->as.function.param_count) {
        *error = runtime_error_new("Wrong number of arguments.",
                                   expr->as.call.paren.line, "<interpreter>");
      } else {
        // Create new environment for function scope
        Environment *previous = interpreter->environment;
        interpreter->environment = environment_new(callee->as.function->closure);
        
        // Bind parameters to arguments
        for (size_t i = 0; i < callee->as.function->declaration->as.function.param_count; i++) {
          // Create a copy of the argument value for the parameter
          Value *arg_copy = value_copy(arguments[i]);
          environment_define(interpreter->environment, 
                           callee->as.function->declaration->as.function.params[i].lexeme, 
                           arg_copy);
        }
        
        // Execute function body
        for (size_t i = 0; i < callee->as.function->declaration->as.function.body.count && !*error; i++) {
          interpreter_execute(interpreter, callee->as.function->declaration->as.function.body.statements[i], error);
        }
        
        // Restore previous environment
        interpreter->environment = previous;
        
        // For now, user functions return nil (proper return value handling needs more work)
        result = value_new(VALUE_NIL);
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
    return interpreter_evaluate(interpreter, expr->as.grouping.expression,
                                error);
  }

  case EXPR_LIST_LITERAL: {
    Value *list_value = value_new(VALUE_LIST);
    list_value->as.list = malloc(sizeof(ValueList));
    list_value->as.list->capacity = expr->as.list_literal.elements.count;
    list_value->as.list->count = 0;
    list_value->as.list->elements =
        malloc(list_value->as.list->capacity * sizeof(Value));

    for (size_t i = 0; i < expr->as.list_literal.elements.count; i++) {
      Value *element = interpreter_evaluate(
          interpreter, expr->as.list_literal.elements.expressions[i], error);
      if (*error) {
        value_free(list_value);
        return NULL;
      }
      list_value->as.list->elements[list_value->as.list->count++] = *element;
      free(element);
    }

    return list_value;
  }

  case EXPR_LOGICAL: {
    Value *left = interpreter_evaluate(interpreter, expr->as.logical.left, error);
    if (*error)
      return NULL;

    // Short-circuit evaluation
    if (expr->as.logical.op.type == AND) {
      // For AND: if left is false, don't evaluate right
      if (!is_truthy(left)) {
        return left; // Return the falsy left value
      }
    } else if (expr->as.logical.op.type == OR) {
      // For OR: if left is true, don't evaluate right
      if (is_truthy(left)) {
        return left; // Return the truthy left value
      }
    }

    // If we get here, we need to evaluate the right side
    value_free(left);
    return interpreter_evaluate(interpreter, expr->as.logical.right, error);
  }

  default:
    *error = runtime_error_new("Unknown expression type.", 0, "<interpreter>");
    return NULL;
  }
}

void interpreter_execute(Interpreter *interpreter, Stmt *stmt,
                         RuntimeError **error) {
  if (!stmt)
    return;

  switch (stmt->type) {
  case STMT_EXPRESSION: {
    Value *value = interpreter_evaluate(interpreter,
                                        stmt->as.expression.expression, error);
    if (*error)
      return;
    value_free(value);
    break;
  }

  case STMT_PRINT: {
    for (size_t i = 0; i < stmt->as.print.count; i++) {
      Value *value = interpreter_evaluate(interpreter,
                                          stmt->as.print.expressions[i], error);
      if (*error)
        return;

      char *str = stringify_value(value);
      printf("%s", str);
      if (i < stmt->as.print.count - 1)
        printf(" ");
      free(str);
      value_free(value);
    }
    printf("\n");
    break;
  }

  case STMT_VAR: {
    Value *value = NULL;
    if (stmt->as.var.initializer != NULL) {
      value =
          interpreter_evaluate(interpreter, stmt->as.var.initializer, error);
      if (*error)
        return;
    } else {
      value = value_new(VALUE_NIL);
    }

    environment_define(interpreter->environment, stmt->as.var.name.lexeme,
                       value);
    /* ownership moved into environment */
    break;
  }

  case STMT_BLOCK: {
    Environment *previous = interpreter->environment;
    interpreter->environment = environment_new(previous);

    for (size_t i = 0; i < stmt->as.block.statements.count; i++) {
      interpreter_execute(interpreter, stmt->as.block.statements.statements[i],
                          error);
      if (*error)
        break;
    }

    environment_free(interpreter->environment);
    interpreter->environment = previous;
    break;
  }

  case STMT_IF: {
    Value *condition =
        interpreter_evaluate(interpreter, stmt->as.if_stmt.condition, error);
    if (*error)
      return;

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
      Value *condition = interpreter_evaluate(
          interpreter, stmt->as.while_stmt.condition, error);
      if (*error)
        return;

      bool is_true = is_truthy(condition);
      value_free(condition);

      if (!is_true)
        break;

      interpreter_execute(interpreter, stmt->as.while_stmt.body, error);
      if (*error)
        return;
    }
    break;
  }

  case STMT_FOR: {
    // Execute initializer once
    if (stmt->as.for_stmt.initializer) {
      interpreter_execute(interpreter, stmt->as.for_stmt.initializer, error);
      if (*error)
        return;
    }

    // Loop with condition and increment
    while (true) {
      // Check condition (if no condition, assume true)
      if (stmt->as.for_stmt.condition) {
        Value *condition = interpreter_evaluate(
            interpreter, stmt->as.for_stmt.condition, error);
        if (*error)
          return;

        bool is_true = is_truthy(condition);
        value_free(condition);

        if (!is_true)
          break;
      }

      // Execute body
      interpreter_execute(interpreter, stmt->as.for_stmt.body, error);
      if (*error)
        return;

      // Execute increment
      if (stmt->as.for_stmt.increment) {
        Value *inc_result = interpreter_evaluate(
            interpreter, stmt->as.for_stmt.increment, error);
        if (*error)
          return;
        value_free(inc_result);
      }
    }
    break;
  }

  case STMT_ASSERT: {
    Value *condition = interpreter_evaluate(
        interpreter, stmt->as.assert_stmt.condition, error);
    if (*error)
      return;
    bool ok = is_truthy(condition);
    value_free(condition);
    if (!ok) {
      const char *msg = "Assertion failed";
      if (stmt->as.assert_stmt.message) {
        Value *msg_val = interpreter_evaluate(
            interpreter, stmt->as.assert_stmt.message, error);
        if (!*error && msg_val && msg_val->type == VALUE_STRING) {
          msg = msg_val->as.string;
        }
        if (msg_val)
          value_free(msg_val);
      }
      *error =
          runtime_error_new(msg, stmt->as.assert_stmt.keyword.line, "<assert>");
      return;
    }
    break;
  }

  case STMT_FUNCTION: {
    // Create a function value and define it in the current environment
    Value *func = value_new(VALUE_FUNCTION);
    func->as.function = malloc(sizeof(MiniScriptFunction));
    func->as.function->declaration = stmt;
    func->as.function->closure = interpreter->environment;
    
    environment_define(interpreter->environment, stmt->as.function.name.lexeme, func);
    break;
  }

  case STMT_RETURN: {
    // For now, just evaluate the return value but don't do anything with it
    // A proper implementation would need a way to unwind the call stack
    // and return the value to the caller
    if (stmt->as.return_stmt.value != NULL) {
      Value *value = interpreter_evaluate(interpreter, stmt->as.return_stmt.value, error);
      if (*error)
        return;
      value_free(value);
    }
    // Just continue execution for now
    break;
  }

  default:
    *error = runtime_error_new("Unknown statement type.", 0, "<interpreter>");
    break;
  }
}

void interpreter_interpret(Interpreter *interpreter, StmtList statements,
                           RuntimeError **error) {
  for (size_t i = 0; i < statements.count; i++) {
    interpreter_execute(interpreter, statements.statements[i], error);
    if (*error)
      return;
  }
}
