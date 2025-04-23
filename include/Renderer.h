#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <optional>
#include "Tetromino.h"
#include "Constants.h"

class Game;

class Renderer {
public:
    Renderer(SDL_Renderer* renderer, TTF_Font* font);
    
    // Core rendering functions
    void clear();
    void present();
    
    // Game element rendering functions
    void drawGrid(const std::vector<std::vector<std::optional<TetrominoType>>>& grid);
    void drawTetromino(const Tetromino& tetromino);
    void drawGhostPiece(const Game& game, const Tetromino& tetromino);
    void drawSidebar(const Game& game, TetrominoType nextTetrominoType);
    void drawNextTetromino(TetrominoType type, int x, int y);
    void drawText(const std::string& text, int x, int y);
    void drawLargeText(const std::string& text, int x, int y);
    void drawGameOver(int score);
    
    // Accessor for the renderer (used for direct drawing in some cases)
    SDL_Renderer* getRenderer() const { return renderer_; }
    
private:
    SDL_Renderer* renderer_; // Non-owning pointer
    TTF_Font* font_;         // Non-owning pointer
};
