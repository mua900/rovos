#include "game.hpp"

GameState::~GameState() {
    for (auto ico : this->icons) {
        SDL_DestroyTexture(ico.texture);
    }
}
