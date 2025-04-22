#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <vector>
#include <optional>
#include <random>
#include <string>
#include <chrono>
#include "Tetromino.h"
#include "Constants.h"

class Game {
public:
    Game();
    ~Game();

    void run();
    bool isPositionFree(int x, int y) const;

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font_;
    
    std::vector<std::vector<std::optional<TetrominoType>>> grid_;
    std::unique_ptr<Tetromino> currentTetromino_;
    TetrominoType nextTetrominoType_;
    bool gameOver_;
    int score_;
    int level_;
    int linesCleared_;
    std::mt19937 rng_;

    std::chrono::milliseconds getFallSpeed() const;
    void handleKeyPress(SDL_Keycode key);
    bool moveTetromino(int dx, int dy);
    void lockTetromino();
    void clearLines();
    bool createNewTetromino();
    
    // Rendering functions
    void render();
    void drawGrid();
    void drawTetromino(const Tetromino& tetromino);
    void drawGhostPiece();
    void drawSidebar();
    void drawNextTetromino(int x, int y);
    void drawText(const std::string& text, int x, int y);
    void drawGameOver();
    
    void resetGame();
};
