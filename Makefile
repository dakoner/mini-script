### Top-level Makefile (delegates to src/c implementation)

.PHONY: all release debug clean test help

# Default: build C interpreter in src/c
all release:
	$(MAKE) -C src/c

# Debug just reuses the src/c Makefile (extend there if needed)
debug:
	$(MAKE) -C src/c

clean:
	$(MAKE) -C src/c clean || true
	rm -f mini_script

test: all
	./run_tests.sh

help:
	@echo "Targets: all (default), release, debug, clean, test, help"
	@echo "Builds delegate to src/c/Makefile"

.DEFAULT_GOAL := all
