CC = gcc
BASE_CFLAGS = -Wall -Wextra -std=c99
DEBUG_CFLAGS = -g
ASAN_CFLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -g
CFLAGS = $(BASE_CFLAGS) $(DEBUG_CFLAGS)
TARGET = mini_script
SOURCES = main.c lexer.c value.c environment.c parser.c interpreter.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# AddressSanitizer / UBSan build
asan: CFLAGS = $(BASE_CFLAGS) $(ASAN_CFLAGS)
asan: clean $(TARGET)
	@echo "Built with AddressSanitizer (asan)"

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

.PHONY: all clean rebuild test asan
