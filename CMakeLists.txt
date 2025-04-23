cmake_minimum_required(VERSION 3.14)
project(TetrisGame VERSION 1.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} include)

# Use file globs to automatically find all source and header files
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)

# Create executable
add_executable(tetris ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(tetris ${SDL2_LIBRARIES} SDL2_ttf SDL2_mixer)

# Check if resources directory exists before copying
if(EXISTS ${CMAKE_SOURCE_DIR}/resources)
    file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
else()
    # Create resources structure if it doesn't exist
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/fonts)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/sounds)
    
    # Copy system fonts to resources directory if available
    if(EXISTS "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")
        file(COPY "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" 
             DESTINATION "${CMAKE_BINARY_DIR}/resources/fonts")
        message(STATUS "Found and copied DejaVuSans.ttf to resources directory")
    elseif(EXISTS "/usr/share/fonts/TTF/DejaVuSans.ttf")
        file(COPY "/usr/share/fonts/TTF/DejaVuSans.ttf" 
             DESTINATION "${CMAKE_BINARY_DIR}/resources/fonts")
        message(STATUS "Found and copied DejaVuSans.ttf to resources directory")
    else()
        message(STATUS "Resources directory was not found. Created empty resources structure.")
        message(STATUS "Please place a Unicode-compatible font in resources/fonts/ directory.")
    endif()
    
    message(STATUS "Created sounds directory. Place sound files in resources/sounds/ directory.")
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
