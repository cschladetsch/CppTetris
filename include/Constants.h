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

// Graphics constants
constexpr int ALPHA_OPAQUE = 255;
constexpr int ALPHA_GHOST_PIECE = 180;
constexpr int BLOCK_BORDER_THICKNESS = 1;

// Color constants
constexpr int COLOR_MAX = 255;
constexpr int COLOR_HALF = 128;
constexpr int COLOR_ORANGE = 165;

// UI constants
constexpr int SIDEBAR_PADDING = 10;
constexpr int UI_PADDING_SMALL = 5;
constexpr int UI_PADDING_MEDIUM = 20;
constexpr int UI_PADDING_LARGE = 30;
constexpr int UI_PADDING_XLARGE = 40;
constexpr int UI_PADDING_XXLARGE = 60;

// Grid and background colors
constexpr int GRID_LINE_COLOR = 50;
constexpr int EMPTY_CELL_COLOR = 40;
constexpr int SCREEN_CLEAR_COLOR_R = 25;
constexpr int SCREEN_CLEAR_COLOR_G = 25;
constexpr int SCREEN_CLEAR_COLOR_B = 25;
constexpr int SCREEN_CLEAR_COLOR_A = 255;

// Text colors
constexpr int TEXT_COLOR_R = 200;
constexpr int TEXT_COLOR_G = 200;
constexpr int TEXT_COLOR_B = 200;
constexpr int TEXT_FALLBACK_CHAR_WIDTH = 8;
constexpr int TEXT_FALLBACK_HEIGHT = 20;

// Overlay colors
constexpr int OVERLAY_COLOR_R = 0;
constexpr int OVERLAY_COLOR_G = 0;
constexpr int OVERLAY_COLOR_B = 0;
constexpr int OVERLAY_COLOR_A = 200;

// Game over text positioning
constexpr int GAME_OVER_OFFSET_X = 80;
constexpr int GAME_OVER_OFFSET_Y = 30;

// Tetromino constants
constexpr int TETROMINO_GRID_SIZE = 4;
constexpr int TETROMINO_GRID_MAX_INDEX = TETROMINO_GRID_SIZE - 1;
constexpr int TETROMINO_ROTATION_COUNT = 4;
constexpr int TETROMINO_PREVIEW_SIZE = TETROMINO_GRID_SIZE * BLOCK_SIZE;

// Ghost piece appearance
constexpr int GHOST_PIECE_BRIGHTNESS_BOOST = 70;

// Movement constants
constexpr int MOVE_LEFT = -1;
constexpr int MOVE_RIGHT = 1;
constexpr int MOVE_DOWN = 1;
constexpr int NO_MOVE = 0;

// Division constants
constexpr int HALF = 2;

// Initial level
constexpr int INITIAL_LEVEL = 1;

// Helper value for renderer
constexpr int RENDERER_DEFAULT_INDEX = -1;

// Fall speed calculation
constexpr double LEVEL_SPEED_FACTOR = 0.1;

// Font rendering
constexpr int FONT_SIZE = 24;

// Frame timing
constexpr auto TARGET_FRAME_TIME = 16ms;  // ~60 FPS
constexpr int MICROSECONDS_PER_MILLISECOND = 1000;
