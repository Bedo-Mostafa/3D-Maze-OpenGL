#include "Game.h"
#include <iostream>
#include <memory>

int main() {
    try {
        auto game = std::make_unique<Game>();
        
        if (!game->initialize()) {
            std::cerr << "Failed to initialize game" << std::endl;
            return 1;
        }

        game->run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}