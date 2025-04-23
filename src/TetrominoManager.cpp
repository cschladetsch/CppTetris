#include "TetrominoManager.h"
#include "Game.h"
#include <algorithm>
#include <array>

TetrominoManager::TetrominoManager(Game& game) 
    : game_(game), currentTetromino_(nullptr) {
    
    initRng();
    generateNextTetrominoType();
}

void TetrominoManager::initRng() {
    std::random_device rd;
    rng_.seed(rd());
}

void TetrominoManager::generateNextTetrominoType() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
}

bool TetrominoManager::moveTetromino(int dx, int dy) {
    if (!currentTetromino_) return false;
    
    int newX = currentTetromino_->x() + dx;
    int newY = currentTetromino_->y() + dy;
    
    // Create a test tetromino to check validity
    Tetromino testTetromino(currentTetromino_->type(), newX, newY);
    
    // Apply the same rotation
    for (int i = 0; i < currentTetromino_->rotation(); i++) {
        testTetromino.rotateWithoutWallKick(); 
    }
    
    // Check if the new position is valid
    if (!isValidPosition(testTetromino)) {
        return false;
    }
    
    // Update position
    if (dx != NO_MOVE) {
        currentTetromino_->setPosition(newX, currentTetromino_->y());
    }
    if (dy != NO_MOVE) {
        currentTetromino_->setPosition(currentTetromino_->x(), newY);
    }
    
    return true;
}

void TetrominoManager::rotateTetromino() {
    if (currentTetromino_) {
        currentTetromino_->rotate(game_);
    }
}

void TetrominoManager::softDrop() {
    if (!moveTetromino(NO_MOVE, MOVE_DOWN)) {
        lockTetromino();
        clearLines();
        
        if (!createNewTetromino()) {
            game_.setGameOver();
        }
    }
}

void TetrominoManager::hardDrop() {
    if (!currentTetromino_) return;
    
    int dropCount = 0;
    while (moveTetromino(NO_MOVE, MOVE_DOWN)) {
        game_.increaseScore(1);  // Extra points for hard drop
        dropCount++;
        
        // Safety check to prevent infinite loop
        if (dropCount > GRID_HEIGHT) {
            break;
        }
    }
    
    lockTetromino();
    clearLines();
    
    if (!createNewTetromino()) {
        game_.setGameOver();
    }
}

void TetrominoManager::lockTetromino() {
    if (!currentTetromino_) return;
    
    auto& grid = const_cast<std::vector<std::vector<std::optional<TetrominoType>>>&>(game_.getGrid());
    
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            int gridX = currentTetromino_->x() + x;
            int gridY = currentTetromino_->y() + y;
            
            if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                if (currentTetromino_->isOccupying(gridX, gridY)) {
                    grid[gridY][gridX] = currentTetromino_->type();
                }
            }
        }
    }
}

void TetrominoManager::clearLines() {
    auto& grid = const_cast<std::vector<std::vector<std::optional<TetrominoType>>>&>(game_.getGrid());
    int linesCleared = 0;
    
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        bool lineComplete = true;
        
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (!grid[y][x].has_value()) {
                lineComplete = false;
                break;
            }
        }
        
        if (lineComplete) {
            // Clear the line
            for (int yy = y; yy > 0; yy--) {
                grid[yy] = grid[yy - 1];
            }
            
            // Clear the top line
            std::fill(grid[0].begin(), grid[0].end(), std::optional<TetrominoType>{});
            
            linesCleared++;
            y++; // Recheck this position
        }
    }
    
    if (linesCleared > 0) {
        calculateScoreAndUpdateLevel(linesCleared);
    }
}

void TetrominoManager::calculateScoreAndUpdateLevel(int linesCleared) {
    // Update score based on number of lines cleared
    static const std::array<int, 4> lineScores = {100, 300, 500, 800};
    int points = lineScores[std::min(linesCleared, TETROMINO_GRID_SIZE) - 1] * game_.getLevel();
    
    game_.increaseScore(points);
    game_.incrementLinesCleared(linesCleared);
}

bool TetrominoManager::createNewTetromino() {
    // Use the next tetromino type that was previously generated
    TetrominoType type = nextTetrominoType_;
    
    // Generate new next tetromino type
    generateNextTetrominoType();
    
    // Initial position - centered at the top with an offset for the I piece
    int startX = GRID_WIDTH / HALF - HALF;
    // Start higher for I piece to give more space
    int startY = (type == TetrominoType::I) ? MOVE_LEFT : NO_MOVE;
    
    // Create the new tetromino
    currentTetromino_ = std::make_unique<Tetromino>(type, startX, startY);
    
    // Check if the new tetromino can be placed
    if (!canPlaceNewTetromino()) {
        return false;  // Game over
    }
    
    return true;
}

bool TetrominoManager::canPlaceNewTetromino() const {
    if (!currentTetromino_) return false;
    
    int startX = currentTetromino_->x();
    int startY = currentTetromino_->y();
    
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            if (currentTetromino_->isOccupying(startX + x, startY + y)) {
                // Only check collisions below y=0 (visible grid area)
                if (startY + y >= 0 && !game_.isPositionFree(startX + x, startY + y)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool TetrominoManager::isValidPosition(const Tetromino& tetromino) const {
    auto shape = tetromino.getRotatedShape();
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            if (shape[y][x]) {
                if (!game_.isPositionFree(tetromino.x() + x, tetromino.y() + y)) {
                    return false;
                }
            }
        }
    }
    return true;
}
// TetrominoManager.cpp
