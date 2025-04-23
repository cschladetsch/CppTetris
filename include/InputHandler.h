#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include "GameState.h"

class Game;
class TetrominoManager;

class InputHandler {
public:
    InputHandler(Game& game, TetrominoManager& tetrominoManager);
    
    // Event handling
    bool processEvents();
    
private:
    Game& game_;
    TetrominoManager& tetrominoManager_;
    bool quit_;
    
    // Input processing
    void handleKeyPress(SDL_Keycode key);
    void handleStartScreenInput(SDL_Keycode key);
    void handlePlayingInput(SDL_Keycode key);
    void handlePausedInput(SDL_Keycode key);
    void handleGameOverInput(SDL_Keycode key);
};
