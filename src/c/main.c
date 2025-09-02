#include "mini_script.h"

char *read_file(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", filename);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char *buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", filename);
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size) {
    fprintf(stderr, "Could not read file \"%s\".\n", filename);
    free(buffer);
    fclose(file);
    return NULL;
  }

  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}

static void run(const char *source) {
  Lexer *lexer = lexer_new(source);
  lexer_scan_tokens(lexer);
  const char *debug_tokens = getenv("MS_DEBUG_TOKENS");
  if (debug_tokens && strcmp(debug_tokens, "0") != 0) {
    fprintf(stderr, "[DEBUG] Tokens (count=%zu):\n", lexer->token_count);
    for (size_t i = 0; i < lexer->token_count; i++) {
      Token *t = &lexer->tokens[i];
      fprintf(stderr, "  %zu: type=%d lexeme='%s' line=%zu\n", i, t->type,
              t->lexeme, t->line);
      if (i > 200) { /* avoid huge dumps */
        fprintf(stderr, "  ... (truncated) ...\n");
        break;
      }
    }
  }

  Parser *parser = parser_new(lexer->tokens, lexer->token_count);
  RuntimeError *error = NULL;
  StmtList statements = parser_parse(parser, &error);

  if (error) {
    fprintf(stderr, "Parse error: %s\n", error->message);
    runtime_error_free(error);
    parser_free(parser);
    lexer_free(lexer);
    return;
  }

  Interpreter *interpreter = interpreter_new();
  interpreter_interpret(interpreter, statements, &error);

  if (error) {
    fprintf(stderr, "Runtime error: %s\n", error->message);
    runtime_error_free(error);
  }

  // Clean up
  for (size_t i = 0; i < statements.count; i++) {
    stmt_free(statements.statements[i]);
  }
  free(statements.statements);

  interpreter_free(interpreter);
  parser_free(parser);
  lexer_free(lexer);
}

void run_file(const char *filename) {
  char *source = read_file(filename);
  if (source == NULL) {
    exit(74);
  }

  run(source);
  free(source);
}

void run_prompt(void) {
  char line[1024];

  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    run(line);
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    fprintf(stderr, "Usage: mini_script [script]\n");
    exit(64);
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    run_prompt();
  }

  return 0;
}
