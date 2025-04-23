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
            return true;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                // Handle ESC differently based on game state
                GameState state = game_.getGameState();
                
                if (state == GameState::StartScreen || state == GameState::GameOver) {
                    // When in start screen or game over, ESC quits the game
                    return true; 
                } else if (state == GameState::Playing) {
                    // When playing, ESC pauses the game
                    game_.pauseGame();
                } else if (state == GameState::Paused) {
                    // When already paused, ESC quits the game
                    return true;
                }
            } else if (e.key.keysym.sym == SDLK_m) {
                game_.toggleSoundMute(); // Toggle mute with M key
            } else {
                handleKeyPress(e.key.keysym.sym);
            }
        }
    }
    
    return false; // Don't quit
}

void InputHandler::handleKeyPress(SDL_Keycode key) {
    switch (game_.getGameState()) {
        case GameState::StartScreen:
            handleStartScreenInput(key);
            break;
            
        case GameState::Playing:
            handlePlayingInput(key);
            break;
            
        case GameState::Paused:
            handlePausedInput(key);
            break;
            
        case GameState::GameOver:
            handleGameOverInput(key);
            break;
    }
}

void InputHandler::handleStartScreenInput(SDL_Keycode key) {
    if (key == SDLK_RETURN || key == SDLK_SPACE) {
        game_.startGame();
    }
}

void InputHandler::handlePlayingInput(SDL_Keycode key) {
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
            
        case SDLK_p:
            game_.pauseGame();
            break;
    }
}

void InputHandler::handlePausedInput(SDL_Keycode key) {
    if (key == SDLK_p || key == SDLK_RETURN) {
        game_.pauseGame(); // Toggle pause state
    }
}

void InputHandler::handleGameOverInput(SDL_Keycode key) {
    if (key == SDLK_RETURN) {
        game_.resetGame();
        game_.startGame();
    }
}
