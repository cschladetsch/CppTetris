#include <algorithm>
#include <random>
#include "Game.h"

void Game::lockTetromino() {
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            int gridX = currentTetromino_->x() + x;
            int gridY = currentTetromino_->y() + y;
            
            if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                if (currentTetromino_->isOccupying(gridX, gridY)) {
                    grid_[gridY][gridX] = currentTetromino_->type();
                }
            }
        }
    }
}

void Game::clearLines() {
    int linesCleared = 0;
    
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        bool lineComplete = true;
        
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (!grid_[y][x].has_value()) {
                lineComplete = false;
                break;
            }
        }
        
        if (lineComplete) {
            // Clear the line
            for (int yy = y; yy > 0; yy--) {
                grid_[yy] = grid_[yy - 1];
            }
            
            // Clear the top line
            std::fill(grid_[0].begin(), grid_[0].end(), std::optional<TetrominoType>{});
            
            linesCleared++;
            y++; // Recheck this position
        }
    }
    
    if (linesCleared > 0) {
        // Update score based on number of lines cleared
        static const std::array<int, 4> lineScores = {100, 300, 500, 800};
        score_ += lineScores[std::min(linesCleared, TETROMINO_GRID_SIZE) - 1] * level_;
        
        linesCleared_ += linesCleared;
        
        // Update level
        level_ = std::min(INITIAL_LEVEL + linesCleared_ / LINES_PER_LEVEL, MAX_LEVEL);
    }
}

bool Game::createNewTetromino() {
    // Use the next tetromino type that was previously generated
    TetrominoType type = nextTetrominoType_;
    
    // Generate the next tetromino type for preview
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    // Initial position - centered at the top with an offset for the I piece
    int startX = GRID_WIDTH / HALF - HALF;
    // Start higher for I piece to give more space
    int startY = (type == TetrominoType::I) ? MOVE_LEFT : NO_MOVE;
    
    // Create the new tetromino
    currentTetromino_ = std::make_unique<Tetromino>(type, startX, startY);
    
    // Check if the new tetromino can be placed - only check visible cells
    // This gives the player a chance to move/rotate before collision
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            if (currentTetromino_->isOccupying(startX + x, startY + y)) {
                // Only check collisions below y=0 (visible grid area)
                if (startY + y >= 0 && !isPositionFree(startX + x, startY + y)) {
                    return false;  // Game over
                }
            }
        }
    }
    
    return true;
}

void Game::resetGame() {
    // Clear grid
    for (auto& row : grid_) {
        std::fill(row.begin(), row.end(), std::optional<TetrominoType>{});
    }
    
    // Reset game state
    score_ = 0;
    level_ = INITIAL_LEVEL;
    linesCleared_ = 0;
    gameOver_ = false;
    
    // Generate the new next tetromino type
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    // Create new tetromino
    createNewTetromino();
}
