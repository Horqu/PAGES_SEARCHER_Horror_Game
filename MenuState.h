#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "GameState.h"

class MenuState : public GameState {

private:
    std::unique_ptr<GameState>& currentState;
    std::vector<sf::Text> menuItems;
    sf::Font font;

public:
    MenuState(std::unique_ptr<GameState>& currentState, sf::RenderWindow& window);
    void handleInput(sf::RenderWindow& window) override;
    void update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleMusic(sf::Music& music) override;
};