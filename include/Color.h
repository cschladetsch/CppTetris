#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <cstddef> // For std::size_t
#include "TetrominoType.h"

struct Color {
    Uint8 r, g, b;
};

// Bright, colorful palette
extern const std::array<Color, static_cast<std::size_t>(TetrominoType::COUNT)> COLORS;
