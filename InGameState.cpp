#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/glu.h>
#include <SFML/OpenGL.hpp>
#include <cmath>

#include "InGameState.h"

#pragma comment(lib, "glu32.lib")

GLuint floorVBO;
GLuint wallVBO;
GLuint textureID;
sf::Image image;

void loadFloorTexture() {
    if (!image.loadFromFile("floor_texture.png")) {
        // TODO handle exception
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glBindTexture(GL_TEXTURE_2D, 0);
}

InGameState::InGameState(std::unique_ptr<GameState>& currentState, sf::RenderWindow& window) : currentState(currentState) {

    collectedPages = 0;
    cameraPosX = 0.0f;
    cameraPosY = 3.0f;
    cameraPosZ = 3.0f;
    cameraLookAtX = 0.0f;
    cameraLookAtY = 3.0f;
    cameraLookAtZ = .0f;
    cameraUpX = 0.0f;
    cameraUpY = 0.1f;
    cameraUpZ = 0.0f;

    distanceToTarget = 5.0f;
    rotationSpeed = 1.0f;

    cameraAngle = 0.0f;

    if (!heartBeatSound.openFromFile("hearbeat-71701.mp3")) {
        // TODO handle exception
    }
    heartBeatSound.setLoop(true);

    if (!BreathingSound.openFromFile("heavy-breathing-14431.mp3")) {
        // TODO handle exception
    }
    BreathingSound.setLoop(true);

    if (!loudSilence.openFromFile("loud-silence-75803.mp3")) {
        // TODO handle exception
    }
    loudSilence.setLoop(true);

    if (!MaskBreathing.openFromFile("mask-breathing-for-killer-or-other-63789.mp3")) {
        // TODO handle exception
    }
    MaskBreathing.setLoop(true);

    if (!DemonicWoman.openFromFile("demonic-woman-scream-6333.mp3")) {
        // TODO handle exception
    }

    //heartBeatSound.play();

    initFloor();
    loadFloorTexture();

    initWall();
}

void InGameState::initFloor() {
    // Przyk³adowe dane wierzcho³ków dla prostok¹ta
    GLfloat floorVertices[] = {
        // Wierzcho³ki         // Kolory        // Tekstura
        -50.0f, 0.0f, 50.0f,     0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Lewy górny
         50.0f, 0.0f, 50.0f,     0.5f, 0.5f, 1.0f, 100.0f, 0.0f, // Prawy górny
         50.0f, 0.0f, -50.0f,    0.5f, 0.5f, 1.0f, 100.0f, 100.0f, // Prawy dolny
        -50.0f, 0.0f, -50.0f,    0.5f, 0.5f, 1.0f, 0.0f, 100.0f  // Lewy dolny
    };

    // Generowanie i wi¹zanie VBO
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    // W³¹czenie atrybutów wierzcho³ków (jeœli u¿ywasz)
    // glVertexPointer, glColorPointer itp.
}

void InGameState::initWall() {
    // Przyk³adowe dane wierzcho³ków dla pionowego prostok¹ta (œciany)
    GLfloat wallVertices[] = {
        // Wierzcho³ki
        -50.0f, 0.0f, 50.0f,     // Lewy dolny
        -50.0f, 100.0f, 50.0f,    // Lewy górny
         50.0f, 100.0f, 50.0f,    // Prawy górny
         50.0f, 0.0f, 50.0f      // Prawy dolny
    };

    // Generowanie i wi¹zanie VBO dla œciany
    glGenBuffers(1, &wallVBO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
}


void InGameState::initCamera(sf::RenderWindow& window) {
    // Ustawienie macierzy projekcji
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)window.getSize().x / (GLfloat)window.getSize().y, 0.1f, 100.0f);

    // Prze³¹czenie na model/widok
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void InGameState::updateCamera() {
    glLoadIdentity();

    GLfloat lookAtX = cameraPosX + cos(cameraAngle);
    GLfloat lookAtZ = cameraPosZ + sin(cameraAngle);

    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,  // Pozycja kamery
        lookAtX, cameraPosY, lookAtZ,        // Punkt, na który kamera jest skierowana
        0.0f, 1.0f, 0.0f);                  // Wektor "góra"
}


void InGameState::handleInput(sf::RenderWindow& window) {
    const GLfloat moveSpeed = 0.1f; // Prêdkoœæ ruchu
    const GLfloat rotationSpeed = 0.03f; // Prêdkoœæ obrotu (w radianach)

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        cameraAngle -= rotationSpeed; // Obrót w lewo
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        cameraAngle += rotationSpeed; // Obrót w prawo
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        cameraPosX += cos(cameraAngle) * moveSpeed; // Ruch do przodu
        cameraPosZ += sin(cameraAngle) * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        cameraPosX -= cos(cameraAngle) * moveSpeed; // Ruch do ty³u
        cameraPosZ -= sin(cameraAngle) * moveSpeed;

    }

}

void InGameState::update(sf::RenderWindow& window) {


}

void InGameState::render(sf::RenderWindow& window) {
    window.setActive(true);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initCamera(window);
    updateCamera();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID); // Za³ó¿my, ¿e to jest tekstura pod³ogi
    // Rysowanie pod³ogi
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glDrawArrays(GL_QUADS, 0, 4); // lub GL_TRIANGLES
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Wy³¹czenie teksturowania
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    // Od³¹czenie VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void InGameState::handleMusic(sf::Music& music) {
    music.stop();

    if (newPage) {
        newPage = false;
        switch (collectedPages) {
        case 0:
            heartBeatSound.play();
            break;
        case 1:
            BreathingSound.play();
            break;
        case 2:
            loudSilence.play();
            break;
        case 3:
            MaskBreathing.play();
            break;
        case 4:
            DemonicWoman.play();
            break;
        default:
            break;
        }
    }
}