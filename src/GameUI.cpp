#include "Game.h"
#include "Color.h"
#include <format>

void Game::drawSidebar() {
    int sidebarX = GRID_WIDTH * BLOCK_SIZE + 10;
    int y = 20;
    
    // Draw "Next" label
    drawText("Next:", sidebarX, y);
    y += 30;
    
    // Draw next tetromino preview
    drawNextTetromino(sidebarX + BLOCK_SIZE, y);
    
    y += 5 * BLOCK_SIZE + 20;
    
    // Draw score
    drawText(std::format("Score: {}", score_), sidebarX, y);
    y += 40;
    
    // Draw level
    drawText(std::format("Level: {}", level_), sidebarX, y);
    y += 40;
    
    // Draw lines cleared
    drawText(std::format("Lines: {}", linesCleared_), sidebarX, y);
    y += 60;
    
    // Draw controls with Unicode arrows
    drawText("Controls:", sidebarX, y);
    y += 30;
    drawText("\u2190 \u2192 : Move", sidebarX, y);  // Unicode LEFT/RIGHT ARROW
    y += 30;
    drawText("\u2191 : Rotate", sidebarX, y);       // Unicode UP ARROW
    y += 30;
    drawText("\u2193 : Soft Drop", sidebarX, y);    // Unicode DOWN ARROW
    y += 30;
    drawText("Space : Hard Drop", sidebarX, y);
}

void Game::drawNextTetromino(int x, int y) {
    // Create a temporary tetromino for the preview
    Tetromino nextTetromino(nextTetrominoType_, 0, 0);
    
    // Calculate the center of the preview area
    int previewSize = 4 * BLOCK_SIZE;
    int centerX = x + previewSize / 2;
    int centerY = y + previewSize / 2;
    
    // Draw the tetromino blocks
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    const auto& color = COLORS[static_cast<std::size_t>(nextTetrominoType_)];
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
    
    // Draw the tetromino centered in the preview area
    auto shape = nextTetromino.getRotatedShape();
    
    // Calculate offsets to center the tetromino
    int offsetX = 0;
    int offsetY = 0;
    
    // Count rows and columns that have blocks
    int minRow = 4, maxRow = -1, minCol = 4, maxCol = -1;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col]) {
                minRow = std::min(minRow, row);
                maxRow = std::max(maxRow, row);
                minCol = std::min(minCol, col);
                maxCol = std::max(maxCol, col);
            }
        }
    }
    
    // If valid dimensions found, calculate centering offsets
    if (minRow <= maxRow && minCol <= maxCol) {
        int width = maxCol - minCol + 1;
        int height = maxRow - minRow + 1;
        offsetX = centerX - ((width * BLOCK_SIZE) / 2);
        offsetY = centerY - ((height * BLOCK_SIZE) / 2);
        
        // Adjust for the min row/col offset
        offsetX -= minCol * BLOCK_SIZE;
        offsetY -= minRow * BLOCK_SIZE;
    }
    
    // Draw the tetromino blocks
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col]) {
                rect.x = offsetX + (col * BLOCK_SIZE);
                rect.y = offsetY + (row * BLOCK_SIZE);
                
                SDL_RenderFillRect(renderer_.get(), &rect);
                
                // Add 3D effect
                SDL_SetRenderDrawColor(renderer_.get(), 
                                     color.r/2, color.g/2, color.b/2, 255);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_.get(), &border);
                
                SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
            }
        }
    }
    
    // Draw outline around the preview area
    SDL_SetRenderDrawColor(renderer_.get(), 50, 50, 50, 255);
    SDL_Rect previewRect = {x, y, previewSize, previewSize};
    SDL_RenderDrawRect(renderer_.get(), &previewRect);
}
