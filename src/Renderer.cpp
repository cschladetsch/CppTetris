#include "Renderer.h"
#include "Game.h"
#include <format>
#include <algorithm>

Renderer::Renderer(SDL_Renderer* renderer, TTF_Font* font)
    : renderer_(renderer), font_(font) {}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 25, 25, 25, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::drawGrid(const std::vector<std::vector<std::optional<TetrominoType>>>& grid) {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    // Draw outline
    SDL_SetRenderDrawColor(renderer_, 50, 50, 50, 255);
    SDL_Rect border = {0, 0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE};
    SDL_RenderDrawRect(renderer_, &border);
    
    // Draw filled cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            rect.x = x * BLOCK_SIZE;
            rect.y = y * BLOCK_SIZE;
            
            if (grid[y][x].has_value()) {
                const auto& color = COLORS[static_cast<std::size_t>(grid[y][x].value())];
                SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);
                SDL_RenderFillRect(renderer_, &rect);
                
                // Add 3D effect with darker borders
                SDL_SetRenderDrawColor(renderer_, 
                                     color.r/2, color.g/2, color.b/2, 255);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_, &border);
            } else {
                // Draw empty cell
                SDL_SetRenderDrawColor(renderer_, 40, 40, 40, 255);
                SDL_RenderDrawRect(renderer_, &rect);
            }
        }
    }
}

void Renderer::drawTetromino(const Tetromino& tetromino) {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    const auto& color = COLORS[static_cast<std::size_t>(tetromino.type())];
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino.isOccupying(tetromino.x() + x, tetromino.y() + y)) {
                rect.x = (tetromino.x() + x) * BLOCK_SIZE;
                rect.y = (tetromino.y() + y) * BLOCK_SIZE;
                
                if (rect.y >= 0) {  // Only draw if visible
                    SDL_RenderFillRect(renderer_, &rect);
                    
                    // Add 3D effect
                    SDL_SetRenderDrawColor(renderer_, 
                                         color.r/2, color.g/2, color.b/2, 255);
                    SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                    SDL_RenderDrawRect(renderer_, &border);
                    
                    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);
                }
            }
        }
    }
}

void Renderer::drawGhostPiece(const Game& game, const Tetromino& tetromino) {
    // Create a manual copy of the current tetromino position and rotation
    int ghostX = tetromino.x();
    int ghostY = tetromino.y();
    int ghostRotation = tetromino.rotation();
    TetrominoType ghostType = tetromino.type();
    
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
            tempTetromino.rotateWithoutWallKick();
        }
        
        // Check if this position would be valid
        canMoveDown = true;
        
        // Check all blocks in the shape for collision
        auto shape = tempTetromino.getRotatedShape();
        for (int y = 0; y < 4 && canMoveDown; y++) {
            for (int x = 0; x < 4 && canMoveDown; x++) {
                if (shape[y][x]) {
                    if (!game.isPositionFree(ghostX + x, testY + y)) {
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
            landingTetromino.rotateWithoutWallKick();
        }
        
        // Get the entire shape for drawing
        auto shape = landingTetromino.getRotatedShape();
        
        // Use a bright version of the color for better visibility
        const auto& color = COLORS[static_cast<std::size_t>(ghostType)];
        SDL_SetRenderDrawColor(renderer_, 
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
                        SDL_RenderDrawRect(renderer_, &rect);
                    }
                }
            }
        }
    }
}

void Renderer::drawSidebar(const Game& game) {
    int sidebarX = GRID_WIDTH * BLOCK_SIZE + 10;
    int sidebarWidth = WINDOW_WIDTH - sidebarX - 10;
    int y = 20;
    
    // Draw sidebar background
    SDL_SetRenderDrawColor(renderer_, 35, 35, 35, 255);
    SDL_Rect sidebarBg = {sidebarX - 5, 0, sidebarWidth + 5, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_, &sidebarBg);
    
    // Draw section divider line
    SDL_SetRenderDrawColor(renderer_, 60, 60, 60, 255);
    SDL_RenderDrawLine(renderer_, sidebarX - 5, 0, sidebarX - 5, WINDOW_HEIGHT);
    
    // Draw "NEXT" header with highlight
    SDL_SetRenderDrawColor(renderer_, 70, 70, 70, 255);
    SDL_Rect headerBg = {sidebarX, y - 5, sidebarWidth - 10, 30};
    SDL_RenderFillRect(renderer_, &headerBg);
    
    drawText("NEXT", sidebarX + 5, y);
    y += 40;
    
    // Draw next tetromino preview with improved background
    int previewSize = 4 * BLOCK_SIZE;
    SDL_SetRenderDrawColor(renderer_, 45, 45, 45, 255);
    SDL_Rect previewBg = {sidebarX, y, previewSize + 20, previewSize + 20};
    SDL_RenderFillRect(renderer_, &previewBg);
    
    // Draw preview border
    SDL_SetRenderDrawColor(renderer_, 70, 70, 70, 255);
    SDL_Rect previewBorder = {sidebarX, y, previewSize + 20, previewSize + 20};
    SDL_RenderDrawRect(renderer_, &previewBorder);
    
    drawNextTetromino(game.getNextTetrominoType(), sidebarX + 10, y + 10);
    
    y += previewSize + 40;
    
    // Draw score section with highlight
    SDL_SetRenderDrawColor(renderer_, 70, 70, 70, 255);
    SDL_Rect scoreBg = {sidebarX, y - 5, sidebarWidth - 10, 30};
    SDL_RenderFillRect(renderer_, &scoreBg);
    
    drawText("SCORE", sidebarX + 5, y);
    y += 35;
    
    // Draw score value with emphasis
    SDL_SetRenderDrawColor(renderer_, 45, 45, 45, 255);
    SDL_Rect scoreValueBg = {sidebarX, y - 5, sidebarWidth - 10, 35};
    SDL_RenderFillRect(renderer_, &scoreValueBg);
    
    drawText(std::format("{}", game.getScore()), sidebarX + 10, y);
    y += 50;
    
    // Draw level section
    SDL_SetRenderDrawColor(renderer_, 70, 70, 70, 255);
    SDL_Rect levelBg = {sidebarX, y - 5, sidebarWidth - 10, 30};
    SDL_RenderFillRect(renderer_, &levelBg);
    
    drawText("LEVEL", sidebarX + 5, y);
    y += 35;
    
    // Draw level value
    SDL_SetRenderDrawColor(renderer_, 45, 45, 45, 255);
    SDL_Rect levelValueBg = {sidebarX, y - 5, sidebarWidth - 10, 35};
    SDL_RenderFillRect(renderer_, &levelValueBg);
    
    drawText(std::format("{}", game.getLevel()), sidebarX + 10, y);
    y += 50;
    
    // Draw lines section
    SDL_SetRenderDrawColor(renderer_, 70, 70, 70, 255);
    SDL_Rect linesBg = {sidebarX, y - 5, sidebarWidth - 10, 30};
    SDL_RenderFillRect(renderer_, &linesBg);
    
    drawText("LINES", sidebarX + 5, y);
    y += 35;
    
    // Draw lines value
    SDL_SetRenderDrawColor(renderer_, 45, 45, 45, 255);
    SDL_Rect linesValueBg = {sidebarX, y - 5, sidebarWidth - 10, 35};
    SDL_RenderFillRect(renderer_, &linesValueBg);
    
    drawText(std::format("{}", game.getLinesCleared()), sidebarX + 10, y);
}

void Renderer::drawNextTetromino(TetrominoType type, int x, int y) {
    // Create a temporary tetromino for the preview
    Tetromino nextTetromino(type, 0, 0);
    
    // Calculate the center of the preview area
    int previewSize = 4 * BLOCK_SIZE;
    int centerX = x + previewSize / 2;
    int centerY = y + previewSize / 2;
    
    // Draw the tetromino blocks
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    const auto& color = COLORS[static_cast<std::size_t>(type)];
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);
    
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
                
                SDL_RenderFillRect(renderer_, &rect);
                
                // Add 3D effect
                SDL_SetRenderDrawColor(renderer_, 
                                     color.r/2, color.g/2, color.b/2, 255);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_, &border);
                
                SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);
            }
        }
    }
}

void Renderer::drawText(const std::string& text, int x, int y) {
    if (font_) {
        SDL_Color textColor = {220, 220, 220, 255};
        
        // Use Blended rendering for better quality with Unicode characters
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font_, text.c_str(), textColor);
        
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
            
            if (texture) {
                SDL_Rect rect = {x, y, surface->w, surface->h};
                SDL_RenderCopy(renderer_, texture, nullptr, &rect);
                SDL_DestroyTexture(texture);
            }
            
            SDL_FreeSurface(surface);
        }
    } else {
        // Fallback if font loading failed: draw a colored rectangle
        SDL_SetRenderDrawColor(renderer_, 200, 200, 200, 255);
        SDL_Rect rect = {x, y, static_cast<int>(text.length() * 8), 20};
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void Renderer::drawGameOver(int score) {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_, &overlay);
    
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    
    drawText("GAME OVER", centerX - 80, centerY - 30);
    drawText(std::format("Final Score: {}", score), centerX - 80, centerY);
    drawText("Press ENTER to restart", centerX - 110, centerY + 30);
}
