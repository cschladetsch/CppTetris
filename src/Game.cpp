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

