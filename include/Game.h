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
    Game(bool test_mode = false);
    virtual ~Game();

    virtual void run();
    virtual bool isPositionFree(int x, int y) const;
    
    // Accessors
    virtual const std::vector<std::vector<std::optional<TetrominoType>>>& getGrid() const { return grid_; }
    virtual GameState getGameState() const { return gameState_; }
    virtual bool isGameOver() const { return gameState_ == GameState::GameOver; }
    virtual int getScore() const { return score_; }
    virtual int getLevel() const { return level_; }
    virtual int getLinesCleared() const { return linesCleared_; }
    
    // Game state modifiers
    virtual void startGame() { gameState_ = GameState::Playing; }
    virtual void pauseGame() { 
        if (gameState_ == GameState::Playing) 
            gameState_ = GameState::Paused; 
        else if (gameState_ == GameState::Paused)
            gameState_ = GameState::Playing;
    }
    virtual void resetGame();
    virtual void setGameOver() { 
        gameState_ = GameState::GameOver; 
        playGameOverSound(); 
    }
    virtual void increaseScore(int points) { score_ += points; }
    virtual void incrementLinesCleared(int lines); 
    
    // Sound methods
    virtual void playMoveSound() { soundManager_->playSound(SoundEffect::Move); }
    virtual void playRotateSound() { soundManager_->playSound(SoundEffect::Rotate); }
    virtual void playDropSound() { soundManager_->playSound(SoundEffect::Drop); }
    virtual void playLineClearSound() { soundManager_->playSound(SoundEffect::LineClear); }
    virtual void playLevelUpSound() { soundManager_->playSound(SoundEffect::LevelUp); }
    virtual void playGameOverSound() { soundManager_->playSound(SoundEffect::GameOver); }
    virtual void toggleSoundMute() { soundManager_->toggleMute(); }

protected:
    // Component managers
    std::unique_ptr<TetrominoManager> tetrominoManager_;
    std::unique_ptr<InputHandler> inputHandler_;
    std::unique_ptr<GameRenderer> gameRenderer_;
    std::unique_ptr<SoundManager> soundManager_;
    
    // Game state
    std::vector<std::vector<std::optional<TetrominoType>>> grid_;
    GameState gameState_;
    bool quit_;
    int score_;
    int level_;
    int linesCleared_;

private:
    // SDL Resources
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font_;
    
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
