#pragma once

#include <memory>
#include <random>
#include <vector>
#include <optional>
#include "Tetromino.h"
#include "Constants.h"

class Game;

class TetrominoManager {
public:
    TetrominoManager(Game& game);
    
    // Tetromino control
    bool moveTetromino(int dx, int dy);
    void rotateTetromino();
    void softDrop();
    void hardDrop();
    
    // Game mechanics
    void lockTetromino();
    void clearLines();
    bool createNewTetromino();
    
    // Accessors
    const Tetromino* getCurrentTetromino() const { return currentTetromino_.get(); }
    TetrominoType getNextTetrominoType() const { return nextTetrominoType_; }
    
private:
    Game& game_;
    std::unique_ptr<Tetromino> currentTetromino_;
    TetrominoType nextTetrominoType_;
    std::mt19937 rng_;
    
    // Helper methods
    bool isValidPosition(const Tetromino& tetromino) const;
    bool canPlaceNewTetromino() const;
    void generateNextTetrominoType();
    void initRng();
    void calculateScoreAndUpdateLevel(int linesCleared);
};
// TetrominoManager.h
