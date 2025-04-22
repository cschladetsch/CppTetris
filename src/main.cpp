#include "Game.h"
#include <iostream>
#include <exception>

int main(int argc, char* args[]) {
    try {
        Game().run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
