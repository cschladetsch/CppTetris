cmake_minimum_required(VERSION 3.14)
project(TetrisGame VERSION 1.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} include)

# Define source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Tetromino.cpp
    src/Color.cpp
)

# Define header files
set(HEADERS
    include/Game.h
    include/Tetromino.h
    include/TetrominoType.h
    include/Color.h
    include/Constants.h
)

# Create executable
add_executable(tetris ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(tetris ${SDL2_LIBRARIES} SDL2_ttf)

# Copy resources to build directory
# Check if resources directory exists before copying
if(EXISTS ${CMAKE_SOURCE_DIR}/resources)
    file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
else()
    # Create resources structure if it doesn't exist
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/fonts)
    message(STATUS "Resources directory was not found. Created empty resources structure.")
endif()

# Install the executable
install(TARGETS tetris DESTINATION bin)

# Optional: Enable warnings
if(MSVC)
    target_compile_options(tetris PRIVATE /W4)
else()
    target_compile_options(tetris PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add a message to help users
message(STATUS "Build with: cmake --build .")
message(STATUS "Run with: ./tetris")
