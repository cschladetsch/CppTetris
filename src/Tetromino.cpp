#include "Tetromino.h"
#include "Game.h"
#include <array>
#include <vector>

// SRS (Super Rotation System) kick data
// Each array contains the offsets to test for each rotation

// Kick table for J, L, S, T, Z pieces
const std::vector<std::vector<std::pair<int, int>>> JLSTZ_KICKS = {
    // 0->1
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    // 1->2
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    // 2->3
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    // 3->0
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}
};

// Kick table for I piece
const std::vector<std::vector<std::pair<int, int>>> I_KICKS = {
    // 0->1
    {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}},
    // 1->2
    {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}},
    // 2->3
    {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}},
    // 3->0
    {{0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1}}
};

// No kicks for O piece since it's symmetric
const std::vector<std::vector<std::pair<int, int>>> O_KICKS = {
    {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}
};

Tetromino::Tetromino(TetrominoType type, int x, int y) 
    : type_(type), x_(x), y_(y), rotation_(0) {
}

bool Tetromino::isOccupying(int x, int y) const {
    int localX = x - x_;
    int localY = y - y_;
    
    if (localX < 0 || localX >= 4 || localY < 0 || localY >= 4) {
        return false;
    }

    auto rotatedShape = getRotatedShape();
    return rotatedShape[localY][localX];
}

std::array<std::array<bool, 4>, 4> Tetromino::getRotatedShape() const {
    auto shape = SHAPES[static_cast<std::size_t>(type_)];
    
    // Apply rotation
    std::array<std::array<bool, 4>, 4> rotatedShape = shape;
    
    for (int r = 0; r < rotation_; r++) {
        std::array<std::array<bool, 4>, 4> tempShape{};
        
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                tempShape[x][3 - y] = rotatedShape[y][x];
            }
        }
        
        rotatedShape = tempShape;
    }
    
    return rotatedShape;
}

bool Tetromino::isValidPosition(const Game& game, int newX, int newY, int newRotation) const {
    // Create a temporary tetromino with the new position and rotation
    Tetromino temp(type_, newX, newY);
    temp.rotation_ = newRotation % 4;
    
    // Check if the new position is valid
    auto rotatedShape = temp.getRotatedShape();
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (rotatedShape[y][x]) {
                if (!game.isPositionFree(newX + x, newY + y)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

void Tetromino::rotate(const Game& game) {
    int oldRotation = rotation_;
    int newRotation = (rotation_ + 1) % 4;
    
    // Get the appropriate kick table based on tetromino type
    const std::vector<std::vector<std::pair<int, int>>>* kickTable;
    
    if (type_ == TetrominoType::I) {
        kickTable = &I_KICKS;
    } else if (type_ == TetrominoType::O) {
        kickTable = &O_KICKS;
    } else {
        kickTable = &JLSTZ_KICKS;
    }
    
    // Try each test position from the kick table
    const std::vector<std::pair<int, int>>& kicks = (*kickTable)[oldRotation];
    
    for (const auto& [dx, dy] : kicks) {
        if (isValidPosition(game, x_ + dx, y_ + dy, newRotation)) {
            x_ += dx;
            y_ += dy;
            rotation_ = newRotation;
            return;
        }
    }
    
    // If we couldn't rotate, try a more aggressive approach with additional tests
    // This helps with pieces that get stuck on walls
    const std::array<std::pair<int, int>, 8> extraKicks = {{
        {-2, 0}, {2, 0},   // farther left/right
        {0, -2}, {0, 2},   // farther up/down
        {-2, -1}, {2, -1}, // diagonal kicks
        {-2, 1}, {2, 1}    // diagonal kicks
    }};
    
    for (const auto& [dx, dy] : extraKicks) {
        if (isValidPosition(game, x_ + dx, y_ + dy, newRotation)) {
            x_ += dx;
            y_ += dy;
            rotation_ = newRotation;
            return;
        }
    }
    
    // If we get here, rotation is not possible
}

void Tetromino::moveLeft(const Game& game) {
    if (isValidPosition(game, x_ - 1, y_, rotation_)) {
        x_--;
    }
}

void Tetromino::moveRight(const Game& game) {
    if (isValidPosition(game, x_ + 1, y_, rotation_)) {
        x_++;
    }
}

void Tetromino::moveDown(const Game& game) {
    if (isValidPosition(game, x_, y_ + 1, rotation_)) {
        y_++;
    }
}
