#include "Game.h"
#include <iostream>
#include <exception>

int main(int  /*unused*/, char* /*unused*/[] ) {
    try {
        return Game().run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
