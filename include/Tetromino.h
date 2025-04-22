#pragma once

#include <array>
#include "TetrominoType.h"

// Forward declaration
class Game;

class Tetromino {
public:
    Tetromino(TetrominoType type, int x, int y);
    
    void rotate(const Game& game);
    void moveLeft(const Game& game);
    void moveRight(const Game& game);
    void moveDown(const Game& game);
    
    TetrominoType type() const { return type_; }
    int x() const { return x_; }
    int y() const { return y_; }
    int rotation() const { return rotation_; }

    bool isOccupying(int x, int y) const;
    std::array<std::array<bool, 4>, 4> getRotatedShape() const;
    
    // Direct state manipulation methods
    void setPosition(int x, int y) {
        x_ = x;
        y_ = y;
    }
    
    void rotateWithoutWallKick() {
        rotation_ = (rotation_ + 1) % 4;
    }

private:
    TetrominoType type_;
    int x_;
    int y_;
    int rotation_;  // 0, 1, 2, or 3 (90-degree increments)

    bool isValidPosition(const Game& game, int newX, int newY, int newRotation) const;
};
