#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <vector>
#include <optional>
#include <random>
#include <string>
#include <chrono>
#include "TetrominoManager.h"
#include "InputHandler.h"
#include "GameRenderer.h"
#include "SoundManager.h"
#include "GameState.h"
#include "Constants.h"

class Game {
public:
    Game();
    ~Game();

    void run();
    bool isPositionFree(int x, int y) const;
    
    // Accessors
    const std::vector<std::vector<std::optional<TetrominoType>>>& getGrid() const { return grid_; }
    GameState getGameState() const { return gameState_; }
    bool isGameOver() const { return gameState_ == GameState::GameOver; }
    int getScore() const { return score_; }
    int getLevel() const { return level_; }
    int getLinesCleared() const { return linesCleared_; }
    
    // Game state modifiers
    void startGame() { gameState_ = GameState::Playing; }
    void pauseGame() { 
        if (gameState_ == GameState::Playing) 
            gameState_ = GameState::Paused; 
        else if (gameState_ == GameState::Paused)
            gameState_ = GameState::Playing;
    }
    void resetGame();
    void setGameOver() { 
        gameState_ = GameState::GameOver; 
        playGameOverSound(); 
    }
    void increaseScore(int points) { score_ += points; }
    void incrementLinesCleared(int lines); 
    
    // Sound methods
    void playMoveSound() { soundManager_->playSound(SoundEffect::Move); }
    void playRotateSound() { soundManager_->playSound(SoundEffect::Rotate); }
    void playDropSound() { soundManager_->playSound(SoundEffect::Drop); }
    void playLineClearSound() { soundManager_->playSound(SoundEffect::LineClear); }
    void playLevelUpSound() { soundManager_->playSound(SoundEffect::LevelUp); }
    void playGameOverSound() { soundManager_->playSound(SoundEffect::GameOver); }
    void toggleSoundMute() { soundManager_->toggleMute(); }

private:
    // SDL Resources
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font_;
    
    // Game state
    std::vector<std::vector<std::optional<TetrominoType>>> grid_;
    GameState gameState_;
    bool quit_;
    int score_;
    int level_;
    int linesCleared_;
    
    // Component managers
    std::unique_ptr<TetrominoManager> tetrominoManager_;
    std::unique_ptr<InputHandler> inputHandler_;
    std::unique_ptr<GameRenderer> gameRenderer_;
    std::unique_ptr<SoundManager> soundManager_;
    
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
