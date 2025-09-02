#include "mini_script.h"

Environment *environment_new(Environment *enclosing) {
  Environment *env = malloc(sizeof(Environment));
  env->values.keys = NULL;
  env->values.values = NULL;
  env->values.count = 0;
  env->values.capacity = 0;
  env->enclosing = enclosing;
  return env;
}

void environment_free(Environment *env) {
  if (!env)
    return;

  for (size_t i = 0; i < env->values.count; i++) {
    free(env->values.keys[i]);
    if (env->values.values[i]) {
      value_free(env->values.values[i]);
    }
  }
  free(env->values.keys);
  free(env->values.values);
  free(env);
}

void environment_define(Environment *env, const char *name, Value *value) {
  // Check if variable already exists
  for (size_t i = 0; i < env->values.count; i++) {
    if (strcmp(env->values.keys[i], name) == 0) {
      // Replace existing value
      if (env->values.values[i]) {
        value_free(env->values.values[i]);
      }
      env->values.values[i] = value; /* take ownership */
      return;
    }
  }

  // Add new variable
  if (env->values.count >= env->values.capacity) {
    env->values.capacity =
        env->values.capacity == 0 ? 8 : env->values.capacity * 2;
    env->values.keys =
        realloc(env->values.keys, env->values.capacity * sizeof(char *));
    env->values.values =
        realloc(env->values.values, env->values.capacity * sizeof(Value *));
  }

  env->values.keys[env->values.count] = malloc(strlen(name) + 1);
  strcpy(env->values.keys[env->values.count], name);
  env->values.values[env->values.count] = value; /* take ownership */
  env->values.count++;
}

Value *environment_get(Environment *env, Token *name, RuntimeError **error) {
  // Search current environment
  for (size_t i = 0; i < env->values.count; i++) {
    if (strcmp(env->values.keys[i], name->lexeme) == 0) {
      return env->values.values[i];
    }
  }

  // Search enclosing environment
  if (env->enclosing != NULL) {
    return environment_get(env->enclosing, name, error);
  }

  // Variable not found
  char message[256];
  snprintf(message, sizeof(message), "Undefined variable '%s'", name->lexeme);
  *error = runtime_error_new(message, name->line, "<unknown>");
  return NULL;
}

void environment_assign(Environment *env, Token *name, Value *value,
                        RuntimeError **error) {
  // Search current environment
  for (size_t i = 0; i < env->values.count; i++) {
    if (strcmp(env->values.keys[i], name->lexeme) == 0) {
      if (env->values.values[i]) {
        value_free(env->values.values[i]);
      }
      env->values.values[i] = value; /* take ownership */
      return;
    }
  }

  // Search enclosing environment
  if (env->enclosing != NULL) {
    environment_assign(env->enclosing, name, value, error);
    return;
  }

  // Variable not found
  char message[256];
  snprintf(message, sizeof(message), "Undefined variable '%s'", name->lexeme);
  *error = runtime_error_new(message, name->line, "<unknown>");
}
