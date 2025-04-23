#include "GameRenderer.h"
#include "Game.h"
#include "TetrominoManager.h"
#include <format>

GameRenderer::GameRenderer(Game& game, TetrominoManager& tetrominoManager, 
                           SDL_Renderer* renderer, TTF_Font* font)
    : game_(game), tetrominoManager_(tetrominoManager) {
    
    renderer_ = std::make_unique<Renderer>(renderer, font);
}

void GameRenderer::render() {
    renderer_->clear();
    
    switch (game_.getGameState()) {
        case GameState::StartScreen:
            renderStartScreen();
            break;
            
        case GameState::Playing:
            renderGame();
            break;
            
        case GameState::Paused:
            renderGame();
            renderPauseScreen();
            break;
            
        case GameState::GameOver:
            renderGame();
            renderGameOver();
            break;
    }
    
    renderer_->present();
}

void GameRenderer::renderStartScreen() {
    // Set background color (slightly different from game background)
    SDL_SetRenderDrawColor(renderer_->getRenderer(), 
                          SCREEN_CLEAR_COLOR_R + 10, 
                          SCREEN_CLEAR_COLOR_G + 10, 
                          SCREEN_CLEAR_COLOR_B + 10, 
                          SCREEN_CLEAR_COLOR_A);
    SDL_Rect fullScreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_->getRenderer(), &fullScreen);
    
    int titleY = 100;
    renderer_->drawLargeText("C++23 TETRIS", WINDOW_WIDTH / 2 - 150, titleY);
    
    int decorationY = titleY + 80;
    int spacing = 60;
    
    for (int i = 0; i < static_cast<int>(TetrominoType::COUNT); i++) {
        TetrominoType type = static_cast<TetrominoType>(i);
        int x = (WINDOW_WIDTH / 2) - (static_cast<int>(TetrominoType::COUNT) * spacing) / 2 + i * spacing;
        renderer_->drawNextTetromino(type, x, decorationY);
    }
    
    int instructionsY = decorationY + 120;
    renderer_->drawText("Arrow Keys: Move and Rotate", WINDOW_WIDTH / 2 - 120, instructionsY);
    renderer_->drawText("Space: Hard Drop", WINDOW_WIDTH / 2 - 120, instructionsY + 30);
    renderer_->drawText("P: Pause Game", WINDOW_WIDTH / 2 - 120, instructionsY + 60);
    renderer_->drawText("M: Toggle Sound", WINDOW_WIDTH / 2 - 120, instructionsY + 90);
    renderer_->drawText("ESC: Quit Game", WINDOW_WIDTH / 2 - 120, instructionsY + 120);
    
    int startY = instructionsY + 180;
    renderer_->drawText("Press ENTER or SPACE to Start", WINDOW_WIDTH / 2 - 140, startY);
    
    Uint32 ticks = SDL_GetTicks();
    if ((ticks / 500) % 2 == 0) {  // Blink every 500ms
        renderer_->drawText("> PRESS START <", WINDOW_WIDTH / 2 - 80, startY + 40);
    }
    
    renderer_->drawText("Version 1.0.0", 20, WINDOW_HEIGHT - 40);
}

void GameRenderer::renderGame() {
    renderer_->drawGrid(game_.getGrid());
    
    const Tetromino* currentTetromino = tetrominoManager_.getCurrentTetromino();
    
    if (currentTetromino) {
        renderer_->drawTetromino(*currentTetromino);
        renderer_->drawGhostPiece(game_, *currentTetromino);
    }
    
    renderer_->drawSidebar(game_, tetrominoManager_.getNextTetrominoType());
}

void GameRenderer::renderPauseScreen() {
    SDL_SetRenderDrawColor(renderer_->getRenderer(), 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_->getRenderer(), &overlay);
    
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    
    renderer_->drawLargeText("PAUSED", centerX - 80, centerY - 60);
    renderer_->drawText("Press P or ENTER to continue", centerX - 120, centerY);
    renderer_->drawText("Press ESC to quit", centerX - 80, centerY + 40);
}

void GameRenderer::renderGameOver() {
    // Semi-transparent overlay
    SDL_SetRenderDrawColor(renderer_->getRenderer(), 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_->getRenderer(), &overlay);
    
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    
    renderer_->drawLargeText("GAME OVER", centerX - 100, centerY - 60);
    renderer_->drawText(std::format("Final Score: {}", game_.getScore()), 
                      centerX - 80, centerY);
    renderer_->drawText("Press ENTER to restart", centerX - 100, centerY + 40);
    renderer_->drawText("Press ESC to quit", centerX - 80, centerY + 70);
}
