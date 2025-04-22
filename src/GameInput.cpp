// src/GameInput.cpp
#include "Game.h"

void Game::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:
            currentTetromino_->moveLeft(*this);
            break;
        case SDLK_RIGHT:
            currentTetromino_->moveRight(*this);
            break;
        case SDLK_DOWN: {
            if (!moveTetromino(0, 1)) {
                // Can't move down further, lock the piece
                lockTetromino();
                clearLines();
                
                if (!createNewTetromino()) {
                    gameOver_ = true;
                }
            }
            break;
        }
        case SDLK_UP:
            currentTetromino_->rotate(*this);
            break;
        case SDLK_SPACE: {
            // Hard drop - move down until collision
            int dropCount = 0;
            while (moveTetromino(0, 1)) {
                score_ += 1;  // Extra points for hard drop
                dropCount++;
                
                // Safety check to prevent infinite loop
                if (dropCount > GRID_HEIGHT) {
                    break;
                }
            }
            
            lockTetromino();
            clearLines();
            
            if (!createNewTetromino()) {
                gameOver_ = true;
            }
            break;
        }
    }
}
#include "Game.h"

bool Game::moveTetromino(int dx, int dy) {
    if (!currentTetromino_) return false;
    
    int newX = currentTetromino_->x() + dx;
    int newY = currentTetromino_->y() + dy;
    int currentRotation = currentTetromino_->rotation();
    TetrominoType currentType = currentTetromino_->type();
    
    // Create a test tetromino at the target position
    Tetromino testTetromino(currentType, newX, newY);
    
    // Apply the same rotation
    for (int i = 0; i < currentRotation; i++) {
        testTetromino.rotateWithoutWallKick(); // Direct rotation without wall kicks
    }
    
    // Check if the new position is valid using the rotated shape
    auto shape = testTetromino.getRotatedShape();
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                if (!isPositionFree(newX + x, newY + y)) {
                    return false;
                }
            }
        }
    }
    
    // Update position directly on the current tetromino
    // This avoids creating a new object which might introduce inconsistencies
    if (dx != 0) {
        currentTetromino_->setPosition(newX, currentTetromino_->y());
    }
    if (dy != 0) {
        currentTetromino_->setPosition(currentTetromino_->x(), newY);
    }
    
    return true;
}
