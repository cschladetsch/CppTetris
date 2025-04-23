#include "Game.h"
#include "Color.h"
#include <format>

constexpr int SCREEN_CLEAR_COLOR_R = 25;
constexpr int SCREEN_CLEAR_COLOR_G = 25;
constexpr int SCREEN_CLEAR_COLOR_B = 25;
constexpr int SCREEN_CLEAR_COLOR_A = 255;

constexpr int GRID_LINE_COLOR = 50;
constexpr int EMPTY_CELL_COLOR = 40;
constexpr int ALPHA_OPAQUE = 255;

constexpr int TETROMINO_GRID_SIZE = 4;

constexpr int TEXT_COLOR_R = 200;
constexpr int TEXT_COLOR_G = 200;
constexpr int TEXT_COLOR_B = 200;

constexpr int TEXT_FALLBACK_CHAR_WIDTH = 8;
constexpr int TEXT_FALLBACK_HEIGHT = 20;

constexpr int OVERLAY_COLOR_R = 0;
constexpr int OVERLAY_COLOR_G = 0;
constexpr int OVERLAY_COLOR_B = 0;
constexpr int OVERLAY_COLOR_A = 200;

constexpr int GAME_OVER_OFFSET_X = 80;
constexpr int GAME_OVER_OFFSET_Y = 30;

void Game::render() {
    SDL_SetRenderDrawColor(renderer_.get(), SCREEN_CLEAR_COLOR_R, SCREEN_CLEAR_COLOR_G, SCREEN_CLEAR_COLOR_B, SCREEN_CLEAR_COLOR_A);
    SDL_RenderClear(renderer_.get());

    drawGrid();

    if (currentTetromino_) {
        drawTetromino(*currentTetromino_);
        drawGhostPiece();
    }

    drawSidebar();

    if (gameOver_) {
        drawGameOver();
    }

    SDL_RenderPresent(renderer_.get());
}

void Game::drawGrid() {
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 1;
    rect.h = BLOCK_SIZE - 1;

    SDL_SetRenderDrawColor(renderer_.get(), GRID_LINE_COLOR, GRID_LINE_COLOR, GRID_LINE_COLOR, ALPHA_OPAQUE);
    SDL_Rect border = {0, 0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE};
    SDL_RenderDrawRect(renderer_.get(), &border);

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            rect.x = x * BLOCK_SIZE;
            rect.y = y * BLOCK_SIZE;

            if (grid_[y][x].has_value()) {
                const auto& color = COLORS[static_cast<std::size_t>(grid_[y][x].value())];
                SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer_.get(), &rect);

                SDL_SetRenderDrawColor(renderer_.get(), color.r / 2, color.g / 2, color.b / 2, ALPHA_OPAQUE);
                SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                SDL_RenderDrawRect(renderer_.get(), &border);
            } else {
                SDL_SetRenderDrawColor(renderer_.get(), EMPTY_CELL_COLOR, EMPTY_CELL_COLOR, EMPTY_CELL_COLOR, ALPHA_OPAQUE);
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
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, ALPHA_OPAQUE);

    for (int y = 0; y < TETROMINO_GRID_SIZE; y++) {
        for (int x = 0; x < TETROMINO_GRID_SIZE; x++) {
            if (tetromino.isOccupying(tetromino.x() + x, tetromino.y() + y)) {
                rect.x = (tetromino.x() + x) * BLOCK_SIZE;
                rect.y = (tetromino.y() + y) * BLOCK_SIZE;

                if (rect.y >= 0) {
                    SDL_RenderFillRect(renderer_.get(), &rect);

                    SDL_SetRenderDrawColor(renderer_.get(), color.r / 2, color.g / 2, color.b / 2, ALPHA_OPAQUE);
                    SDL_Rect border = {rect.x, rect.y, rect.w, rect.h};
                    SDL_RenderDrawRect(renderer_.get(), &border);

                    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, ALPHA_OPAQUE);
                }
            }
        }
    }
}

void Game::drawText(const std::string& text, int x, int y) {
    if (font_) {
        SDL_Color textColor = {TEXT_COLOR_R, TEXT_COLOR_G, TEXT_COLOR_B, ALPHA_OPAQUE};

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
        SDL_SetRenderDrawColor(renderer_.get(), TEXT_COLOR_R, TEXT_COLOR_G, TEXT_COLOR_B, ALPHA_OPAQUE);
        SDL_Rect rect = {x, y, static_cast<int>(text.length() * TEXT_FALLBACK_CHAR_WIDTH), TEXT_FALLBACK_HEIGHT};
        SDL_RenderDrawRect(renderer_.get(), &rect);
    }
}

void Game::drawGameOver() {
    SDL_SetRenderDrawColor(renderer_.get(), OVERLAY_COLOR_R, OVERLAY_COLOR_G, OVERLAY_COLOR_B, OVERLAY_COLOR_A);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer_.get(), &overlay);

    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;

    drawText("GAME OVER", centerX - GAME_OVER_OFFSET_X, centerY - GAME_OVER_OFFSET_Y);
    drawText(std::format("Final Score: {}", score_), centerX - GAME_OVER_OFFSET_X, centerY);
    drawText("Press ENTER to restart", centerX - GAME_OVER_OFFSET_X - 30, centerY + GAME_OVER_OFFSET_Y);
}
