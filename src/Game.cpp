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
        // Continue without font - we'll use blocks for score
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

void Game::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:
            currentTetromino_->moveLeft(*this);
            break;
        case SDLK_RIGHT:
            currentTetromino_->moveRight(*this);
            break;
        case SDLK_DOWN:
            if (!moveTetromino(0, 1)) {
                // Can't move down further, lock the piece
                lockTetromino();
                clearLines();
                
                if (!createNewTetromino()) {
                    gameOver_ = true;
                }
            }
            break;
        case SDLK_UP:
            currentTetromino_->rotate(*this);
            break;
        case SDLK_SPACE:
            // Hard drop
            while (moveTetromino(0, 1)) {
                score_ += 1;  // Extra points for hard drop
            }
            lockTetromino();
            clearLines();
            
            if (!createNewTetromino()) {
                gameOver_ = true;
            }
            break;
    }
}

bool Game::moveTetromino(int dx, int dy) {
    int newX = currentTetromino_->x() + dx;
    int newY = currentTetromino_->y() + dy;
    
    // Check if new position is valid
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentTetromino_->isOccupying(currentTetromino_->x() + x, currentTetromino_->y() + y)) {
                if (!isPositionFree(newX + x, newY + y)) {
                    return false;
                }
            }
        }
    }
    
    // Update position while preserving rotation
    int currentRotation = currentTetromino_->rotation();
    currentTetromino_.reset(new Tetromino(currentTetromino_->type(), newX, newY));
    
    // Apply the saved rotation
    for (int i = 0; i < currentRotation; i++) {
        currentTetromino_->rotate(*this);
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
    
    currentTetromino_ = std::make_unique<Tetromino>(type, GRID_WIDTH / 2 - 2, 0);
    
    // Check if new tetromino can be placed
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int gridX = currentTetromino_->x() + x;
            int gridY = currentTetromino_->y() + y;
            
            if (currentTetromino_->isOccupying(gridX, gridY)) {
                if (!isPositionFree(gridX, gridY)) {
                    return false;  // Game over
                }
            }
        }
    }
    
    return true;
}

void Game::render() {
    // Clear screen with dark background
    SDL_SetRenderDrawColor(renderer_.get(), 25, 25, 25, 255);
    SDL_RenderClear(renderer_.get());
    
    // Draw grid
    drawGrid();
    
    // Draw current tetromino
    if (currentTetromino_) {
        drawTetromino(*currentTetromino_);
        
        // Draw ghost piece (preview of where piece will land)
        drawGhostPiece();
    }
    
    // Draw score and level
    drawSidebar();
    
    // Draw game over message if needed
    if (gameOver_) {
        drawGameOver();
    }
    
    SDL_RenderPresent(renderer_.get());
}

void Game::drawGrid() {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    // Draw outline
    SDL_SetRenderDrawColor(renderer_.get(), 50, 50, 50, 255);
    SDL_Rect border = {0, 0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE};
    SDL_RenderDrawRect(renderer_.get(), &border);
    
    // Draw filled cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            rect.x = x * BLOCK_SIZE;
            rect.y = y * BLOCK_SIZE;
            
            if (grid_[y][x].has_value()) {
                const auto& color = COLORS[static_cast<std::size_t>(grid_[y][x].value())];
                SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
                SDL_RenderFillRect(renderer_.get(), &rect);
                
                // Add 3D effect with darker borders
                SDL_SetRenderDrawColor(renderer_.get(), 
                                     color.r/2, color.g/2, color.b/2, 255);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_.get(), &border);
            } else {
                // Draw empty cell
                SDL_SetRenderDrawColor(renderer_.get(), 40, 40, 40, 255);
                SDL_RenderDrawRect(renderer_.get(), &rect);
            }
        }
    }
}

void Game::drawTetromino(const Tetromino& tetromino) {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    const auto& color = COLORS[static_cast<std::size_t>(tetromino.type())];
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino.isOccupying(tetromino.x() + x, tetromino.y() + y)) {
                rect.x = (tetromino.x() + x) * BLOCK_SIZE;
                rect.y = (tetromino.y() + y) * BLOCK_SIZE;
                
                if (rect.y >= 0) {  // Only draw if visible
                    SDL_RenderFillRect(renderer_.get(), &rect);
                    
                    // Add 3D effect
                    SDL_SetRenderDrawColor(renderer_.get(), 
                                         color.r/2, color.g/2, color.b/2, 255);
                    SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                    SDL_RenderDrawRect(renderer_.get(), &border);
                    
                    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
                }
            }
        }
    }
}

void Game::drawGhostPiece() {
    if (!currentTetromino_) return;
    
    // Create a copy of the current tetromino
    auto ghostTetromino = std::make_unique<Tetromino>(
        currentTetromino_->type(),
        currentTetromino_->x(),
        currentTetromino_->y()
    );
    
    // Drop it as far as it can go
    int dropDistance = 0;
    while (true) {
        int newY = ghostTetromino->y() + 1;
        
        bool canDrop = true;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (ghostTetromino->isOccupying(ghostTetromino->x() + x, ghostTetromino->y() + y)) {
                    if (!isPositionFree(ghostTetromino->x() + x, newY + y)) {
                        canDrop = false;
                        break;
                    }
                }
            }
            if (!canDrop) break;
        }
        
        if (!canDrop) break;
        
        ghostTetromino = std::make_unique<Tetromino>(
            ghostTetromino->type(),
            ghostTetromino->x(),
            newY
        );
        dropDistance++;
    }
    
    // Draw the ghost piece if it's different from the current tetromino
    if (dropDistance > 0) {
        SDL_Rect rect;
        rect.w = BLOCK_SIZE - 1;
        rect.h = BLOCK_SIZE - 1;
        
        // Use a semi-transparent version of the color
        SDL_SetRenderDrawColor(renderer_.get(), 100, 100, 100, 128);
        
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (ghostTetromino->isOccupying(ghostTetromino->x() + x, ghostTetromino->y() + y)) {
                    rect.x = (ghostTetromino->x() + x) * BLOCK_SIZE;
                    rect.y = (ghostTetromino->y() + y) * BLOCK_SIZE;
                    
                    if (rect.y >= 0) {  // Only draw if visible
                        SDL_RenderDrawRect(renderer_.get(), &rect);
                    }
                }
            }
        }
    }
}

void Game::drawSidebar() {
    int sidebarX = GRID_WIDTH * BLOCK_SIZE + 10;
    int y = 20;
    
    // Draw "Next" label
    drawText("Next:", sidebarX, y);
    y += 30;
    
    // Draw next tetromino preview
    drawNextTetromino(sidebarX + BLOCK_SIZE, y);
    
    y += 5 * BLOCK_SIZE + 20;
    
    // Draw score
    drawText(std::format("Score: {}", score_), sidebarX, y);
    y += 40;
    
    // Draw level
    drawText(std::format("Level: {}", level_), sidebarX, y);
    y += 40;
    
    // Draw lines cleared
    drawText(std::format("Lines: {}", linesCleared_), sidebarX, y);
    y += 60;
    
    // Draw controls with Unicode arrows
    drawText("Controls:", sidebarX, y);
    y += 30;
    drawText("\u2190 \u2192 : Move", sidebarX, y);  // Unicode LEFT/RIGHT ARROW
    y += 30;
    drawText("\u2191 : Rotate", sidebarX, y);       // Unicode UP ARROW
    y += 30;
    drawText("\u2193 : Soft Drop", sidebarX, y);    // Unicode DOWN ARROW
    y += 30;
    drawText("Space : Hard Drop", sidebarX, y);
}

void Game::drawNextTetromino(int x, int y) {
    // Create a temporary tetromino for the preview
    Tetromino nextTetromino(nextTetrominoType_, 0, 0);
    
    // Calculate the center of the preview area
    int previewSize = 4 * BLOCK_SIZE;
    int centerX = x + previewSize / 2;
    int centerY = y + previewSize / 2;
    
    // Draw the tetromino blocks
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;
    
    const auto& color = COLORS[static_cast<std::size_t>(nextTetrominoType_)];
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
    
    // Draw the tetromino centered in the preview area
    auto shape = nextTetromino.getRotatedShape();
    
    // Calculate offsets to center the tetromino
    int offsetX = 0;
    int offsetY = 0;
    
    // Count rows and columns that have blocks
    int minRow = 4, maxRow = -1, minCol = 4, maxCol = -1;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col]) {
                minRow = std::min(minRow, row);
                maxRow = std::max(maxRow, row);
                minCol = std::min(minCol, col);
                maxCol = std::max(maxCol, col);
            }
        }
    }
    
    // If valid dimensions found, calculate centering offsets
    if (minRow <= maxRow && minCol <= maxCol) {
        int width = maxCol - minCol + 1;
        int height = maxRow - minRow + 1;
        offsetX = centerX - ((width * BLOCK_SIZE) / 2);
        offsetY = centerY - ((height * BLOCK_SIZE) / 2);
        
        // Adjust for the min row/col offset
        offsetX -= minCol * BLOCK_SIZE;
        offsetY -= minRow * BLOCK_SIZE;
    }
    
    // Draw the tetromino blocks
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col]) {
                rect.x = offsetX + (col * BLOCK_SIZE);
                rect.y = offsetY + (row * BLOCK_SIZE);
                
                SDL_RenderFillRect(renderer_.get(), &rect);
                
                // Add 3D effect
                SDL_SetRenderDrawColor(renderer_.get(), 
                                     color.r/2, color.g/2, color.b/2, 255);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_.get(), &border);
                
                SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, 255);
            }
        }
    }
    
    // Draw outline around the preview area
    SDL_SetRenderDrawColor(renderer_.get(), 50, 50, 50, 255);
    SDL_Rect previewRect = {x, y, previewSize, previewSize};
    SDL_RenderDrawRect(renderer_.get(), &previewRect);
}

void Game::drawText(const std::string& text, int x, int y) {
    if (font_) {
        SDL_Color textColor = {200, 200, 200, 255};
        
        // Use Blended rendering for better quality with Unicode characters
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font_.get(), text.c_str(), textColor);
        
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_.get(), surface);
            
            if (texture) {
                SDL_Rect rect = {x, y, surface->w, surface->h};
                SDL_RenderCopy(renderer_.get(), texture, nullptr, &rect);
                SDL_DestroyTexture(texture);
            }
            
            SDL_FreeSurface(surface);
        }
    } else {
        // Fallback if font loading failed: draw a colored rectangle
        SDL_SetRenderDrawColor(renderer_.get(), 200, 200, 200, 255);
        SDL_Rect rect = {x, y, static_cast<int>(text.length() * 8), 20};
        SDL_RenderDrawRect(renderer_.get(), &rect);
    }
}

void Game::drawGameOver() {
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_.get(), &overlay);
    
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    
    drawText("GAME OVER", centerX - 80, centerY - 30);
    drawText(std::format("Final Score: {}", score_), centerX - 80, centerY);
    drawText("Press ENTER to restart", centerX - 110, centerY + 30);
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
