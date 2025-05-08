#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. || { echo "CMake configuration failed"; exit 1; }

# Build the project and tests
cmake --build . || { echo "Build failed"; exit 1; }

# Run the tests
GTEST_COLOR=1 ctest -V || { echo "Tests failed"; exit 1; }

echo "All tests completed successfully!"