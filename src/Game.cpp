#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game(bool test_mode) : 
    window_(nullptr, SDL_DestroyWindow),
    renderer_(nullptr, SDL_DestroyRenderer),
    font_(nullptr, TTF_CloseFont),
    grid_(GRID_HEIGHT, std::vector<std::optional<TetrominoType>>(GRID_WIDTH)),
    gameState_(GameState::StartScreen),  // Start with the start screen
    quit_(false),
    score_(0),
    level_(INITIAL_LEVEL),
    linesCleared_(0) {
    
    if (!test_mode) {
        initSDL();
        
        soundManager_ = std::make_unique<SoundManager>();
        if (!soundManager_->initialize()) {
            std::cerr << "Warning: Sound system could not be initialized. Continuing without sound." << std::endl;
        } else if (!soundManager_->loadSounds()) {
            std::cerr << "Warning: Some sound effects could not be loaded." << std::endl;
        }
        
        // Create component managers - order matters due to dependencies
        tetrominoManager_ = std::make_unique<TetrominoManager>(*this);
        inputHandler_ = std::make_unique<InputHandler>(*this, *tetrominoManager_);
        gameRenderer_ = std::make_unique<GameRenderer>(*this, *tetrominoManager_, renderer_.get(), font_.get());
    } else {
        // Just create the tetromino manager for test mode
        soundManager_ = std::make_unique<SoundManager>();
        tetrominoManager_ = std::make_unique<TetrominoManager>(*this);
    }
    
    resetGame();
}

Game::~Game() {
    // Only quit SDL if window was initialized (not in test mode)
    if (window_) {
        TTF_Quit();
        SDL_Quit();
    }
}

void Game::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

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

    renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED));
    if (!renderer_) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    loadFont();
}

void Game::loadFont() {
    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/noto/NotoSans-Regular.ttf",
        "resources/fonts/DejaVuSans.ttf",
        "DejaVuSans.ttf",
        "resources/fonts/Arial.ttf",
        "Arial.ttf"
    };
    
    for (const char* path : fontPaths) {
        font_.reset(TTF_OpenFont(path, FONT_SIZE));
        if (font_) break;
    }
    
    if (!font_) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        std::cerr << "Will continue without font - using blocks for score display." << std::endl;
    }
}

void Game::run() {
    auto lastFallTime = std::chrono::steady_clock::now();
    auto lastFrameTime = std::chrono::steady_clock::now();
    
    while (!quit_) {
        auto currentTime = std::chrono::steady_clock::now();
        
        quit_ = inputHandler_->processEvents();
        
        if (gameState_ == GameState::Playing) {
            updateGameState(currentTime, lastFallTime);
        }
        
        gameRenderer_->render();
        
        capFrameRate(lastFrameTime);
    }
}

void Game::updateGameState(const std::chrono::steady_clock::time_point& currentTime, 
                          std::chrono::steady_clock::time_point& lastFallTime) {
    auto fallSpeed = getFallSpeed();
    if (currentTime - lastFallTime > fallSpeed) {
        if (!tetrominoManager_->moveTetromino(NO_MOVE, MOVE_DOWN)) {
            tetrominoManager_->lockTetromino();
            tetrominoManager_->clearLines();
            
            if (!tetrominoManager_->createNewTetromino()) {
                setGameOver();
            }
        }
        lastFallTime = currentTime;
    }
}

void Game::capFrameRate(std::chrono::steady_clock::time_point& lastFrameTime) {
    auto frameTime = std::chrono::steady_clock::now() - lastFrameTime;
    if (frameTime < TARGET_FRAME_TIME) {
        SDL_Delay(static_cast<Uint32>((TARGET_FRAME_TIME - frameTime).count() / MICROSECONDS_PER_MILLISECOND));
    }
    lastFrameTime = std::chrono::steady_clock::now();
}

std::chrono::milliseconds Game::getFallSpeed() const {
    // Speed increases with level
    return std::chrono::milliseconds(
        static_cast<int>(INITIAL_FALL_SPEED.count() / (1 + level_ * LEVEL_SPEED_FACTOR))
    );
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

void Game::incrementLinesCleared(int lines) {
    linesCleared_ += lines;
    
    // Check if we leveled up
    int oldLevel = level_;
    level_ = std::min(INITIAL_LEVEL + linesCleared_ / LINES_PER_LEVEL, MAX_LEVEL);
    
    if (level_ > oldLevel) {
        playLevelUpSound();
    }
}

void Game::resetGame() {
    // Clear grid
    for (auto& row : grid_) {
        std::fill(row.begin(), row.end(), std::optional<TetrominoType>{});
    }
    
    // Reset game state
    score_ = 0;
    level_ = INITIAL_LEVEL;
    linesCleared_ = 0;
    
    // Create new tetromino
    tetrominoManager_->createNewTetromino();
}
