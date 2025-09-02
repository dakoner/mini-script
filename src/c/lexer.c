#include "mini_script.h"
#include <ctype.h>
#include <string.h>

/* Keywords table */
static const struct {
  const char *keyword;
  TokenType type;
} keywords[] = {{"and", AND},
                {"assert", ASSERT},
                {"char", CHAR_TYPE},
                {"else", ELSE},
                {"false", FALSE},
                {"float", FLOAT_TYPE},
                {"for", FOR},
                {"from", FROM},
                {"function", FUNCTION},
                {"if", IF},
                {"import", IMPORT},
                {"int", INT_TYPE},
                {"list", LIST},
                {"map", MAP},
                {"nil", NIL},
                {"or", OR},
                {"print", PRINT},
                {"return", RETURN},
                {"string", STRING_TYPE},
                {"true", TRUE},
                {"var", VAR},
                {"while", WHILE},
                {"loadlib", LOADLIB},
                {"getproc", GETPROC},
                {"freelib", FREELIB},
                {"callext", CALLEXT},
                {NULL, EOF_TOKEN}};

static TokenType get_keyword_type(const char *text) {
  for (int i = 0; keywords[i].keyword != NULL; i++) {
    if (strcmp(text, keywords[i].keyword) == 0) {
      return keywords[i].type;
    }
  }
  return IDENTIFIER;
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_alphanumeric(char c) { return is_alpha(c) || is_digit(c); }

static void add_token(Lexer *lexer, TokenType type, LiteralValue *literal) {
  if (lexer->token_count >= lexer->token_capacity) {
    lexer->token_capacity =
        lexer->token_capacity == 0 ? 8 : lexer->token_capacity * 2;
    lexer->tokens =
        realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
  }

  size_t length = lexer->current - lexer->start;
  char *lexeme = malloc(length + 1);
  strncpy(lexeme, lexer->source + lexer->start, length);
  lexeme[length] = '\0';

  lexer->tokens[lexer->token_count].type = type;
  lexer->tokens[lexer->token_count].lexeme = lexeme;
  lexer->tokens[lexer->token_count].literal = literal;
  lexer->tokens[lexer->token_count].line = lexer->line;
  lexer->token_count++;
}

static char advance(Lexer *lexer) { return lexer->source[lexer->current++]; }

static bool match(Lexer *lexer, char expected) {
  if (lexer->source[lexer->current] == '\0')
    return false;
  if (lexer->source[lexer->current] != expected)
    return false;
  lexer->current++;
  return true;
}

static char peek(Lexer *lexer) { return lexer->source[lexer->current]; }

static char peek_next(Lexer *lexer) {
  if (lexer->source[lexer->current] == '\0')
    return '\0';
  return lexer->source[lexer->current + 1];
}

static void scan_string(Lexer *lexer) {
  while (peek(lexer) != '"' && peek(lexer) != '\0') {
    if (peek(lexer) == '\n')
      lexer->line++;
    advance(lexer);
  }

  if (peek(lexer) == '\0') {
    // Unterminated string - just return
    return;
  }

  advance(lexer); // The closing "

  // Trim the surrounding quotes
  size_t length = lexer->current - lexer->start - 2;
  char *value = malloc(length + 1);
  strncpy(value, lexer->source + lexer->start + 1, length);
  value[length] = '\0';

  LiteralValue *literal = literal_new(LITERAL_STRING);
  literal->value.string = value;
  /* Lexer owns raw string until parser duplicates; mark ownership */
  literal->owns_string = true;

  add_token(lexer, STRING, literal);
}

static void scan_number(Lexer *lexer) {
  while (is_digit(peek(lexer)))
    advance(lexer);

  // Look for fractional part
  if (peek(lexer) == '.' && is_digit(peek_next(lexer))) {
    advance(lexer); // Consume the "."
    while (is_digit(peek(lexer)))
      advance(lexer);

    size_t length = lexer->current - lexer->start;
    char *text = malloc(length + 1);
    strncpy(text, lexer->source + lexer->start, length);
    text[length] = '\0';

    LiteralValue *literal = literal_new(LITERAL_NUMBER);
    literal->value.number = atof(text);
    free(text);

    add_token(lexer, NUMBER, literal);
  } else {
    size_t length = lexer->current - lexer->start;
    char *text = malloc(length + 1);
    strncpy(text, lexer->source + lexer->start, length);
    text[length] = '\0';

    LiteralValue *literal = literal_new(LITERAL_INTEGER);
    literal->value.integer = atol(text);
    free(text);

    add_token(lexer, NUMBER, literal);
  }
}

static void scan_identifier(Lexer *lexer) {
  while (is_alphanumeric(peek(lexer)))
    advance(lexer);

  size_t length = lexer->current - lexer->start;
  char *text = malloc(length + 1);
  strncpy(text, lexer->source + lexer->start, length);
  text[length] = '\0';

  TokenType type = get_keyword_type(text);

  LiteralValue *literal = NULL;
  if (type == TRUE) {
    literal = literal_new(LITERAL_BOOLEAN);
    literal->value.boolean = true;
  } else if (type == FALSE) {
    literal = literal_new(LITERAL_BOOLEAN);
    literal->value.boolean = false;
  } else if (type == NIL) {
    literal = literal_new(LITERAL_NIL);
  }

  add_token(lexer, type, literal);
  free(text);
}

static void scan_token(Lexer *lexer) {
  char c = advance(lexer);

  switch (c) {
  case '(':
    add_token(lexer, LEFT_PAREN, NULL);
    break;
  case ')':
    add_token(lexer, RIGHT_PAREN, NULL);
    break;
  case '{':
    add_token(lexer, LEFT_BRACE, NULL);
    break;
  case '}':
    add_token(lexer, RIGHT_BRACE, NULL);
    break;
  case '[':
    add_token(lexer, LEFT_BRACKET, NULL);
    break;
  case ']':
    add_token(lexer, RIGHT_BRACKET, NULL);
    break;
  case ',':
    add_token(lexer, COMMA, NULL);
    break;
  case '.':
    add_token(lexer, DOT, NULL);
    break;
  case '-':
    add_token(lexer, MINUS, NULL);
    break;
  case '+':
    add_token(lexer, PLUS, NULL);
    break;
  case ';':
    add_token(lexer, SEMICOLON, NULL);
    break;
  case '*':
    add_token(lexer, MULTIPLY, NULL);
    break;
  case '/':
    if (match(lexer, '/')) {
      // A comment goes until the end of the line
      while (peek(lexer) != '\n' && peek(lexer) != '\0')
        advance(lexer);
    } else {
      add_token(lexer, DIVIDE, NULL);
    }
    break;
  case '!':
    add_token(lexer, match(lexer, '=') ? NOT_EQUAL : NOT, NULL);
    break;
  case '=':
    add_token(lexer, match(lexer, '=') ? EQUAL : ASSIGN, NULL);
    break;
  case '<':
    add_token(lexer, match(lexer, '=') ? LESS_EQUAL : LESS, NULL);
    break;
  case '>':
    add_token(lexer, match(lexer, '=') ? GREATER_EQUAL : GREATER, NULL);
    break;
  case '&':
    if (match(lexer, '&')) {
      add_token(lexer, AND, NULL);
    } else {
      // Single & is not supported, could be an error
      add_token(lexer, EOF_TOKEN, NULL); // Treat as error for now
    }
    break;
  case '|':
    if (match(lexer, '|')) {
      add_token(lexer, OR, NULL);
    } else {
      // Single | is not supported, could be an error
      add_token(lexer, EOF_TOKEN, NULL); // Treat as error for now
    }
    break;
  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace
    break;
  case '\n':
    lexer->line++;
    break;
  case '"':
    scan_string(lexer);
    break;
  case '\'':
    // Character literal
    if (peek(lexer) != '\0' && peek_next(lexer) == '\'') {
      char ch = advance(lexer);
      advance(lexer); // closing '

      LiteralValue *literal = literal_new(LITERAL_CHAR);
      literal->value.character = ch;
      add_token(lexer, CHAR, literal);
    }
    break;
  default:
    if (is_digit(c)) {
      scan_number(lexer);
    } else if (is_alpha(c)) {
      scan_identifier(lexer);
    } else {
      // Unexpected character - ignore
    }
    break;
  }
}

Lexer *lexer_new(const char *source) {
  Lexer *lexer = malloc(sizeof(Lexer));
  lexer->source = source;
  lexer->start = 0;
  lexer->current = 0;
  lexer->line = 1;
  lexer->tokens = NULL;
  lexer->token_count = 0;
  lexer->token_capacity = 0;
  return lexer;
}

void lexer_free(Lexer *lexer) {
  if (lexer) {
    for (size_t i = 0; i < lexer->token_count; i++) {
      free(lexer->tokens[i].lexeme);
      if (lexer->tokens[i].literal) {
        literal_free(lexer->tokens[i].literal);
      }
    }
    free(lexer->tokens);
    free(lexer);
  }
}

void lexer_scan_tokens(Lexer *lexer) {
  while (lexer->source[lexer->current] != '\0') {
    lexer->start = lexer->current;
    scan_token(lexer);
  }

  lexer->start = lexer->current;
  add_token(lexer, EOF_TOKEN, NULL);
}

/* Literal value functions */
LiteralValue *literal_new(LiteralType type) {
  LiteralValue *literal = malloc(sizeof(LiteralValue));
  literal->type = type;
  memset(&literal->value, 0, sizeof(literal->value));
  literal->owns_string = false;
  return literal;
}

void literal_free(LiteralValue *literal) {
  if (literal) {
    if (literal->type == LITERAL_STRING && literal->value.string &&
        literal->owns_string) {
      free(literal->value.string);
    }
    free(literal);
  }
}
