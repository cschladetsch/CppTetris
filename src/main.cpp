#include "Game.h"
#include <iostream>
#include <exception>

int main(int argc, char* args[]) {
    try {
        return Game().run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
