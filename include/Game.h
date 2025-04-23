#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <vector>
#include <optional>
#include <random>
#include <chrono>
#include "TetrominoManager.h"
#include "InputHandler.h"
#include "GameRenderer.h"
#include "Constants.h"

class Game {
public:
    Game();
    ~Game();

    void run();
    bool isPositionFree(int x, int y) const;
    
    // Accessors
    const std::vector<std::vector<std::optional<TetrominoType>>>& getGrid() const { return grid_; }
    bool isGameOver() const { return gameOver_; }
    int getScore() const { return score_; }
    int getLevel() const { return level_; }
    int getLinesCleared() const { return linesCleared_; }
    
    // Game state modifiers
    void resetGame();
    void setGameOver() { gameOver_ = true; }
    void increaseScore(int points) { score_ += points; }
    void incrementLinesCleared(int lines) { 
        linesCleared_ += lines; 
        level_ = std::min(INITIAL_LEVEL + linesCleared_ / LINES_PER_LEVEL, MAX_LEVEL);
    }

private:
    // SDL Resources
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font_;
    
    // Game state
    std::vector<std::vector<std::optional<TetrominoType>>> grid_;
    bool gameOver_;
    bool quit_;
    int score_;
    int level_;
    int linesCleared_;
    
    // Component managers
    std::unique_ptr<TetrominoManager> tetrominoManager_;
    std::unique_ptr<InputHandler> inputHandler_;
    std::unique_ptr<GameRenderer> gameRenderer_;
    
    // Initialization methods
    void initSDL();
    void loadFont();
    
    // Game loop methods
    void updateGameState(const std::chrono::steady_clock::time_point& currentTime, 
                         std::chrono::steady_clock::time_point& lastFallTime);
    void capFrameRate(std::chrono::steady_clock::time_point& lastFrameTime);
    
    // Game mechanics
    std::chrono::milliseconds getFallSpeed() const;
};
// Game.h
