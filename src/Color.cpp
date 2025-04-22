#include "Color.h"

// Define the colors outside of the class
const std::array<Color, static_cast<std::size_t>(TetrominoType::COUNT)> COLORS = {{
    {0, 255, 255},    // I: Cyan
    {0, 0, 255},      // J: Blue
    {255, 165, 0},    // L: Orange
    {255, 255, 0},    // O: Yellow
    {0, 255, 0},      // S: Green
    {128, 0, 128},    // T: Purple
    {255, 0, 0}       // Z: Red
}};
