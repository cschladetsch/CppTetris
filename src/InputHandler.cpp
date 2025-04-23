#include "InputHandler.h"
#include "Game.h"
#include "TetrominoManager.h"

InputHandler::InputHandler(Game& game, TetrominoManager& tetrominoManager)
    : game_(game), tetrominoManager_(tetrominoManager), quit_(false) {
}

bool InputHandler::processEvents() {
    SDL_Event e;
    
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            return true; // Signal to quit
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                return true; // Quit on Escape key
            }
            handleKeyPress(e.key.keysym.sym);
        }
    }
    
    return false; // Don't quit
}

void InputHandler::handleKeyPress(SDL_Keycode key) {
    // Handle game over state
    if (game_.isGameOver()) {
        if (key == SDLK_RETURN) {
            game_.resetGame();
        }
        return;
    }
    
    // Handle active game state
    switch (key) {
        case SDLK_LEFT:
            tetrominoManager_.moveTetromino(MOVE_LEFT, NO_MOVE);
            break;
            
        case SDLK_RIGHT:
            tetrominoManager_.moveTetromino(MOVE_RIGHT, NO_MOVE);
            break;
            
        case SDLK_DOWN:
            tetrominoManager_.softDrop();
            break;
            
        case SDLK_UP:
            tetrominoManager_.rotateTetromino();
            break;
            
        case SDLK_SPACE:
            tetrominoManager_.hardDrop();
            break;
    }
}
