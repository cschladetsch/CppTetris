#include "Tetromino.h"
#include "Game.h"

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
    int newRotation = (rotation_ + 1) % 4;
    
    // Try basic rotation
    if (isValidPosition(game, x_, y_, newRotation)) {
        rotation_ = newRotation;
        return;
    }
    
    // Try wall kicks
    const std::array<std::pair<int, int>, 4> kicks = {{
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    }};
    
    for (const auto& [dx, dy] : kicks) {
        if (isValidPosition(game, x_ + dx, y_ + dy, newRotation)) {
            x_ += dx;
            y_ += dy;
            rotation_ = newRotation;
            return;
        }
    }
    
    // Can't rotate
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
