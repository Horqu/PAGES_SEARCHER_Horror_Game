#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "InGameState.h"
#include "MenuState.h"
#include "CredentialsState.h"


int main() {

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();

    sf::RenderWindow window(fullScreenMode, "PAGES SEARCHER", sf::Style::Fullscreen, settings);
    window.setVerticalSyncEnabled(true);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        return -1;
    }

    std::unique_ptr<GameState> currentState;
    currentState = std::make_unique<MenuState>(currentState, window);

    sf::Music music;
    if (!music.openFromFile("rain-and-thunder-16705.mp3")) {
        // TODO handle exception
    }

    music.setLoop(true);
    music.play();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (window.isOpen()) {

        currentState->handleInput(window);
        currentState->update(window);
        currentState->handleMusic(music);
        currentState->render(window);
        
        window.display();
    }

    return 0;
}
