#pragma once

#include <GL/glew.h>
#include <GL/glu.h>
#include <SFML/Graphics.hpp>

#include "GameState.h"

class InGameState : public GameState {

private:
    std::unique_ptr<GameState>& currentState;

    void initFloor();
    void initWall();
    void initCamera(sf::RenderWindow& window);
    void updateCamera();

    int collectedPages;
    bool newPage = true;
    GLfloat cameraPosX, cameraPosY, cameraPosZ;
    GLfloat cameraLookAtX, cameraLookAtY, cameraLookAtZ;
    GLfloat cameraUpX, cameraUpY, cameraUpZ;
    GLfloat cameraAngle;
    GLfloat distanceToTarget;
    GLfloat rotationSpeed;

    sf::Clock clock;
    
    sf::Music heartBeatSound;
    sf::Music BreathingSound;
    sf::Music loudSilence;
    sf::Music MaskBreathing;
    sf::Music DemonicWoman;

public:
    InGameState(std::unique_ptr<GameState>& currentState, sf::RenderWindow& window);
    void handleInput(sf::RenderWindow& window) override;
    void update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleMusic(sf::Music& music) override;
};