#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include "Constants.h"
#include "Renderer.h"

class Game;
class TetrominoManager;

class GameRenderer {
public:
    GameRenderer(Game& game, TetrominoManager& tetrominoManager, 
                SDL_Renderer* renderer, TTF_Font* font);
    
    // Main rendering method
    void render();
    
private:
    Game& game_;
    TetrominoManager& tetrominoManager_;
    std::unique_ptr<Renderer> renderer_;
    
    // Helper rendering methods
    void renderGame();
    void renderGameOver();
};
// GameRenderer.h
