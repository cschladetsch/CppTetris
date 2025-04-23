#include "Renderer.h"
#include "Game.h"
#include "Color.h"
#include <format>
#include <algorithm>

Renderer::Renderer(SDL_Renderer* renderer, TTF_Font* font)
    : renderer_(renderer), font_(font) {}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer_, SCREEN_CLEAR_COLOR_R, SCREEN_CLEAR_COLOR_G, 
                          SCREEN_CLEAR_COLOR_B, SCREEN_CLEAR_COLOR_A);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::drawGrid(const std::vector<std::vector<std::optional<TetrominoType>>>& grid) {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
    rect.h = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
    
    SDL_SetRenderDrawColor(renderer_, GRID_LINE_COLOR, GRID_LINE_COLOR, GRID_LINE_COLOR, ALPHA_OPAQUE);
    SDL_Rect border = {0, 0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE};
    SDL_RenderDrawRect(renderer_, &border);
    
    // Draw filled cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            rect.x = x * BLOCK_SIZE;
            rect.y = y * BLOCK_SIZE;
            
            if (grid[y][x].has_value()) {
                const auto& color = COLORS[static_cast<std::size_t>(grid[y][x].value())];
                SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer_, &rect);
                
                // Add 3D effect with darker borders
                SDL_SetRenderDrawColor(renderer_, 
                                     color.r/HALF, color.g/HALF, color.b/HALF, ALPHA_OPAQUE);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_, &border);
            } else {
                // Draw empty cell
                SDL_SetRenderDrawColor(renderer_, EMPTY_CELL_COLOR, EMPTY_CELL_COLOR, 
                                      EMPTY_CELL_COLOR, ALPHA_OPAQUE);
                SDL_RenderDrawRect(renderer_, &rect);
            }
        }
    }
}

void Renderer::drawTetromino(const Tetromino& tetromino) {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
    rect.h = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
    
    const auto& color = COLORS[static_cast<std::size_t>(tetromino.type())];
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, ALPHA_OPAQUE);
    
    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            if (tetromino.isOccupying(tetromino.x() + x, tetromino.y() + y)) {
                rect.x = (tetromino.x() + x) * BLOCK_SIZE;
                rect.y = (tetromino.y() + y) * BLOCK_SIZE;
                
                if (rect.y >= 0) {  // Only draw if visible
                    SDL_RenderFillRect(renderer_, &rect);
                    
                    // Add 3D effect
                    SDL_SetRenderDrawColor(renderer_, 
                                         color.r/HALF, color.g/HALF, color.b/HALF, ALPHA_OPAQUE);
                    SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                    SDL_RenderDrawRect(renderer_, &border);
                    
                    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, ALPHA_OPAQUE);
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
        for (int y = 0; y < TETROMINO_GRID_SIZE && canMoveDown; y++) {
            for (int x = 0; x < TETROMINO_GRID_SIZE && canMoveDown; x++) {
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
        rect.w = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
        rect.h = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
        
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
                              std::min(color.r + GHOST_PIECE_BRIGHTNESS_BOOST, COLOR_MAX),
                              std::min(color.g + GHOST_PIECE_BRIGHTNESS_BOOST, COLOR_MAX), 
                              std::min(color.b + GHOST_PIECE_BRIGHTNESS_BOOST, COLOR_MAX), 
                              ALPHA_GHOST_PIECE);
        
        // Draw the ghost piece - draw each block in the shape
        for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
            for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
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

void Renderer::drawSidebar(const Game& game, TetrominoType nextTetrominoType) {
    int sidebarX = GRID_WIDTH * BLOCK_SIZE + SIDEBAR_PADDING;
    int y = UI_PADDING_MEDIUM;
    
    drawText("Next:", sidebarX, y);
    y += UI_PADDING_LARGE;
    
    drawNextTetromino(nextTetrominoType, sidebarX + BLOCK_SIZE, y);
    
    y += TETROMINO_GRID_SIZE * BLOCK_SIZE + UI_PADDING_MEDIUM;
    
    drawText(std::format("Score: {}", game.getScore()), sidebarX, y);
    y += UI_PADDING_XLARGE;
    
    drawText(std::format("Level: {}", game.getLevel()), sidebarX, y);
    y += UI_PADDING_XLARGE;
    
    drawText(std::format("Lines: {}", game.getLinesCleared()), sidebarX, y);
    y += UI_PADDING_XXLARGE;
    
    drawText("Controls:", sidebarX, y);
    y += UI_PADDING_LARGE;
    drawText("\u2190 \u2192 : Move", sidebarX, y);  // Unicode LEFT/RIGHT ARROW
    y += UI_PADDING_LARGE;
    drawText("\u2191 : Rotate", sidebarX, y);       // Unicode UP ARROW
    y += UI_PADDING_LARGE;
    drawText("\u2193 : Soft Drop", sidebarX, y);    // Unicode DOWN ARROW
    y += UI_PADDING_LARGE;
    drawText("Space : Hard Drop", sidebarX, y);
}

void Renderer::drawNextTetromino(TetrominoType type, int x, int y) {
    Tetromino nextTetromino(type, 0, 0);
    
    int previewSize = TETROMINO_GRID_SIZE * BLOCK_SIZE;
    int centerX = x + previewSize / HALF;
    int centerY = y + previewSize / HALF;
    
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
    rect.h = BLOCK_SIZE - BLOCK_BORDER_THICKNESS;
    
    const auto& color = COLORS[static_cast<std::size_t>(type)];
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, ALPHA_OPAQUE);
    
    auto shape = nextTetromino.getRotatedShape();
    
    int offsetX = 0;
    int offsetY = 0;
    
    int minRow = TETROMINO_GRID_SIZE, maxRow = -1, minCol = TETROMINO_GRID_SIZE, maxCol = -1;
    for (int row = 0; row < TETROMINO_GRID_SIZE; row++) {
        for (int col = 0; col < TETROMINO_GRID_SIZE; col++) {
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
        offsetX = centerX - ((width * BLOCK_SIZE) / HALF);
        offsetY = centerY - ((height * BLOCK_SIZE) / HALF);
        
        // Adjust for the min row/col offset
        offsetX -= minCol * BLOCK_SIZE;
        offsetY -= minRow * BLOCK_SIZE;
    }
    
    // Draw the tetromino blocks
    for (int row = 0; row < TETROMINO_GRID_SIZE; row++) {
        for (int col = 0; col < TETROMINO_GRID_SIZE; col++) {
            if (shape[row][col]) {
                rect.x = offsetX + (col * BLOCK_SIZE);
                rect.y = offsetY + (row * BLOCK_SIZE);
                
                SDL_RenderFillRect(renderer_, &rect);
                
                // Add 3D effect
                SDL_SetRenderDrawColor(renderer_, 
                                     color.r/HALF, color.g/HALF, color.b/HALF, ALPHA_OPAQUE);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_, &border);
                
                SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, ALPHA_OPAQUE);
            }
        }
    }
    
    // Draw outline around the preview area
    SDL_SetRenderDrawColor(renderer_, GRID_LINE_COLOR, GRID_LINE_COLOR, GRID_LINE_COLOR, ALPHA_OPAQUE);
    SDL_Rect previewRect = {x, y, previewSize, previewSize};
    SDL_RenderDrawRect(renderer_, &previewRect);
}

void Renderer::drawText(const std::string& text, int x, int y) {
    if (font_) {
        SDL_Color textColor = {TEXT_COLOR_R, TEXT_COLOR_G, TEXT_COLOR_B, ALPHA_OPAQUE};
        
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
        SDL_SetRenderDrawColor(renderer_, TEXT_COLOR_R, TEXT_COLOR_G, TEXT_COLOR_B, ALPHA_OPAQUE);
        SDL_Rect rect = {x, y, static_cast<int>(text.length() * TEXT_FALLBACK_CHAR_WIDTH), TEXT_FALLBACK_HEIGHT};
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void Renderer::drawLargeText(const std::string& text, int x, int y) {
    if (font_) {
        int originalSize = TTF_FontHeight(font_);
        
        TTF_Font* largeFont = TTF_OpenFont(TTF_FontFaceFamilyName(font_), originalSize * 2);
        
        if (largeFont) {
            SDL_Color textColor = {255, 255, 255, 255}; // Bright white for large text
            
            SDL_Surface* surface = TTF_RenderUTF8_Blended(largeFont, text.c_str(), textColor);
            
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
                
                if (texture) {
                    SDL_Rect rect = {x, y, surface->w, surface->h};
                    SDL_RenderCopy(renderer_, texture, nullptr, &rect);
                    SDL_DestroyTexture(texture);
                }
                
                SDL_FreeSurface(surface);
            }
            
            TTF_CloseFont(largeFont);
        } else {
            // Fall back to regular size if large font creation fails
            SDL_Color textColor = {255, 255, 255, 255}; // White for emphasis
            
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
        }
    } else {
        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_Rect rect = {x, y, static_cast<int>(text.length() * 16), 40}; // Larger size
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void Renderer::drawGameOver(int score) {
    SDL_SetRenderDrawColor(renderer_, OVERLAY_COLOR_R, OVERLAY_COLOR_G, 
                          OVERLAY_COLOR_B, OVERLAY_COLOR_A);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_, &overlay);
    
    int centerX = WINDOW_WIDTH / HALF;
    int centerY = WINDOW_HEIGHT / HALF;
    
    drawText("GAME OVER", centerX - GAME_OVER_OFFSET_X, centerY - GAME_OVER_OFFSET_Y);
    drawText(std::format("Final Score: {}", score), centerX - GAME_OVER_OFFSET_X, centerY);
    drawText("Press ENTER to restart", centerX - GAME_OVER_OFFSET_X - UI_PADDING_LARGE, centerY + GAME_OVER_OFFSET_Y);
}
