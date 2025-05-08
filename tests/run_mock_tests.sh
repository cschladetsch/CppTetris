#!/bin/bash

# This script runs the tests in a simplified environment, suppressing SDL errors
# and allowing for better focus on the actual test results

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. || { echo "CMake configuration failed"; exit 1; }

# Build the project and tests
cmake --build . || { echo "Build failed"; exit 1; }

# Run the tests with more readable output, suppressing SDL initialization errors
echo "Running tests..."
echo "================"

# Run each test executable with a focus on the actual test results
cd tests
for test in tetromino_test tetromino_manager_test game_test grid_collision_test; do
  echo "Running $test:"
  ./$test 2>/dev/null | grep -E '(RUN|OK|\[|\]|Failure)' | grep -v "ALSA\|SDL_mixer\|audio"
  echo ""
done

echo "================"
echo "Test run complete"