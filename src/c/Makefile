CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = mini_script
SOURCES = main.c lexer.c value.c environment.c parser.c interpreter.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) -lm

# Build object files
%.o: %.c mini_script.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run tests
test: $(TARGET)
	@echo "Running basic test..."
	@echo 'print "Hello, World!";' | ./$(TARGET)

.PHONY: all clean rebuild test
