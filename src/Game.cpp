#include "Game.h"
#include "Color.h"
#include <iostream>
#include <algorithm>
#include <format>

Game::Game() : 
    window_(nullptr, SDL_DestroyWindow),
    renderer_(nullptr, SDL_DestroyRenderer),
    font_(nullptr, TTF_CloseFont),
    grid_(GRID_HEIGHT, std::vector<std::optional<TetrominoType>>(GRID_WIDTH)),
    gameOver_(false),
    score_(0),
    level_(1),
    linesCleared_(0) {
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize TTF
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create window
    window_.reset(SDL_CreateWindow("Tetris C++23", 
                                   SDL_WINDOWPOS_UNDEFINED, 
                                   SDL_WINDOWPOS_UNDEFINED, 
                                   WINDOW_WIDTH, 
                                   WINDOW_HEIGHT, 
                                   SDL_WINDOW_SHOWN));
    if (!window_) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create renderer
    renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED));
    if (!renderer_) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Try to load font from multiple possible locations with fonts that support Unicode
    font_.reset(TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24));
    if (!font_) {
        font_.reset(TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 24));
    }
    if (!font_) {
        font_.reset(TTF_OpenFont("/usr/share/fonts/noto/NotoSans-Regular.ttf", 24));
    }
    if (!font_) {
        font_.reset(TTF_OpenFont("resources/fonts/DejaVuSans.ttf", 24));
    }
    if (!font_) {
        font_.reset(TTF_OpenFont("DejaVuSans.ttf", 24));
    }
    if (!font_) {
        font_.reset(TTF_OpenFont("resources/fonts/Arial.ttf", 24));
    }
    if (!font_) {
        font_.reset(TTF_OpenFont("Arial.ttf", 24));
    }
    if (!font_) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        std::cerr << "Will continue without font - using blocks for score display." << std::endl;
    }

    // Create the renderer instance
    renderer_instance_ = std::make_unique<Renderer>(renderer_.get(), font_.get());

    // Seed random generator
    std::random_device rd;
    rng_.seed(rd());

    // Generate the first next tetromino type
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    // Create the first tetromino
    createNewTetromino();
}

Game::~Game() {
    TTF_Quit();
    SDL_Quit();
}

void Game::run() {
    bool quit = false;
    SDL_Event e;
    
    auto lastFallTime = std::chrono::steady_clock::now();
    auto lastFrameTime = std::chrono::steady_clock::now();
    
    while (!quit) {
        auto currentTime = std::chrono::steady_clock::now();
        
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN && !gameOver_) {
                handleKeyPress(e.key.keysym.sym);
            } else if (e.type == SDL_KEYDOWN && gameOver_ && e.key.keysym.sym == SDLK_RETURN) {
                resetGame();
            }
        }
        
        // Update game state
        if (!gameOver_) {
            auto fallSpeed = getFallSpeed();
            if (currentTime - lastFallTime > fallSpeed) {
                if (!moveTetromino(0, 1)) {
                    // Can't move down further, lock the piece
                    lockTetromino();
                    clearLines();
                    
                    if (!createNewTetromino()) {
                        gameOver_ = true;
                    }
                }
                lastFallTime = currentTime;
            }
        }
        
        // Render
        render();
        
        // Cap frame rate
        auto frameTime = std::chrono::steady_clock::now() - lastFrameTime;
        if (frameTime < 16ms) {  // Target ~60 FPS
            SDL_Delay(static_cast<Uint32>((16ms - frameTime).count() / 1000000));
        }
        lastFrameTime = std::chrono::steady_clock::now();
    }
}

void Game::render() {
    // Clear the screen
    renderer_instance_->clear();
    
    // Draw the game grid
    renderer_instance_->drawGrid(grid_);
    
    // Draw current tetromino and ghost piece
    if (currentTetromino_) {
        renderer_instance_->drawTetromino(*currentTetromino_);
        renderer_instance_->drawGhostPiece(*this, *currentTetromino_);
    }
    
    // Draw UI elements
    renderer_instance_->drawSidebar(*this);
    
    // Draw game over message if needed
    if (gameOver_) {
        renderer_instance_->drawGameOver(score_);
    }
    
    // Present the rendered content
    renderer_instance_->present();
}

bool Game::isPositionFree(int x, int y) const {
    if (x < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        return false;
    }
    
    // Above the grid is free
    if (y < 0) {
        return true;
    }
    
    return !grid_[y][x].has_value();
}

std::chrono::milliseconds Game::getFallSpeed() const {
    // Speed increases with level
    return std::chrono::milliseconds(
        static_cast<int>(INITIAL_FALL_SPEED.count() / (1 + level_ * 0.1))
    );
}

void Game::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:
            currentTetromino_->moveLeft(*this);
            break;
        case SDLK_RIGHT:
            currentTetromino_->moveRight(*this);
            break;
        case SDLK_DOWN: {
            if (!moveTetromino(0, 1)) {
                // Can't move down further, lock the piece
                lockTetromino();
                clearLines();
                
                if (!createNewTetromino()) {
                    gameOver_ = true;
                }
            }
            break;
        }
        case SDLK_UP:
            currentTetromino_->rotate(*this);
            break;
        case SDLK_SPACE: {
            // Hard drop - move down until collision
            int dropCount = 0;
            while (moveTetromino(0, 1)) {
                score_ += 1;  // Extra points for hard drop
                dropCount++;
                
                // Safety check to prevent infinite loop
                if (dropCount > GRID_HEIGHT) {
                    break;
                }
            }
            
            lockTetromino();
            clearLines();
            
            if (!createNewTetromino()) {
                gameOver_ = true;
            }
            break;
        }
    }
}

bool Game::moveTetromino(int dx, int dy) {
    if (!currentTetromino_) return false;
    
    int newX = currentTetromino_->x() + dx;
    int newY = currentTetromino_->y() + dy;
    int currentRotation = currentTetromino_->rotation();
    TetrominoType currentType = currentTetromino_->type();
    
    // Create a test tetromino at the target position
    Tetromino testTetromino(currentType, newX, newY);
    
    // Apply the same rotation
    for (int i = 0; i < currentRotation; i++) {
        testTetromino.rotateWithoutWallKick(); // Direct rotation without wall kicks
    }
    
    // Check if the new position is valid using the rotated shape
    auto shape = testTetromino.getRotatedShape();
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                if (!isPositionFree(newX + x, newY + y)) {
                    return false;
                }
            }
        }
    }
    
    // Update position directly on the current tetromino
    // This avoids creating a new object which might introduce inconsistencies
    if (dx != 0) {
        currentTetromino_->setPosition(newX, currentTetromino_->y());
    }
    if (dy != 0) {
        currentTetromino_->setPosition(currentTetromino_->x(), newY);
    }
    
    return true;
}

void Game::lockTetromino() {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int gridX = currentTetromino_->x() + x;
            int gridY = currentTetromino_->y() + y;
            
            if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                if (currentTetromino_->isOccupying(gridX, gridY)) {
                    grid_[gridY][gridX] = currentTetromino_->type();
                }
            }
        }
    }
}

void Game::clearLines() {
    int linesCleared = 0;
    
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        bool lineComplete = true;
        
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (!grid_[y][x].has_value()) {
                lineComplete = false;
                break;
            }
        }
        
        if (lineComplete) {
            // Clear the line
            for (int yy = y; yy > 0; yy--) {
                grid_[yy] = grid_[yy - 1];
            }
            
            // Clear the top line
            std::fill(grid_[0].begin(), grid_[0].end(), std::optional<TetrominoType>{});
            
            linesCleared++;
            y++; // Recheck this position
        }
    }
    
    if (linesCleared > 0) {
        // Update score based on number of lines cleared
        static const std::array<int, 4> lineScores = {100, 300, 500, 800};
        score_ += lineScores[std::min(linesCleared, 4) - 1] * level_;
        
        linesCleared_ += linesCleared;
        
        // Update level
        level_ = std::min(1 + linesCleared_ / LINES_PER_LEVEL, MAX_LEVEL);
    }
}

bool Game::createNewTetromino() {
    // Use the next tetromino type that was previously generated
    TetrominoType type = nextTetrominoType_;
    
    // Generate the next tetromino type for preview
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    // Initial position - centered at the top with an offset for the I piece
    int startX = GRID_WIDTH / 2 - 2;
    // Start higher for I piece to give more space
    int startY = (type == TetrominoType::I) ? -1 : 0;
    
    // Create the new tetromino
    currentTetromino_ = std::make_unique<Tetromino>(type, startX, startY);
    
    // Check if the new tetromino can be placed - only check visible cells
    // This gives the player a chance to move/rotate before collision
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentTetromino_->isOccupying(startX + x, startY + y)) {
                // Only check collisions below y=0 (visible grid area)
                if (startY + y >= 0 && !isPositionFree(startX + x, startY + y)) {
                    return false;  // Game over
                }
            }
        }
    }
    
    return true;
}

void Game::resetGame() {
    // Clear grid
    for (auto& row : grid_) {
        std::fill(row.begin(), row.end(), std::optional<TetrominoType>{});
    }
    
    // Reset game state
    score_ = 0;
    level_ = 1;
    linesCleared_ = 0;
    gameOver_ = false;
    
    // Generate the new next tetromino type
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::COUNT) - 1);
    nextTetrominoType_ = static_cast<TetrominoType>(dist(rng_));
    
    // Create new tetromino
    createNewTetromino();
}
