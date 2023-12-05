#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "InGameState.h"
#include "MenuState.h"
#include "CredentialsState.h"


int main() {
    sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();

    sf::RenderWindow window(fullScreenMode, "PAGES SEARCHER", sf::Style::Fullscreen);
    window.setVerticalSyncEnabled(true);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // TODO handle exception
    }

    std::unique_ptr<GameState> currentState;
    currentState = std::make_unique<MenuState>(currentState, window);

    sf::Music music;
    if (!music.openFromFile("rain-and-thunder-16705.mp3")) {
        // TODO handle exception
    }

    music.setLoop(true);
    music.play();

    while (window.isOpen()) {

        currentState->handleInput(window);
        currentState->update(window);

        window.clear(sf::Color::Black);

        currentState->render(window);
        currentState->handleMusic(music);

        window.display();
    }

    return 0;
}
