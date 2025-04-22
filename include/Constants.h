#pragma once

#include <chrono>

using namespace std::chrono_literals;

// Game constants
constexpr int BLOCK_SIZE = 30;
constexpr int GRID_WIDTH = 10;
constexpr int GRID_HEIGHT = 20;
constexpr int WINDOW_WIDTH = BLOCK_SIZE * (GRID_WIDTH + 6);
constexpr int WINDOW_HEIGHT = BLOCK_SIZE * GRID_HEIGHT;
constexpr auto INITIAL_FALL_SPEED = 500ms;
constexpr int LINES_PER_LEVEL = 10;
constexpr int MAX_LEVEL = 15;
