// src/GameGhostPiece.cpp
#include "Game.h"
#include "Color.h"
#include <algorithm>

void Game::drawGhostPiece() {
    if (!currentTetromino_) return;
    
    // Create a manual copy of the current tetromino position and rotation
    int ghostX = currentTetromino_->x();
    int ghostY = currentTetromino_->y();
    int ghostRotation = currentTetromino_->rotation();
    TetrominoType ghostType = currentTetromino_->type();
    
    // Determine how far down the piece can go
    int maxY = ghostY;
    bool canMoveDown = true;
    int safetyCounter = 0;
    const int MAX_ATTEMPTS = GRID_HEIGHT * 2; // Safety limit
    
    // Manually check positions downward
    for (int testY = ghostY + 1; testY < GRID_HEIGHT + 4 && canMoveDown && safetyCounter < MAX_ATTEMPTS; testY++) {
        safetyCounter++;
        
        // Create a temporary tetromino at the test position
        Tetromino tempTetromino(ghostType, ghostX, testY);
        
        // Apply the same rotation as the current tetromino
        for (int i = 0; i < ghostRotation; i++) {
            tempTetromino.rotate(*this);
        }
        
        // Check if this position would be valid
        canMoveDown = true;
        
        // Check all blocks in the shape for collision
        auto shape = tempTetromino.getRotatedShape();
        for (int y = 0; y < 4 && canMoveDown; y++) {
            for (int x = 0; x < 4 && canMoveDown; x++) {
                if (shape[y][x]) {
                    if (!isPositionFree(ghostX + x, testY + y)) {
                        canMoveDown = false;
                    }
                }
            }
        }
        
        if (canMoveDown) {
            maxY = testY;
        }
    }
    
    // If we can drop the piece and it's different from the current position, draw the ghost
    if (maxY > ghostY) {
        SDL_Rect rect;
        rect.w = BLOCK_SIZE - 1;
        rect.h = BLOCK_SIZE - 1;
        
        // Create a temporary tetromino at the landing position
        Tetromino landingTetromino(ghostType, ghostX, maxY);
        
        // Apply the same rotation
        for (int i = 0; i < ghostRotation; i++) {
            landingTetromino.rotate(*this);
        }
        
        // Get the entire shape for drawing
        auto shape = landingTetromino.getRotatedShape();
        
        // Use a bright version of the color for better visibility
        const auto& color = COLORS[static_cast<std::size_t>(ghostType)];
        SDL_SetRenderDrawColor(renderer_.get(), 
                              std::min(color.r + 70, 255),
                              std::min(color.g + 70, 255), 
                              std::min(color.b + 70, 255), 
                              180);
        
        // Draw the ghost piece - draw each block in the shape
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x]) {
                    rect.x = (ghostX + x) * BLOCK_SIZE;
                    rect.y = (maxY + y) * BLOCK_SIZE;
                    
                    if (rect.y >= 0) {  // Only draw if visible
                        SDL_RenderDrawRect(renderer_.get(), &rect);
                    }
                }
            }
        }
    }
}
