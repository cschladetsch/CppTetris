cmake_minimum_required(VERSION 3.14)

# Google Test requires at least C++14
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add test executables
add_executable(
  tetromino_test
  tetromino_test.cpp
)
target_link_libraries(
  tetromino_test
  GTest::gtest_main
  tetris_lib
)

add_executable(
  tetromino_manager_test
  tetromino_manager_test.cpp
)
target_link_libraries(
  tetromino_manager_test
  GTest::gtest_main
  tetris_lib
)

add_executable(
  game_test
  game_test.cpp
)
target_link_libraries(
  game_test
  GTest::gtest_main
  tetris_lib
)

add_executable(
  grid_collision_test
  grid_collision_test.cpp
)
target_link_libraries(
  grid_collision_test
  GTest::gtest_main
  tetris_lib
)

# Register tests
include(GoogleTest)
gtest_discover_tests(tetromino_test)
gtest_discover_tests(tetromino_manager_test)
gtest_discover_tests(game_test)
gtest_discover_tests(grid_collision_test)