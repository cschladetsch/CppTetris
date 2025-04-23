#include "Color.h"
#include "Constants.h"

// Define the colors outside of the class
const std::array<Color, static_cast<std::size_t>(TetrominoType::COUNT)> COLORS = {{
    {0, COLOR_MAX, COLOR_MAX},    // I: Cyan
    {0, 0, COLOR_MAX},      // J: Blue
    {COLOR_MAX, COLOR_ORANGE, 0},    // L: Orange
    {COLOR_MAX, COLOR_MAX, 0},    // O: Yellow
    {0, COLOR_MAX, 0},      // S: Green
    {COLOR_HALF, 0, COLOR_HALF},    // T: Purple
    {COLOR_MAX, 0, 0}       // Z: Red
}};
