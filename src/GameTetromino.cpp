#include "Game.h"
#include <algorithm>

void Game::lockTetromino() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
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
        score_ += lineScores[std::min(linesCleared, 4) - 1] * level_;
        
        linesCleared_ += linesCleared;
        
        // Update level
        level_ = std::min(1 + linesCleared_ / LINES_PER_LEVEL, MAX_LEVEL);
    }
}

bool Game::createNewTetromino() {
    // Use the next tetromino type that was previously generated
    TetrominoType type = nextTetrominoType_;
    
    // Generate the next tetromino type for preview
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    currentTetromino_ = std::make_unique<Tetromino>(type, GRID_WIDTH / 2 - 2, 0);
    
    // Check if new tetromino can be placed
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int gridX = currentTetromino_->x() + x;
            int gridY = currentTetromino_->y() + y;
            
            if (currentTetromino_->isOccupying(gridX, gridY)) {
                if (!isPositionFree(gridX, gridY)) {
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
    level_ = 1;
    linesCleared_ = 0;
    gameOver_ = false;
    
    // Generate the new next tetromino type
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    // Create new tetromino
    createNewTetromino();
}
