#include "Tetromino.h"
#include "Game.h"
#include <array>
#include <vector>

// SRS (Super Rotation System) kick data
// Each array contains the offsets to test for each rotation

// Kick table for J, L, S, T, Z pieces
const std::vector<std::vector<std::pair<int, int>>> JLSTZ_KICKS = {
    // 0->1
    {{0, 0}, {MOVE_LEFT, 0}, {MOVE_LEFT, MOVE_LEFT}, {0, HALF}, {MOVE_LEFT, HALF}},
    // 1->2
    {{0, 0}, {MOVE_RIGHT, 0}, {MOVE_RIGHT, MOVE_RIGHT}, {0, -HALF}, {MOVE_RIGHT, -HALF}},
    // 2->3
    {{0, 0}, {MOVE_RIGHT, 0}, {MOVE_RIGHT, MOVE_LEFT}, {0, HALF}, {MOVE_RIGHT, HALF}},
    // 3->0
    {{0, 0}, {MOVE_LEFT, 0}, {MOVE_LEFT, MOVE_RIGHT}, {0, -HALF}, {MOVE_LEFT, -HALF}}
};

// Kick table for I piece
const std::vector<std::vector<std::pair<int, int>>> I_KICKS = {
    // 0->1
    {{0, 0}, {-HALF, 0}, {MOVE_RIGHT, 0}, {-HALF, MOVE_RIGHT}, {MOVE_RIGHT, -HALF}},
    // 1->2
    {{0, 0}, {MOVE_LEFT, 0}, {HALF, 0}, {MOVE_LEFT, -HALF}, {HALF, MOVE_RIGHT}},
    // 2->3
    {{0, 0}, {HALF, 0}, {MOVE_LEFT, 0}, {HALF, MOVE_LEFT}, {MOVE_LEFT, HALF}},
    // 3->0
    {{0, 0}, {MOVE_RIGHT, 0}, {-HALF, 0}, {MOVE_RIGHT, HALF}, {-HALF, MOVE_LEFT}}
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
    
    if (localX < 0 || localX >= TETROMINO_GRID_SIZE || localY < 0 || localY >= TETROMINO_GRID_SIZE) {
        return false;
    }

    auto rotatedShape = getRotatedShape();
    return rotatedShape[localY][localX];
}

std::array<std::array<bool, TETROMINO_GRID_SIZE>, TETROMINO_GRID_SIZE> Tetromino::getRotatedShape() const {
    auto shape = SHAPES[static_cast<std::size_t>(type_)];
    
    // Apply rotation
    std::array<std::array<bool, TETROMINO_GRID_SIZE>, TETROMINO_GRID_SIZE> rotatedShape = shape;
    
    for (int r = 0; r < rotation_; r++) {
        std::array<std::array<bool, TETROMINO_GRID_SIZE>, TETROMINO_GRID_SIZE> tempShape{};
        
        for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
            for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
                tempShape[x][TETROMINO_GRID_MAX_INDEX - y] = rotatedShape[y][x];
            }
        }
        
        rotatedShape = tempShape;
    }
    
    return rotatedShape;
}

bool Tetromino::isValidPosition(const Game& game, int newX, int newY, int newRotation) const {
    // Create a temporary tetromino with the new position and rotation
    Tetromino temp(type_, newX, newY);
    temp.rotation_ = newRotation % TETROMINO_ROTATION_COUNT;
    
    // Check if the new position is valid
    auto rotatedShape = temp.getRotatedShape();
    
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
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
    int newRotation = (rotation_ + MOVE_RIGHT) % TETROMINO_ROTATION_COUNT;
    
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
        {-HALF, 0}, {HALF, 0},   // farther left/right
        {0, -HALF}, {0, HALF},   // farther up/down
        {-HALF, MOVE_LEFT}, {HALF, MOVE_LEFT}, // diagonal kicks
        {-HALF, MOVE_RIGHT}, {HALF, MOVE_RIGHT}    // diagonal kicks
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
    if (isValidPosition(game, x_ + MOVE_LEFT, y_, rotation_)) {
        x_ += MOVE_LEFT;
    }
}

void Tetromino::moveRight(const Game& game) {
    if (isValidPosition(game, x_ + MOVE_RIGHT, y_, rotation_)) {
        x_ += MOVE_RIGHT;
    }
}

void Tetromino::moveDown(const Game& game) {
    if (isValidPosition(game, x_, y_ + MOVE_DOWN, rotation_)) {
        y_ += MOVE_DOWN;
    }
}
