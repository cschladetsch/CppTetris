#include "Game.h"
#include "Color.h"
#include <format>

void Game::render() {
    // Clear screen with dark background
    SDL_SetRenderDrawColor(renderer_.get(), 25, 25, 25, 255);
    SDL_RenderClear(renderer_.get());
    
    // Draw grid
    drawGrid();
    
    // Draw current tetromino
    if (currentTetromino_) {
        drawTetromino(*currentTetromino_);
        
        // Draw ghost piece (preview of where piece will land)
        drawGhostPiece();
    }
    
    // Draw score and level
    drawSidebar();
    
    // Draw game over message if needed
    if (gameOver_) {
        drawGameOver();
    }
    
    SDL_RenderPresent(renderer_.get());
}

void Game::drawGrid() {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    // Draw outline
    SDL_SetRenderDrawColor(renderer_.get(), 50, 50, 50, 255);
    SDL_Rect border = {0, 0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE};
    SDL_RenderDrawRect(renderer_.get(), &border);
    
    // Draw filled cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            rect.x = x * BLOCK_SIZE;
            rect.y = y * BLOCK_SIZE;
            
            if (grid_[y][x].has_value()) {
                const auto& color = COLORS[static_cast<std::size_t>(grid_[y][x].value())];
                SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
                SDL_RenderFillRect(renderer_.get(), &rect);
                
                // Add 3D effect with darker borders
                SDL_SetRenderDrawColor(renderer_.get(), 
                                     color.r/2, color.g/2, color.b/2, 255);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_.get(), &border);
            } else {
                // Draw empty cell
                SDL_SetRenderDrawColor(renderer_.get(), 40, 40, 40, 255);
                SDL_RenderDrawRect(renderer_.get(), &rect);
            }
        }
    }
}

void Game::drawTetromino(const Tetromino& tetromino) {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    const auto& color = COLORS[static_cast<std::size_t>(tetromino.type())];
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino.isOccupying(tetromino.x() + x, tetromino.y() + y)) {
                rect.x = (tetromino.x() + x) * BLOCK_SIZE;
                rect.y = (tetromino.y() + y) * BLOCK_SIZE;
                
                if (rect.y >= 0) {  // Only draw if visible
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
    }
}

void Game::drawText(const std::string& text, int x, int y) {
    if (font_) {
        SDL_Color textColor = {200, 200, 200, 255};
        
        // Use Blended rendering for better quality with Unicode characters
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font_.get(), text.c_str(), textColor);
        
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_.get(), surface);
            
            if (texture) {
                SDL_Rect rect = {x, y, surface->w, surface->h};
                SDL_RenderCopy(renderer_.get(), texture, nullptr, &rect);
                SDL_DestroyTexture(texture);
            }
            
            SDL_FreeSurface(surface);
        }
    } else {
        // Fallback if font loading failed: draw a colored rectangle
        SDL_SetRenderDrawColor(renderer_.get(), 200, 200, 200, 255);
        SDL_Rect rect = {x, y, static_cast<int>(text.length() * 8), 20};
        SDL_RenderDrawRect(renderer_.get(), &rect);
    }
}

void Game::drawGameOver() {
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_.get(), &overlay);
    
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    
    drawText("GAME OVER", centerX - 80, centerY - 30);
    drawText(std::format("Final Score: {}", score_), centerX - 80, centerY);
    drawText("Press ENTER to restart", centerX - 110, centerY + 30);
}
