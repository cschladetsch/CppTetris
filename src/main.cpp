#include "Game.h"
#include <iostream>
#include <exception>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* args[]) {
    try {
        Game game;
        game.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
