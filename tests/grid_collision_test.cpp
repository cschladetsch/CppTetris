#include <gtest/gtest.h>
#include "test_helpers.h"
#include "Constants.h"
#include <memory>

// Test fixture for grid and collision detection
class GridCollisionTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = std::make_unique<TestGame>();
    }
    
    std::unique_ptr<TestGame> game;
};

// Test basic grid boundary checking
TEST_F(GridCollisionTest, GridBoundaryCheck) {
    // Check boundaries
    EXPECT_FALSE(game->isPositionFree(-1, 0));        // Left boundary
    EXPECT_FALSE(game->isPositionFree(GRID_WIDTH, 0)); // Right boundary
    EXPECT_FALSE(game->isPositionFree(0, GRID_HEIGHT)); // Bottom boundary
    
    // Check valid positions within grid
    EXPECT_TRUE(game->isPositionFree(0, 0));          // Top-left
    EXPECT_TRUE(game->isPositionFree(GRID_WIDTH-1, 0)); // Top-right
    EXPECT_TRUE(game->isPositionFree(0, GRID_HEIGHT-1)); // Bottom-left
    EXPECT_TRUE(game->isPositionFree(GRID_WIDTH-1, GRID_HEIGHT-1)); // Bottom-right
    
    // Check positions above the grid (should be free for tetrominos entering)
    EXPECT_TRUE(game->isPositionFree(GRID_WIDTH/2, -1));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}