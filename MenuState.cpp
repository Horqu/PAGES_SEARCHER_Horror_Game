#include "MenuState.h"
#include "CredentialsState.h"
#include "InGameState.h"

MenuState::MenuState(std::unique_ptr<GameState>& currentState, sf::RenderWindow& window) : currentState(currentState) {
    
    font.loadFromFile("WELATTE_BRUSH.ttf");
    
    std::string menuItemNames[] = { "New Game", "Credentials", "Exit" };
    float verticalSpacing = 250.0f;

    for (size_t i = 0; i < 3; i++) {
        sf::Text text(menuItemNames[i], font);
        text.setCharacterSize(72);
        text.setFillColor(sf::Color::White);

        sf::Vector2u windowSize = window.getSize();
        sf::FloatRect textRect = text.getLocalBounds();

        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(windowSize.x / 2.0f, (i + 1) * verticalSpacing));

        menuItems.push_back(text);
    }
}


void MenuState::handleInput(sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed) {
            for (size_t i = 0; i < menuItems.size(); i++) {
                if (menuItems[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {

                    switch (i) {
                        case 0: // "New Game"
                            switchState(currentState, std::make_unique<InGameState>(currentState, window));
                            break;
                        case 1: // "Credentials"
                            switchState(currentState, std::make_unique<CredentialsState>(currentState, window));
                            break;
                        case 2: // "Exit"
                            window.close();
                            break;
                        default:
                            break;
                    }

                }
            }
        }
    }
}

void MenuState::update(sf::RenderWindow& window) {

}

void MenuState::render(sf::RenderWindow& window) {

    for (const auto& item : menuItems) {
        window.draw(item);
    }
}

void MenuState::handleMusic(sf::Music& music) {

}