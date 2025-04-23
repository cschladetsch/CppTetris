#include "Game.h"
#include <iostream>
#include <exception>

int main(int  /*unused*/, char* /*unused*/[] ) {
    try {
        Game().run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
