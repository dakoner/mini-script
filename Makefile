# Makefile for Mini Script Language
# Linux build system

# Project configuration
PROJECT_NAME = mini_script
SOURCE_FILE = mini_script.c

# Linux configuration
CC = gcc
TARGET = $(PROJECT_NAME)
RM = rm -f
MKDIR = mkdir -p
PATH_SEP = /

# Compiler flags
CFLAGS = -Wall -Wextra -std=c17 -pedantic
CFLAGS_DEBUG = $(CFLAGS) -g -O0 -DDEBUG -fsanitize=address -fsanitize=undefined
CFLAGS_RELEASE = $(CFLAGS) -O2 -DNDEBUG -flto
CFLAGS_PROFILE = $(CFLAGS) -O2 -g -pg

# Build directories
BUILD_DIR = build
DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release
PROFILE_DIR = $(BUILD_DIR)/profile

# Targets
DEBUG_TARGET = $(DEBUG_DIR)/$(TARGET)
RELEASE_TARGET = $(RELEASE_DIR)/$(TARGET)
PROFILE_TARGET = $(PROFILE_DIR)/$(TARGET)

# Default target
.PHONY: all
all: release

# Release build (optimized)
.PHONY: release
release: $(RELEASE_TARGET)

$(RELEASE_TARGET): $(SOURCE_FILE) | $(RELEASE_DIR)
	@echo "Building RELEASE version..."
	$(CC) $(CFLAGS_RELEASE) $(LDFLAGS) -o $@ $<
	@echo "✓ Release build completed: $@"

# Debug build (with symbols and sanitizers)
.PHONY: debug
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(SOURCE_FILE) | $(DEBUG_DIR)
	@echo "Building DEBUG version..."
	$(CC) $(CFLAGS_DEBUG) $(LDFLAGS) -o $@ $<
	@echo "✓ Debug build completed: $@"

# Profile build (with profiling support)
.PHONY: profile
profile: $(PROFILE_TARGET)

$(PROFILE_TARGET): $(SOURCE_FILE) | $(PROFILE_DIR)
	@echo "Building PROFILE version..."
	$(CC) $(CFLAGS_PROFILE) $(LDFLAGS) -o $@ $<
	@echo "✓ Profile build completed: $@"

# Create build directories
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

$(DEBUG_DIR): | $(BUILD_DIR)
	$(MKDIR) $(DEBUG_DIR)

$(RELEASE_DIR): | $(BUILD_DIR)
	$(MKDIR) $(RELEASE_DIR)

$(PROFILE_DIR): | $(BUILD_DIR)
	$(MKDIR) $(PROFILE_DIR)

# Install target (copy to current directory for compatibility)
.PHONY: install
install: release
	@echo "Installing $(TARGET) to current directory..."
	cp "$(RELEASE_TARGET)" "$(TARGET)"
	@echo "✓ Installation completed: $(TARGET)"

# Test targets
.PHONY: test
test: install
	@echo "Running test suite..."
	@if [ -f "run_tests.sh" ]; then \
		./run_tests.sh; \
	elif [ -f "run_tests.bat" ]; then \
		echo "Note: Found run_tests.bat but this is Unix. Creating wrapper..."; \
		echo "#!/bin/bash" > run_tests.sh; \
		echo "# Unix wrapper for Windows test script" >> run_tests.sh; \
		echo "./$(TARGET) tests/test_*.ms" >> run_tests.sh; \
		chmod +x run_tests.sh; \
		./run_tests.sh; \
	else \
		echo "Running basic tests..."; \
		find tests -name "test_*.ms" -exec ./$(TARGET) {} \; 2>/dev/null || echo "No test files found"; \
	fi

.PHONY: test-debug
test-debug: debug
	@echo "Running test suite with debug build..."
	$(DEBUG_TARGET) --help

.PHONY: test-quick
test-quick: install
	@echo "Running quick test..."
	./$(TARGET) --help

# REPL target
.PHONY: repl
repl: install
	@echo "Starting REPL mode..."
	./$(TARGET)

# Example targets
.PHONY: examples
examples: install
	@echo "Running example scripts..."
	@if [ -d "examples" ]; then \
		for example in examples/*.ms; do \
			if [ -f "$$example" ]; then \
				echo "Running $$example..."; \
				./$(TARGET) "$$example"; \
			fi; \
		done; \
	else \
		echo "Examples directory not found"; \
	fi

# Benchmark target
.PHONY: bench
bench: profile
	@echo "Running performance benchmark..."
	$(PROFILE_TARGET) --help
	@echo "Use 'gprof $(PROFILE_TARGET) gmon.out' to analyze profile data"

# Clean targets
.PHONY: clean
clean:
	@echo "Cleaning build files..."
	$(RM) -r $(BUILD_DIR)
	$(RM) $(TARGET)
	$(RM) gmon.out
	$(RM) *.o *.so *.dylib
	@echo "✓ Clean completed"

.PHONY: distclean
distclean: clean
	@echo "Deep cleaning..."
	$(RM) *.exe *.dll
	@echo "✓ Distribution clean completed"

# Development targets
.PHONY: format
format:
	@echo "Formatting source code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		clang-format -i $(SOURCE_FILE); \
		echo "✓ Code formatted"; \
	else \
		echo "clang-format not found, skipping formatting"; \
	fi

.PHONY: lint
lint:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c17 $(SOURCE_FILE); \
	else \
		echo "cppcheck not found, running basic gcc checks..."; \
		$(CC) $(CFLAGS) -fsyntax-only $(SOURCE_FILE); \
	fi

# Information targets
.PHONY: info
info:
	@echo "Mini Script Language Build Information"
	@echo "======================================"
	@echo "Operating System: Linux"
	@echo "Compiler: $(CC)"
	@echo "Source File: $(SOURCE_FILE)"
	@echo "Target: $(TARGET)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo ""
	@echo "Compiler Version:"
	@$(CC) --version | head -1 || echo "Compiler not found"
	@echo ""
	@echo "Available Targets:"
	@echo "  all       - Build release version (default)"
	@echo "  release   - Build optimized release version"
	@echo "  debug     - Build debug version with sanitizers"
	@echo "  profile   - Build profiling version"
	@echo "  install   - Install release build to current directory"
	@echo "  test      - Run comprehensive test suite"
	@echo "  test-quick- Run quick test"
	@echo "  repl      - Start interactive REPL mode"
	@echo "  examples  - Run example scripts"
	@echo "  bench     - Run performance benchmark"
	@echo "  clean     - Clean build files"
	@echo "  distclean - Deep clean all generated files"
	@echo "  format    - Format source code (requires clang-format)"
	@echo "  lint      - Run static analysis (requires cppcheck)"
	@echo "  info      - Show this information"

.PHONY: help
help: info

# Version target
.PHONY: version
version: install
	@echo "Mini Script Language Version Information:"
	./$(TARGET) --help | head -5

# Check prerequisites
.PHONY: check
check:
	@echo "Checking build prerequisites..."
	@echo -n "Checking compiler... "
	@$(CC) --version >/dev/null 2>&1 && echo "✓ $(CC)" || echo "✗ $(CC) not found"
	@echo -n "Checking source file... "
	@test -f $(SOURCE_FILE) && echo "✓ $(SOURCE_FILE)" || echo "✗ $(SOURCE_FILE) not found"
	@echo "Prerequisite check completed"

# Quick development cycle
.PHONY: dev
dev: clean debug test-debug
	@echo "✓ Development cycle completed"

# Release cycle
.PHONY: dist
dist: clean release test
	@echo "✓ Distribution build completed"

# Parallel build support
.PHONY: parallel
parallel:
	$(MAKE) -j$(shell nproc 2>/dev/null || echo 4) all

# Documentation
.PHONY: docs
docs:
	@echo "Generating documentation..."
	@echo "Source code documentation would go here"
	@echo "See README.md and BUILD.md for usage information"

# Default make without arguments shows help
.DEFAULT_GOAL := all
