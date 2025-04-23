#include "GameRenderer.h"
#include "Game.h"
#include "TetrominoManager.h"

GameRenderer::GameRenderer(Game& game, TetrominoManager& tetrominoManager, 
                           SDL_Renderer* renderer, TTF_Font* font)
    : game_(game), tetrominoManager_(tetrominoManager) {
    
    // Create the renderer instance
    renderer_ = std::make_unique<Renderer>(renderer, font);
}

void GameRenderer::render() {
    // Clear the screen
    renderer_->clear();
    
    // Render the game elements
    renderGame();
    
    // Render game over message if needed
    if (game_.isGameOver()) {
        renderGameOver();
    }
    
    // Present the rendered content
    renderer_->present();
}

void GameRenderer::renderGame() {
    // Draw the game grid
    renderer_->drawGrid(game_.getGrid());
    
    // Draw current tetromino and ghost piece
    const Tetromino* currentTetromino = tetrominoManager_.getCurrentTetromino();
    
    if (currentTetromino) {
        renderer_->drawTetromino(*currentTetromino);
        renderer_->drawGhostPiece(game_, *currentTetromino);
    }
    
    // Draw UI elements
    renderer_->drawSidebar(game_, tetrominoManager_.getNextTetrominoType());
}

void GameRenderer::renderGameOver() {
    renderer_->drawGameOver(game_.getScore());
}
