import sys
import os
import time
from datetime import datetime
from enum import Enum, auto


# =============================================================================
# 1. TOKENIZER (LEXER)
# =============================================================================

class TokenType(Enum):
    # Single-character tokens.
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET = (
        auto(), auto(), auto(), auto(), auto(), auto())
    COMMA, DOT, MINUS, PLUS, SEMICOLON, DIVIDE, MULTIPLY = (
        auto(), auto(), auto(), auto(), auto(), auto(), auto())

    # One or two character tokens.
    NOT, NOT_EQUAL = auto(), auto()
    ASSIGN, EQUAL = auto(), auto()
    GREATER, GREATER_EQUAL = auto(), auto()
    LESS, LESS_EQUAL = auto(), auto()
    AND, OR = auto(), auto()

    # Literals.
    IDENTIFIER, STRING, NUMBER, CHAR = auto(), auto(), auto(), auto()

    # Keywords.
    ELSE, FALSE, FOR, FUNCTION, IF, RETURN, TRUE, WHILE, IMPORT = (
        auto(), auto(), auto(), auto(), auto(), auto(), auto(), auto(), auto())
    INT_TYPE, FLOAT_TYPE, CHAR_TYPE, STRING_TYPE, LIST, MAP = (
        auto(), auto(), auto(), auto(), auto(), auto())
    LOADLIB, GETPROC, FREELIB, CALLEXT = auto(), auto(), auto(), auto()
    ASSERT, VAR, NIL = auto(), auto(), auto()

    EOF = auto()


KEYWORDS = {
    "if":       TokenType.IF,
    "else":     TokenType.ELSE,
    "while":    TokenType.WHILE,
    "for":      TokenType.FOR,
    "function": TokenType.FUNCTION,
    "return":   TokenType.RETURN,
    "true":     TokenType.TRUE,
    "false":    TokenType.FALSE,
    "import":   TokenType.IMPORT,
    "int":      TokenType.INT_TYPE,
    "float":    TokenType.FLOAT_TYPE,
    "char":     TokenType.CHAR_TYPE,
    "string":   TokenType.STRING_TYPE,
    "list":     TokenType.LIST,
    "map":      TokenType.MAP,
    "loadlib":  TokenType.LOADLIB,
    "getproc":  TokenType.GETPROC,
    "freelib":  TokenType.FREELIB,
    "callext":  TokenType.CALLEXT,
    "assert":   TokenType.ASSERT,
    "var":      TokenType.VAR,
    "nil":      TokenType.NIL,
}


class Token:
    def __init__(self, type, lexeme, literal, line):
        self.type = type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line

    def __repr__(self):
        return (f"Token(type={self.type.name}, "
                f"lexeme='{self.lexeme}', literal={self.literal})")


class Lexer:
    def __init__(self, source, filename="<unknown>"):
        self.source = source
        self.filename = filename
        self.tokens = []
        self.start = 0
        self.current = 0
        self.line = 1

    def scan_tokens(self):
        while not self.is_at_end():
            self.start = self.current
            self.scan_token()
        self.tokens.append(Token(TokenType.EOF, "", None, self.line))
        return self.tokens

    def is_at_end(self):
        return self.current >= len(self.source)

    def advance(self):
        char = self.source[self.current]
        self.current += 1
        return char

    def peek(self):
        if self.is_at_end():
            return '\0'
        return self.source[self.current]

    def peek_next(self):
        if self.current + 1 >= len(self.source):
            return '\0'
        return self.source[self.current + 1]

    def match(self, expected):
        if self.is_at_end():
            return False
        if self.source[self.current] != expected:
            return False
        self.current += 1
        return True

    def add_token(self, type, literal=None):
        text = self.source[self.start:self.current]
        self.tokens.append(Token(type, text, literal, self.line))

    def string(self):
        while self.peek() != '"' and not self.is_at_end():
            if self.peek() == '\n':
                self.line += 1
            self.advance()
        if self.is_at_end():
            self.error("Unterminated string.")
            return
        self.advance()  # The closing ".
        value = self.source[self.start + 1:self.current - 1]
        self.add_token(TokenType.STRING, value)

    def number(self):
        is_float = False
        while self.peek().isdigit():
            self.advance()
        if self.peek() == '.' and self.peek_next().isdigit():
            is_float = True
            self.advance()
            while self.peek().isdigit():
                self.advance()
        text = self.source[self.start:self.current]
        self.add_token(TokenType.NUMBER,
                       float(text) if is_float else int(text))

    def identifier(self):
        while self.peek().isalnum() or self.peek() == '_':
            self.advance()
        text = self.source[self.start:self.current]
        token_type = KEYWORDS.get(text, TokenType.IDENTIFIER)
        literal = None
        if token_type == TokenType.TRUE:
            literal = True
        elif token_type == TokenType.FALSE:
            literal = False
        self.add_token(token_type, literal)

    def scan_token(self):
        c = self.advance()
        if c in ' \r\t':
            return
        if c == '\n':
            self.line += 1
            return

        if c == '(':
            self.add_token(TokenType.LPAREN)
        elif c == ')':
            self.add_token(TokenType.RPAREN)
        elif c == '{':
            self.add_token(TokenType.LBRACE)
        elif c == '}':
            self.add_token(TokenType.RBRACE)
        elif c == '[':
            self.add_token(TokenType.LBRACKET)
        elif c == ']':
            self.add_token(TokenType.RBRACKET)
        elif c == ',':
            self.add_token(TokenType.COMMA)
        elif c == '.':
            self.add_token(TokenType.DOT)
        elif c == '-':
            self.add_token(TokenType.MINUS)
        elif c == '+':
            self.add_token(TokenType.PLUS)
        elif c == ';':
            self.add_token(TokenType.SEMICOLON)
        elif c == '*':
            self.add_token(TokenType.MULTIPLY)
        elif c == '!:
            self.add_token(
                TokenType.NOT_EQUAL if self.match('=') else TokenType.NOT)
        elif c == '=':
            self.add_token(
                TokenType.EQUAL if self.match('=') else TokenType.ASSIGN)
        elif c == '<':
            self.add_token(
                TokenType.LESS_EQUAL if self.match('=') else TokenType.LESS)
        elif c == '>':
            self.add_token(
                TokenType.GREATER_EQUAL
                if self.match('=') else TokenType.GREATER)
        elif c == '|':
            if self.match('|'):
                self.add_token(TokenType.OR)
            else:
                self.error("Unexpected character: |")
        elif c == '/':
            if self.match('/'):
                # A comment goes until the end of the line.
                while self.peek() != '\n' and not self.is_at_end():
                    self.advance()
            else:
                self.add_token(TokenType.DIVIDE)
        elif c == '&':
            if self.match('&'):
                self.add_token(TokenType.AND)
            else:
                self.error("Unexpected character: &")
        elif c == '"':
            self.string()
        elif c == "'":
            char_val = self.advance()
            if self.advance() != "'":
                self.error("Unterminated character literal.")
            else:
                self.add_token(TokenType.CHAR, char_val)
        elif c.isdigit():
            self.number()
        elif c.isalpha() or c == '_':
            self.identifier()
        else:
            self.error(f"Unexpected character: {c}")

    def error(self, message):
        # The batch script expects errors on stdout
        print(f"Lexer Error in {self.filename} at line {self.line}: {message}",
              file=sys.stdout)


# =============================================================================
# 2. ABSTRACT SYNTAX TREE (AST) NODES
# =============================================================================

class Stmt:
    class Visitor:
        def visit_block_stmt(self, stmt): raise NotImplementedError
        def visit_expressionstmt_stmt(self, stmt): raise NotImplementedError
        def visit_function_stmt(self, stmt): raise NotImplementedError
        def visit_if_stmt(self, stmt): raise NotImplementedError
        def visit_return_stmt(self, stmt): raise NotImplementedError
        def visit_while_stmt(self, stmt): raise NotImplementedError
        def visit_import_stmt(self, stmt): raise NotImplementedError
        def visit_assert_stmt(self, stmt): raise NotImplementedError
        def visit_var_stmt(self, stmt): raise NotImplementedError

    def accept(self, visitor):
        method_name = f'visit_{self.__class__.__name__.lower()}_stmt'
        return getattr(visitor, method_name)(self)


class Expr:
    class Visitor:
        def visit_assign_expr(self, expr): raise NotImplementedError
        def visit_binary_expr(self, expr): raise NotImplementedError
        def visit_call_expr(self, expr): raise NotImplementedError
        def visit_grouping_expr(self, expr): raise NotImplementedError
        def visit_literal_expr(self, expr): raise NotImplementedError
        def visit_logical_expr(self, expr): raise NotImplementedError
        def visit_unary_expr(self, expr): raise NotImplementedError
        def visit_variable_expr(self, expr): raise NotImplementedError
        def visit_listliteral_expr(self, expr): raise NotImplementedError
        def visit_indexget_expr(self, expr): raise NotImplementedError
        def visit_indexset_expr(self, expr): raise NotImplementedError

    def accept(self, visitor):
        method_name = f'visit_{self.__class__.__name__.lower()}_expr'
        return getattr(visitor, method_name)(self)


# Expressions
class Assign(Expr):
    def __init__(self, name, value):
        self.name = name
        self.value = value


class Binary(Expr):
    def __init__(self, left, operator, right):
        self.left = left
        self.operator = operator
        self.right = right


class Call(Expr):
    def __init__(self, callee, paren, arguments):
        self.callee = callee
        self.paren = paren
        self.arguments = arguments


class Grouping(Expr):
    def __init__(self, expression):
        self.expression = expression


class Literal(Expr):
    def __init__(self, value):
        self.value = value


class ListLiteral(Expr):
    def __init__(self, elements):
        self.elements = elements

class IndexGet(Expr):
    def __init__(self, object, bracket, index):
        self.object = object
        self.bracket = bracket
        self.index = index

class IndexSet(Expr):
    def __init__(self, object, bracket, index, value):
        self.object = object
        self.bracket = bracket
        self.index = index
        self.value = value


class Logical(Expr):
    def __init__(self, left, operator, right):
        self.left = left
        self.operator = operator
        self.right = right


class Unary(Expr):
    def __init__(self, operator, right):
        self.operator = operator
        self.right = right


class Variable(Expr):
    def __init__(self, name):
        self.name = name


# Statements
class Block(Stmt):
    def __init__(self, statements):
        self.statements = statements


class ExpressionStmt(Stmt):
    def __init__(self, expression):
        self.expression = expression


class Function(Stmt):
    def __init__(self, name, params, body):
        self.name = name
        self.params = params
        self.body = body


class If(Stmt):
    def __init__(self, condition, then_branch, else_branch):
        self.condition = condition
        self.then_branch = then_branch
        self.else_branch = else_branch


class Return(Stmt):
    def __init__(self, keyword, value):
        self.keyword = keyword
        self.value = value


class While(Stmt):
    def __init__(self, condition, body):
        self.condition = condition
        self.body = body


class Import(Stmt):
    def __init__(self, path_token, namespace):
        self.path_token = path_token
        self.namespace = namespace


class Assert(Stmt):
    def __init__(self, keyword, condition, message):
        self.keyword = keyword
        self.condition = condition
        self.message = message


class Var(Stmt):
    def __init__(self, name, initializer):
        self.name = name
        self.initializer = initializer


# =============================================================================
# 3. PARSER
# =============================================================================

class ParseError(Exception):
    pass


class Parser:
    def __init__(self, tokens, filename="<unknown>"):
        self.tokens = tokens
        self.filename = filename
        self.current = 0

    def parse(self):
        statements = []
        while not self.is_at_end():
            stmt = self.declaration()
            if stmt is not None:
                statements.append(stmt)
        return statements

    def is_at_end(self):
        return self.peek().type == TokenType.EOF

    def peek(self):
        return self.tokens[self.current]

    def peek_next(self):
        if self.current + 1 >= len(self.tokens):
            return self.tokens[len(self.tokens) - 1]
        return self.tokens[self.current + 1]

    def previous(self):
        return self.tokens[self.current - 1]

    def advance(self):
        if not self.is_at_end():
            self.current += 1
        return self.previous()

    def check(self, type):
        if self.is_at_end():
            return False
        return self.peek().type == type

    def match(self, *types):
        for t in types:
            if self.check(t):
                self.advance()
                return True
        return False

    def consume(self, type, message):
        if self.check(type):
            return self.advance()
        raise ParseError()

    def synchronize(self):
        self.advance()
        while not self.is_at_end():
            if self.previous().type == TokenType.SEMICOLON:
                return
            if self.peek().type in (TokenType.FUNCTION, TokenType.WHILE, TokenType.FOR,
                                     TokenType.IF, TokenType.RETURN, TokenType.PRINT):
                return
            self.advance()

    def declaration(self):
        try:
            if self.match(TokenType.FUNCTION):
                return self.function_declaration()
            if self.match(TokenType.VAR):
                return self.var_declaration()
            return self.statement()
        except ParseError:
            self.synchronize()
            return None

    def var_declaration(self):
        name = self.consume(TokenType.IDENTIFIER, "Expect variable name.")
        initializer = None
        if self.match(TokenType.ASSIGN):
            initializer = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after variable declaration.")
        return Var(name, initializer)

    def function_declaration(self):
        name = self.consume(TokenType.IDENTIFIER, "Expect function name.")
        self.consume(TokenType.LPAREN, "Expect '(' after function name.")

        params = []
        if not self.check(TokenType.RPAREN):
            while True:
                if len(params) >= 255:
                    self.error(self.peek(), "Can't have more than 255 parameters.")
                params.append(self.consume(TokenType.IDENTIFIER, "Expect parameter name."))
                if not self.match(TokenType.COMMA):
                    break
        self.consume(TokenType.RPAREN, "Expect ')' after parameters.")

        self.consume(TokenType.LBRACE, "Expect '{' before function body.")
        body = self.block()
        return Function(name, params, body)

    def statement(self):
        if self.match(TokenType.ASSERT):
            return self.assert_statement()
        if self.match(TokenType.IMPORT):
            return self.import_statement()
        if self.match(TokenType.LBRACE):
            return Block(self.block())
        if self.match(TokenType.RETURN):
            return self.return_statement()
        if self.match(TokenType.WHILE):
            return self.while_statement()
        if self.match(TokenType.FOR):
            return self.for_statement()
        if self.match(TokenType.IF):
            return self.if_statement()

        return self.expression_statement()

    def assert_statement(self):
        keyword = self.previous()
        condition = self.expression()
        self.consume(TokenType.COMMA, "Expect ',' after assert condition.")
        message = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after assert message.")
        return Assert(keyword, condition, message)

    def import_statement(self):
        namespace = None
        if (self.check(TokenType.IDENTIFIER) and
            self.peek_next().type in (TokenType.COMMA, TokenType.SEMICOLON)):
            namespace = self.consume(TokenType.IDENTIFIER, "Expect namespace for import.")
        path_token = self.consume(TokenType.STRING, "Expect string path for import.")
        self.consume(TokenType.SEMICOLON, "Expect ';' after import statement.")
        return Import(path_token, namespace)

    def expression_statement(self):
        expr = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after expression.")
        return ExpressionStmt(expr)

    def return_statement(self):
        keyword = self.previous()
        value = None
        if not self.check(TokenType.SEMICOLON):
            value = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after return value.")
        return Return(keyword, value)

    def while_statement(self):
        self.consume(TokenType.LPAREN, "Expect '(' after 'while'.")
        condition = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')' after condition.")
        body = self.statement()
        return While(condition, body)

    def for_statement(self):
        self.consume(TokenType.LPAREN, "Expect '(' after 'for'.")

        # For loop variable
        initializer = None
        if self.match(TokenType.VAR):
            initializer = self.var_declaration()
        elif not self.check(TokenType.SEMICOLON):
            initializer = self.expression_statement()
        self.consume(TokenType.SEMICOLON, "Expect ';' after initializer.")

        # For loop condition
        condition = None
        if not self.check(TokenType.SEMICOLON):
            condition = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after for loop condition.")

        # For loop increment
        increment = None
        if not self.check(TokenType.RPAREN):
            increment = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')' after for clauses.")

        body = self.statement()

        if increment is not None:
            body = Block([body, ExpressionStmt(increment)])

        if condition is None:
            condition = Literal(True)

        body = While(condition, body)

        if initializer is not None:
            body = Block([initializer, body])

        return body

    def if_statement(self):
        self.consume(TokenType.LPAREN, "Expect '(' after 'if'.")
        condition = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')' after if condition.")

        then_branch = self.statement()
        else_branch = None
        if self.match(TokenType.ELSE):
            else_branch = self.statement()

        return If(condition, then_branch, else_branch)

    def block(self):
        statements = []
        while not self.check(TokenType.RBRACE) and not self.is_at_end():
            stmt = self.declaration()
            if stmt is not None:
                statements.append(stmt)
        self.consume(TokenType.RBRACE, "Expect '}' after block.")
        return statements

    def expression(self):
        return self.assignment()

    def assignment(self):
        expr = self.logical_or()

        if self.match(TokenType.ASSIGN):
            equals = self.previous()
            value = self.assignment()

            if isinstance(expr, Variable):
                name = expr.name
                return Assign(name, value)

            self.error(equals, "Invalid assignment target.")

        return expr

    def logical_or(self):
        expr = self.logical_and()
        while self.match(TokenType.OR):
            operator = self.previous()
            right = self.logical_and()
            expr = Logical(expr, operator, right)
        return expr

    def logical_and(self):
        expr = self.equality()
        while self.match(TokenType.AND):
            operator = self.previous()
            right = self.equality()
            expr = Logical(expr, operator, right)
        return expr

    def equality(self):
        expr = self.comparison()
        while self.match(TokenType.NOT_EQUAL, TokenType.EQUAL):
            operator = self.previous()
            right = self.comparison()
            expr = Binary(expr, operator, right)
        return expr

    def comparison(self):
        expr = self.term()
        while self.match(TokenType.GREATER, TokenType.GREATER_EQUAL,
                        TokenType.LESS, TokenType.LESS_EQUAL):
            operator = self.previous()
            right = self.term()
            expr = Binary(expr, operator, right)
        return expr

    def term(self):
        expr = self.factor()
        while self.match(TokenType.PLUS, TokenType.MINUS):
            operator = self.previous()
            right = self.factor()
            expr = Binary(expr, operator, right)
        return expr

    def factor(self):
        expr = self.unary()
        while self.match(TokenType.MULTIPLY, TokenType.DIVIDE):
            operator = self.previous()
            right = self.unary()
            expr = Binary(expr, operator, right)
        return expr

    def unary(self):
        if self.match(TokenType.NOT, TokenType.MINUS):
            operator = self.previous()
            right = self.unary()
            return Unary(operator, right)

        return self.call()

    def call(self):
        expr = self.primary()
        while True:
            if self.match(TokenType.LPAREN):
                arguments = []
                if not self.check(TokenType.RPAREN):
                    while True:
                        if len(arguments) >= 255:
                            self.error(self.peek(), "Can't have more than 255 arguments.")
                        arguments.append(self.expression())
                        if not self.match(TokenType.COMMA):
                            break
                paren = self.consume(TokenType.RPAREN, "Expect ')' after arguments.")
                expr = Call(expr, paren, arguments)
            else:
                break
        return expr

    def primary(self):
        if self.match(TokenType.NUMBER, TokenType.FALSE, TokenType.TRUE, TokenType.NIL):
            return Literal(self.previous().literal)

        if self.match(TokenType.STRING):
            return Literal(self.previous().literal)
        if self.match(TokenType.TRUE): return Literal(True)
        if self.match(TokenType.FALSE): return Literal(False)

        if self.match(TokenType.LPAREN):
            expr = self.expression()
            self.consume(TokenType.RPAREN, "Expect ')' after expression.")
            return Grouping(expr)

        if self.match(TokenType.IDENTIFIER):
            return Variable(self.previous())

        if self.match(TokenType.LBRACKET):
            elements = []
            if not self.check(TokenType.RBRACKET):
                while True:
                    elements.append(self.expression())
                    if not self.match(TokenType.COMMA):
                        break
            self.consume(TokenType.RBRACKET, "Expect ']' after list elements.")
            return ListLiteral(elements)

        raise self.error(self.peek(), "Expect expression.")

    def consume(self, type, message):
        if self.check(type):
            return self.advance()
        raise ParseError()

    def error(self, token, message):
        if token.type == TokenType.EOF:
            print(f"Parse Error at end: {message}", file=sys.stdout)
        else:
            print(f"Parse Error at '{token.lexeme}': {message}", file=sys.stdout)
        return ParseError()

    def synchronize(self):
        self.advance()
        while not self.is_at_end():
            if self.previous().type == TokenType.SEMICOLON:
                return
            if self.peek().type in (TokenType.FUNCTION, TokenType.WHILE, TokenType.FOR,
                                     TokenType.IF, TokenType.RETURN, TokenType.PRINT):
                return
            self.advance()


# =============================================================================
# 4. BUILT-IN FUNCTIONS
# =============================================================================

class MiniScriptCallable:
    def arity(self):
        raise NotImplementedError()
    def call(self, interpreter, arguments):
        raise NotImplementedError()

class BuiltinPrint(MiniScriptCallable):
    def arity(self):
        return -1  # Variadic
    def call(self, interpreter, arguments):
        print(*[interpreter.stringify(arg) for arg in arguments])
        return None
    def __str__(self):
        return "<native fn>"

class BuiltinLen(MiniScriptCallable):
    def arity(self):
        return 1
    def call(self, interpreter, arguments):
        arg = arguments[0]
        if isinstance(arg, str):
            return len(arg)
        if isinstance(arg, list):
            return len(arg)
        raise RuntimeError(None, "len() expects a string or a list.")
    def __str__(self):
        return "<native fn>"

class BuiltinTimeNow(MiniScriptCallable):
    def arity(self):
        return 0
    def call(self, interpreter, arguments):
        return time.time()
    def __str__(self):
        return "<native fn>"

class BuiltinTimeFormat(MiniScriptCallable):
    def arity(self):
        return 2
    def call(self, interpreter, arguments):
        timestamp, fmt = arguments
        if not isinstance(timestamp, (int, float)):
            raise RuntimeError(None, "time_format() expects a numeric timestamp as the first argument.")
        if not isinstance(fmt, str):
            raise RuntimeError(None, "time_format() expects a format string as the second argument.")
        try:
            return time.strftime(fmt, time.localtime(timestamp))
        except ValueError:
            # Handle potential errors with invalid format strings or timestamps
            raise RuntimeError(None, "Invalid timestamp or format string for time_format().")
    def __str__(self):
        return "<native fn>"

class BuiltinTimeParse(MiniScriptCallable):
    def arity(self):
        return 2
    def call(self, interpreter, arguments):
        time_str, fmt = arguments
        if not isinstance(time_str, str) or not isinstance(fmt, str):
            raise RuntimeError(None, "time_parse() expects two string arguments.")
        try:
            dt = datetime.strptime(time_str, fmt)
            return time.mktime(dt.timetuple())
        except ValueError:
            return -1 # Return -1 on parsing failure as per spec
    def __str__(self):
        return "<native fn>"

class BuiltinTimeDiff(MiniScriptCallable):
    def arity(self):
        return 2
    def call(self, interpreter, arguments):
        t1, t2 = arguments
        if not isinstance(t1, (int, float)) or not isinstance(t2, (int, float)):
            raise RuntimeError(None, "time_diff() expects two numeric timestamps.")
        return t1 - t2
    def __str__(self):
        return "<native fn>"

class BuiltinSleep(MiniScriptCallable):
    def arity(self):
        return 1
    def call(self, interpreter, arguments):
        duration = arguments[0]
        if not isinstance(duration, (int, float)):
            raise RuntimeError(None, "sleep() expects a numeric duration in seconds.")
        time.sleep(duration)
        return None
    def __str__(self):
        return "<native fn>"


# =============================================================================
# 5. INTERPRETER
# =============================================================================

class RuntimeError(Exception):
    def __init__(self, token, message):
        self.token = token
        self.message = message
        super().__init__(self.message)


class ReturnValue(Exception):
    """Used to unwind the stack for return statements."""
    def __init__(self, value):
        self.value = value


class Environment:
    def __init__(self, enclosing=None):
        self.values = {}
        self.enclosing = enclosing

    def define(self, name, value):
        self.values[name] = value

    def get(self, name_token):
        name = name_token.lexeme
        if name in self.values:
            return self.values[name]
        if self.enclosing is not None:
            return self.enclosing.get(name_token)
        raise RuntimeError(name_token, f"Undefined variable '{name}'.")

    def assign(self, name_token, value):
        name = name_token.lexeme
        # MiniScript uses implicit declaration, assigning to the global scope
        # if the variable is not found in the current scope chain.
        env = self
        while env is not None:
            if name in env.values:
                env.values[name] = value
                return
            if env.enclosing is None:
                break
            env = env.enclosing
        env.values[name] = value





class MiniScriptFunction(MiniScriptCallable):
    def __init__(self, declaration, closure):
        self.declaration = declaration
        self.closure = closure

    def arity(self):
        return len(self.declaration.params)

    def call(self, interpreter, arguments):
        environment = Environment(self.closure)
        for i, param in enumerate(self.declaration.params):
            environment.define(param.lexeme, arguments[i])

        try:
            interpreter.execute_block(self.declaration.body, environment)
        except ReturnValue as ret:
            return ret.value

        return None

    def __str__(self):
        return f"<fn {self.declaration.name.lexeme}>"


class Interpreter(Expr.Visitor, Stmt.Visitor):
    def __init__(self, filename="<unknown>"):
        self.filename = filename
        self.globals = Environment()
        self.environment = self.globals
        self.locals = {}
        self.modules_path = [os.getcwd()]  # Start with current directory

        # Built-in functions
        self.globals.define("print", BuiltinPrint())
        self.globals.define("len", BuiltinLen())
        self.globals.define("time_now", BuiltinTimeNow())
        self.globals.define("time_format", BuiltinTimeFormat())
        self.globals.define("time_parse", BuiltinTimeParse())
        self.globals.define("time_diff", BuiltinTimeDiff())
        self.globals.define("sleep", BuiltinSleep())

    def interpret(self, statements):
        try:
            for statement in statements:
                self.execute(statement)
        except RuntimeError as error:
            # The batch script expects errors on stdout
            print(f"Error in {self.filename} at line {error.token.line if error.token else 'unknown'}: {error.message}",
                  file=sys.stdout)

    def execute(self, stmt):
        stmt.accept(self)

    def execute_block(self, statements, environment):
        previous = self.environment
        try:
            self.environment = environment
            for statement in statements:
                self.execute(statement)
        finally:
            self.environment = previous

    def visit_var_stmt(self, stmt):
        value = None
        if stmt.initializer is not None:
            value = self.evaluate(stmt.initializer)
        self.environment.define(stmt.name.lexeme, value)

    def visit_block_stmt(self, stmt):
        self.execute_block(stmt.statements, Environment(self.environment))

    def visit_expressionstmt_stmt(self, stmt):
        self.evaluate(stmt.expression)

    def visit_function_stmt(self, stmt):
        function = MiniScriptFunction(stmt, self.environment)
        self.environment.define(stmt.name.lexeme, function)

    def visit_if_stmt(self, stmt):
        if self.is_truthy(self.evaluate(stmt.condition)):
            self.execute(stmt.then_branch)
        elif stmt.else_branch is not None:
            self.execute(stmt.else_branch)

    def visit_return_stmt(self, stmt):
        value = None
        if stmt.value is not None:
            value = self.evaluate(stmt.value)
        raise ReturnValue(value)

    def visit_while_stmt(self, stmt):
        while self.is_truthy(self.evaluate(stmt.condition)):
            self.execute(stmt.body)

    def visit_assert_stmt(self, stmt):
        condition_val = self.evaluate(stmt.condition)
        if not self.is_truthy(condition_val):
            message_val = self.evaluate(stmt.message)
            raise RuntimeError(
                stmt.keyword,
                f"Assertion failed: {self.stringify(message_val)}")

    def visit_import_stmt(self, stmt):
        module_path = stmt.path_token.literal
        full_path = None

        # --- Path Resolution Logic ---
        search_paths = []
        # 1. Path relative to the current script file (if not in REPL)
        if self.filename != "<REPL>" and self.filename != "<unknown>":
            script_dir = os.path.dirname(os.path.abspath(self.filename))
            if script_dir not in search_paths:
                search_paths.append(script_dir)

        # 2. Current working directory
        cwd = os.getcwd()
        if cwd not in search_paths:
            search_paths.append(cwd)

        # 3. Paths from MODULESPATH environment variable
        modules_path_env = os.getenv("MODULESPATH")
        if modules_path_env:
            for p in modules_path_env.split(';'):
                if p not in search_paths:
                    search_paths.append(p)

        found_path = None
        for base_dir in search_paths:
            # Try the path as is
            test_path = os.path.join(base_dir, module_path)
            if os.path.exists(test_path) and os.path.isfile(test_path):
                found_path = test_path
                break
            # Try adding .ms extension
            if not module_path.endswith(".ms"):
                test_path_ext = test_path + ".ms"
                if os.path.exists(test_path_ext) and os.path.isfile(test_path_ext):
                    found_path = test_path_ext
                    break
        
        if not found_path:
            self.error(stmt.path_token, f"Cannot find module: {stmt.path_token.literal}")
            return

        # Execute module in the current interpreter context
        run_file(found_path, interpreter_instance=self)


    # Expression Visitors
    def visit_literal_expr(self, expr):
        return expr.value

    def visit_listliteral_expr(self, expr):
        elements = [self.evaluate(elem) for elem in expr.elements]
        return elements

    def visit_indexget_expr(self, expr):
        obj = self.evaluate(expr.object)
        index = self.evaluate(expr.index)

        if not isinstance(obj, list):
            self.error(expr.bracket, "Can only index lists.")
        if not isinstance(index, int):
            self.error(expr.bracket, "List index must be an integer.")
        
        if not 0 <= index < len(obj):
            self.error(expr.bracket, f"Index {index} out of bounds for list of size {len(obj)}.")

        return obj[index]

    def visit_indexset_expr(self, expr):
        obj = self.evaluate(expr.object)
        index = self.evaluate(expr.index)
        value = self.evaluate(expr.value)

        if not isinstance(obj, list):
            self.error(expr.bracket, "Can only index lists.")
        if not isinstance(index, int):
            self.error(expr.bracket, "List index must be an integer.")
        
        if not 0 <= index < len(obj):
            self.error(expr.bracket, f"Index {index} out of bounds for list of size {len(obj)}.")

        obj[index] = value
        return value

    def visit_variable_expr(self, expr):
        return self.environment.get(expr.name)

    def visit_assign_expr(self, expr):
        value = self.evaluate(expr.value)
        self.environment.assign(expr.name, value)
        return value

    def visit_grouping_expr(self, expr):
        return self.evaluate(expr.expression)

    def visit_unary_expr(self, expr):
        right = self.evaluate(expr.right)
        op_type = expr.operator.type

        if op_type == TokenType.MINUS:
            self.check_number_operand(expr.operator, right)
            return -float(right)
        if op_type == TokenType.NOT:
            return not self.is_truthy(right)

        return None  # Unreachable

    def visit_logical_expr(self, expr):
        left = self.evaluate(expr.left)

        if expr.operator.type == TokenType.OR:
            if self.is_truthy(left): return True
        else: # AND
            if not self.is_truthy(left): return False

        return self.is_truthy(self.evaluate(expr.right))

    def visit_binary_expr(self, expr):
        left = self.evaluate(expr.left)
        right = self.evaluate(expr.right)
        op_type = expr.operator.type

        # Arithmetic
        if op_type == TokenType.PLUS:
            if (isinstance(left, (int, float)) and
                    isinstance(right, (int, float))):
                return float(left) + float(right)
            # MiniScript allows string concatenation with any type
            return self.stringify(left) + self.stringify(right)
        if op_type == TokenType.MINUS:
            self.check_number_operands(expr.operator, left, right)
            return float(left) - float(right)
        if op_type == TokenType.MULTIPLY:
            self.check_number_operands(expr.operator, left, right)
            return float(left) * float(right)
        if op_type == TokenType.DIVIDE:
            self.check_number_operands(expr.operator, left, right)
            if float(right) == 0:
                self.error(expr.operator, "Division by zero.")
            return float(left) / float(right)

        # Comparison
        if op_type == TokenType.GREATER:
            self.check_number_operands(expr.operator, left, right)
            return float(left) > float(right)
        if op_type == TokenType.GREATER_EQUAL:
            self.check_number_operands(expr.operator, left, right)
            return float(left) >= float(right)
        if op_type == TokenType.LESS:
            self.check_number_operands(expr.operator, left, right)
            return float(left) < float(right)
        if op_type == TokenType.LESS_EQUAL:
            self.check_number_operands(expr.operator, left, right)
            return float(left) <= float(right)

        # Equality
        if op_type == TokenType.EQUAL:
            return self.is_equal(left, right)
        if op_type == TokenType.NOT_EQUAL:
            return not self.is_equal(left, right)

        return None  # Unreachable

    def visit_call_expr(self, expr):
        callee = self.evaluate(expr.callee)
        arguments = [self.evaluate(arg) for arg in expr.arguments]

        if not isinstance(callee, MiniScriptCallable):
            self.error(expr.paren, "Can only call functions and classes.")

        if callee.arity() != -1 and len(arguments) != callee.arity():
            self.error(
                expr.paren,
                f"Expected {callee.arity()} args but got {len(arguments)}.")

        return callee.call(self, arguments)

    def evaluate(self, expr):
        return expr.accept(self)

    # Helper methods
    def is_truthy(self, obj):
        if obj is None:
            return False
        if isinstance(obj, bool):
            return obj
        if isinstance(obj, (int, float)):
            return obj != 0
        return True

    def is_equal(self, a, b):
        if a is None and b is None:
            return True
        if a is None:
            return False
        return a == b

    def check_number_operand(self, operator, operand):
        if isinstance(operand, (int, float)):
            return
        self.error(operator, "Operand must be a number.")

    def check_number_operands(self, operator, left, right):
        if isinstance(left, (int, float)) and isinstance(right, (int, float)):
            return
        self.error(operator, "Operands must be numbers.")

    def error(self, token, message):
        raise RuntimeError(token, message)

    def stringify(self, obj):
        if obj is None: return "nil"
        if isinstance(obj, float):
            text = str(obj)
            if text.endswith(".0"):
                text = text[:-2]
            return text
        if isinstance(obj, bool):
            return "true" if obj else "false"
        return str(obj)


# =============================================================================
# 6. MAIN SCRIPT LOGIC
# =============================================================================

def run(source, filename, interpreter_instance=None):
    """Runs a mini-script program."""
    is_main_run = interpreter_instance is None
    if is_main_run:
        interpreter_instance = Interpreter(filename)
    
    # Update filename for correct relative path resolution in imports
    # and error messages.
    original_filename = interpreter_instance.filename
    interpreter_instance.filename = filename

    lexer = Lexer(source, filename)
    tokens = lexer.scan_tokens()

    parser = Parser(tokens, filename)
    statements = parser.parse()

    if any(s is None for s in statements):
        # Errors already printed by parser
        interpreter_instance.filename = original_filename # Restore filename
        return

    interpreter_instance.interpret(statements)
    
    # Restore original filename after execution
    interpreter_instance.filename = original_filename


def run_file(path, interpreter_instance=None):
    """Reads a file and runs it."""
    is_main_run = interpreter_instance is None
    if is_main_run:
        print("Mini Script Language Interpreter")
        print("=================================")
        try:
            # Make path relative to current working directory for output
            display_path = os.path.relpath(path)
        except ValueError:
            # This can happen on Windows if the path is on a different drive
            # In that case, we'll just use the original path.
            display_path = path
        
        # Normalize path separators for consistency
        display_path = display_path.replace('\\', '/')

        print(f"Executing: {display_path}")
        print("---------------------------------")
        print()

    try:
        abs_path = os.path.abspath(path)
        with open(abs_path, 'r', encoding='utf-8') as f:
            source = f.read()
        run(source, abs_path, interpreter_instance)
    except FileNotFoundError:
        # The batch script expects errors on stdout
        print(f"Error: Could not open file '{path}'", file=sys.stdout)
        if is_main_run:
            sys.exit(1)
    except RuntimeError:
        # Runtime errors are already printed by the interpreter
        if is_main_run:
            sys.exit(1)


def run_repl():
    """Starts an interactive Read-Eval-Print Loop."""
    global main_interpreter
    main_interpreter = Interpreter("<REPL>")

    print("Mini Script Language (Python Version) - REPL")
    print("Enter 'exit' to quit.")
    line_num = 1
    while True:
        try:
            line = input(f"ms[{line_num}]> ")
            if line.lower() in ['exit', 'quit']:
                print("Goodbye!")
                break
            if not line:
                continue

            # In REPL, we can treat everything as an expression and print
            # the result, or handle statements gracefully. This is a
            # simple version.
            try:
                # Try parsing as a statement
                source = line
                if not source.strip().endswith((';', '{', '}')):
                    source += ';'

                lexer = Lexer(source, "<REPL>")
                tokens = lexer.scan_tokens()
                parser = Parser(tokens, "<REPL>")
                stmt = parser.declaration()

                # If it's an expression statement, evaluate and print
                if isinstance(stmt, ExpressionStmt):
                    result = main_interpreter.evaluate(stmt.expression)
                    if result is not None:
                        print(f"=> {main_interpreter.stringify(result)}")
                elif stmt:  # It's another kind of statement
                    main_interpreter.execute(stmt)

            except (ParseError, RuntimeError):
                # Errors are already printed, just continue
                pass

            line_num += 1
        except EOFError:
            print("\nGoodbye!")
            break
        except Exception as e:
            print(f"An unexpected error occurred: {e}", file=sys.stdout)


def main():
    if len(sys.argv) > 2:
        print("Usage: mini-script.py [script_file]")
        sys.exit(1)
    elif len(sys.argv) == 2:
        run_file(sys.argv[1])
    else:
        run_repl()


if __name__ == "__main__":
    main()
