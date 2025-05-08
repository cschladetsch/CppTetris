# Simple Makefile for Tetris game and tests

.PHONY: all clean build test game

# Default target
all: build

# Build everything
build:
	mkdir -p build
	cd build && cmake .. && cmake --build .

# Run the game
game: build
	./build/tetris

# Run tests
test: build
	cd build && ctest

# Run tests with mock mode (fewer error messages)
test-mock: build
	./tests/run_mock_tests.sh

# Clean build artifacts
clean:
	rm -rf build