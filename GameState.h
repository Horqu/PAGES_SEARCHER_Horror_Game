#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class GameState {
public:
    virtual void handleInput(sf::RenderWindow& window) = 0;
    virtual void update(sf::RenderWindow& window) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void handleMusic(sf::Music& music) = 0;
    virtual ~GameState() {}

    void switchState(std::unique_ptr<GameState>& currentState, std::unique_ptr<GameState> newState) {
        currentState = std::move(newState);
    }
};
