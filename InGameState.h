#pragma once

#include <SFML/Graphics.hpp>

#include "GameState.h"

class InGameState : public GameState {

private:
    std::unique_ptr<GameState>& currentState;

public:
    InGameState(std::unique_ptr<GameState>& currentState, sf::RenderWindow& window);
    void handleInput(sf::RenderWindow& window) override;
    void update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleMusic(sf::Music& music) override;
};