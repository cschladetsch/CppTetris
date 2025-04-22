#pragma once

#include <array>
#include <cstddef> // For std::size_t

// Tetromino types
enum class TetrominoType {
    I, J, L, O, S, T, Z, COUNT
};

// Tetromino shapes
const std::array<std::array<std::array<bool, 4>, 4>, static_cast<std::size_t>(TetrominoType::COUNT)> SHAPES = {{
    // I
    {{
        {false, false, false, false},
        {true, true, true, true},
        {false, false, false, false},
        {false, false, false, false}
    }},
    // J
    {{
        {true, false, false, false},
        {true, true, true, false},
        {false, false, false, false},
        {false, false, false, false}
    }},
    // L
    {{
        {false, false, true, false},
        {true, true, true, false},
        {false, false, false, false},
        {false, false, false, false}
    }},
    // O
    {{
        {false, true, true, false},
        {false, true, true, false},
        {false, false, false, false},
        {false, false, false, false}
    }},
    // S
    {{
        {false, true, true, false},
        {true, true, false, false},
        {false, false, false, false},
        {false, false, false, false}
    }},
    // T
    {{
        {false, true, false, false},
        {true, true, true, false},
        {false, false, false, false},
        {false, false, false, false}
    }},
    // Z
    {{
        {true, true, false, false},
        {false, true, true, false},
        {false, false, false, false},
        {false, false, false, false}
    }}
}};
