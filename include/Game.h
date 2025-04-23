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
#include "Renderer.h"

class Game {
public:
    Game();
    ~Game();

    void run();
    bool isPositionFree(int x, int y) const;
    
    // Getters for renderer to access game state
    const std::vector<std::vector<std::optional<TetrominoType>>>& getGrid() const { return grid_; }
    const Tetromino* getCurrentTetromino() const { return currentTetromino_.get(); }
    TetrominoType getNextTetrominoType() const { return nextTetrominoType_; }
    bool isGameOver() const { return gameOver_; }
    int getScore() const { return score_; }
    int getLevel() const { return level_; }
    int getLinesCleared() const { return linesCleared_; }

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font_;
    std::unique_ptr<Renderer> renderer_instance_; // Renderer object
    
    std::vector<std::vector<std::optional<TetrominoType>>> grid_;
    std::unique_ptr<Tetromino> currentTetromino_;
    TetrominoType nextTetrominoType_;
    bool gameOver_;
    int score_;
    int level_;
    int linesCleared_;
    std::mt19937 rng_;

    // Game logic functions
    void render();
    std::chrono::milliseconds getFallSpeed() const;
    void handleKeyPress(SDL_Keycode key);
    bool moveTetromino(int dx, int dy);
    void lockTetromino();
    void clearLines();
    bool createNewTetromino();
    void resetGame();
};
