#pragma once

#include <SDL2/SDL.h>
#include <memory>

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
};
// InputHandler.h
