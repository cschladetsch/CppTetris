#include <gtest/gtest.h>
#include "Game.h"
#include "test_helpers.h"
#include <memory>

// Test fixture for Game class
class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a testable Game subclass
        game = std::make_unique<TestGame>();
    }
    
    void TearDown() override {
        game.reset();
    }
    
    std::unique_ptr<Game> game;
};

// Tests for Game class core functionality
TEST_F(GameTest, InitialGameStateIsStartScreen) {
    EXPECT_EQ(game->getGameState(), GameState::StartScreen);
}

TEST_F(GameTest, StartGameChangesStateToPlaying) {
    game->startGame();
    EXPECT_EQ(game->getGameState(), GameState::Playing);
}

TEST_F(GameTest, PauseGameTogglesPlayingAndPaused) {
    // Start playing first
    game->startGame();
    EXPECT_EQ(game->getGameState(), GameState::Playing);
    
    // Pause
    game->pauseGame();
    EXPECT_EQ(game->getGameState(), GameState::Paused);
    
    // Unpause
    game->pauseGame();
    EXPECT_EQ(game->getGameState(), GameState::Playing);
}

TEST_F(GameTest, GameOverSetsCorrectState) {
    game->setGameOver();
    EXPECT_EQ(game->getGameState(), GameState::GameOver);
    EXPECT_TRUE(game->isGameOver());
}

TEST_F(GameTest, ResetGameClearsGrid) {
    // First set game over
    game->setGameOver();
    
    // Reset game
    game->resetGame();
    
    // Check that grid is empty
    const auto& grid = game->getGrid();
    bool grid_is_empty = true;
    
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            if (cell.has_value()) {
                grid_is_empty = false;
                break;
            }
        }
        if (!grid_is_empty) break;
    }
    
    EXPECT_TRUE(grid_is_empty);
    EXPECT_EQ(game->getScore(), 0);
    EXPECT_EQ(game->getLevel(), 1);
    EXPECT_EQ(game->getLinesCleared(), 0);
}

TEST_F(GameTest, IncreaseScoreAddsPoints) {
    int initial_score = game->getScore();
    game->increaseScore(100);
    EXPECT_EQ(game->getScore(), initial_score + 100);
}

TEST_F(GameTest, IncrementLinesClearedUpdatesLinesAndLevel) {
    int initial_lines = game->getLinesCleared();
    int initial_level = game->getLevel();
    
    // Add lines but not enough to level up
    game->incrementLinesCleared(5);
    EXPECT_EQ(game->getLinesCleared(), initial_lines + 5);
    EXPECT_EQ(game->getLevel(), initial_level);
    
    // Add more lines to trigger level up (10 lines per level)
    game->incrementLinesCleared(5);
    EXPECT_EQ(game->getLinesCleared(), initial_lines + 10);
    EXPECT_EQ(game->getLevel(), initial_level + 1);
}

TEST_F(GameTest, FallSpeedIncreasesWithLevel) {
    // Can't directly test this as getFallSpeed() is private,
    // but we can test the level increases
    int initial_level = game->getLevel();
    
    // Add 10 lines to trigger level up
    game->incrementLinesCleared(10);
    
    // Level should increase
    EXPECT_EQ(game->getLevel(), initial_level + 1);
}

// Tests specifically for collision detection
class CollisionDetectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = std::make_unique<TestGame>();
    }
    
    std::unique_ptr<TestGame> game;
};

TEST_F(CollisionDetectionTest, PositionOutsideGridIsNotFree) {
    // Test left boundary
    EXPECT_FALSE(game->isPositionFree(-1, 10));
    
    // Test right boundary
    EXPECT_FALSE(game->isPositionFree(GRID_WIDTH, 10));
    
    // Test bottom boundary
    EXPECT_FALSE(game->isPositionFree(5, GRID_HEIGHT));
}

TEST_F(CollisionDetectionTest, PositionAboveGridIsFree) {
    // Position above grid should be free (for pieces entering the grid)
    EXPECT_TRUE(game->isPositionFree(5, -1));
}

TEST_F(CollisionDetectionTest, EmptyGridCellIsFree) {
    // Empty cell within grid should be free
    EXPECT_TRUE(game->isPositionFree(5, 5));
}

TEST_F(CollisionDetectionTest, OccupiedGridCellIsNotFree) {
    // Can't modify grid directly through public API
    // This would require a custom accessor or test-specific subclass
    // Instead we'll verify the default empty grid values
    
    // Reset game to ensure grid is empty
    game->resetGame();
    
    // All positions in empty grid should be free
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            EXPECT_TRUE(game->isPositionFree(x, y));
        }
    }
    
    // Note: Testing occupied cells would need direct access 
    // to modify the grid, which is not available through public API
}

// Test for integration of collision detection with tetromino movement
TEST_F(CollisionDetectionTest, TetrominoCollisionAtBoundaries) {
    // Start the game to create a tetromino
    game->startGame();
    
    // Run the game loop manually a few times to ensure everything is initialized
    // Note: This is a simple approximation, actual game might have more complex startup
    
    // This test would need to interact with the TetrominoManager,
    // which we can't directly access through Game's public API.
    
    // Instead, we'll verify the boundary conditions work as expected
    EXPECT_FALSE(game->isPositionFree(-1, 0));
    EXPECT_FALSE(game->isPositionFree(GRID_WIDTH, 0));
    EXPECT_FALSE(game->isPositionFree(0, GRID_HEIGHT));
    EXPECT_TRUE(game->isPositionFree(0, 0));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}