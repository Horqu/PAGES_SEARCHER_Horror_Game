#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SFML/OpenGL.hpp>
#include <cmath>

#include "InGameState.h"

#pragma comment(lib, "glu32.lib")

GLuint VBO, VAO;

GLuint floorVBO;
GLuint wallVBO;
sf::Image image;

GLuint loadTextureFromImage(const std::string& filename) {
    sf::Image image;
    if (!image.loadFromFile(filename)) {
        // TODO handle exception
    }

    
    image.flipVertically();
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLfloat maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

    
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;
}

InGameState::InGameState(std::unique_ptr<GameState>& currentState, sf::RenderWindow& window) : currentState(currentState) {

    collectedPages = 0;
    cameraPosX = 3.0f;
    cameraPosY = 1.0f;
    cameraPosZ = 5.0f;
    cameraLookAtX = 0.0f;
    cameraLookAtY = 0.0f;
    cameraLookAtZ = 0.0f;
    cameraUpX = 0.0f;
    cameraUpY = 0.1f;
    cameraUpZ = 0.0f;

    distanceToTarget = 5.0f;
    rotationSpeed = 1.0f;

    cameraAngle = 0.0f;

    floorTextureID = loadTextureFromImage("floor_texture.png");
    pageTextureID = loadTextureFromImage("floor_texture_with_page.png");

    endGameTimer = clock.getElapsedTime();

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

    if (!pagePickUpSound.openFromFile("item-equip-6904.mp3")) {
        // TODO handle exception
    }

    //heartBeatSound.play();

    //window.clear(sf::Color::White);

    window.setActive(true);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Kolor t³a

    //initCamera(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void InGameState::initFloor() {
    
    GLfloat floorVertices[] = {
        
        -50.0f, 0.0f, 50.0f,     0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 
         50.0f, 0.0f, 50.0f,     0.5f, 0.5f, 1.0f, 100.0f, 0.0f, 
         50.0f, 0.0f, -50.0f,    0.5f, 0.5f, 1.0f, 100.0f, 100.0f, 
        -50.0f, 0.0f, -50.0f,    0.5f, 0.5f, 1.0f, 0.0f, 100.0f  
    };

    
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);


}

void InGameState::initWall() {
    
    GLfloat wallVertices[] = {
        
        -50.0f, 0.0f, 50.0f,  
        -50.0f, 100.0f, 50.0f, 
         50.0f, 100.0f, 50.0f, 
         50.0f, 0.0f, 50.0f 
    };

    glGenBuffers(1, &wallVBO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
}


void InGameState::initCamera(sf::RenderWindow& window) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)window.getSize().x / (GLfloat)window.getSize().y, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void InGameState::updateCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1920 / 1080, 1.0, 100.0);

    GLfloat lookAtX = cameraPosX + cos(cameraAngle);
    GLfloat lookAtZ = cameraPosZ + sin(cameraAngle);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,  
        lookAtX, cameraPosY, lookAtZ,       
        0.0f, 1.0f, 0.0f);
}


void InGameState::handleInput(sf::RenderWindow& window) {
    const GLfloat moveSpeed = 0.1f; 
    const GLfloat rotationSpeed = 0.03f; 

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        cameraAngle -= rotationSpeed; 
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        cameraAngle += rotationSpeed; 
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {

        int newCameraPosXCeil = ceil(cameraPosX + cos(cameraAngle) * moveSpeed);
        int newCameraPosZCeil = ceil(cameraPosZ + sin(cameraAngle) * moveSpeed);
        int newCameraPosXFloor = floor(cameraPosX + cos(cameraAngle) * moveSpeed);
        int newCameraPosZFloor = floor(cameraPosZ + sin(cameraAngle) * moveSpeed);
        
        if (map[newCameraPosXCeil][newCameraPosZCeil] != 1 && map[newCameraPosXFloor][newCameraPosZFloor] != 1) {
            cameraPosX += cos(cameraAngle) * moveSpeed;
            cameraPosZ += sin(cameraAngle) * moveSpeed;
        }

    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {

        int newCameraPosXCeil = ceil(cameraPosX - cos(cameraAngle) * moveSpeed);
        int newCameraPosZCeil = ceil(cameraPosZ - sin(cameraAngle) * moveSpeed);
        int newCameraPosXFloor = floor(cameraPosX - cos(cameraAngle) * moveSpeed);
        int newCameraPosZFloor = floor(cameraPosZ - sin(cameraAngle) * moveSpeed);

        if (map[newCameraPosXCeil][newCameraPosZCeil] != 1 && map[newCameraPosXFloor][newCameraPosZFloor] != 1) {
            cameraPosX -= cos(cameraAngle) * moveSpeed;
            cameraPosZ -= sin(cameraAngle) * moveSpeed;
        }

    }

}

void InGameState::update(sf::RenderWindow& window) {

    if (map[static_cast<int>(cameraPosX)][static_cast<int>(cameraPosZ)] == 2) {

        ++collectedPages;
        newPage = true;
        map[static_cast<int>(cameraPosX)][static_cast<int>(cameraPosZ)] = 0;
        pagePickUpSound.play();
        if (collectedPages == 4) {
            clock.restart();
        }
    }

    endGameTimer = clock.getElapsedTime();
    if (collectedPages == 4 && endGameTimer.asSeconds() > 5) {
        window.close();
    }

}

void InGameState::renderFloor(int x, int y) {

    glEnable(GL_TEXTURE_2D);

    switch (map[x][y]) {
    case 0:
        glBindTexture(GL_TEXTURE_2D, floorTextureID);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); // Dolny lewy
        glTexCoord2f(256.0f, 0.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); // Dolny prawy
        glTexCoord2f(256.0f, 256.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); // Gorny prawy
        glTexCoord2f(0.0f, 256.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); // Gorny lewy
        glEnd();
        break;
    case 2:
        glBindTexture(GL_TEXTURE_2D, pageTextureID);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); // Dolny lewy
        glTexCoord2f(256.0f, 0.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); // Dolny prawy
        glTexCoord2f(256.0f, 256.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); // Gorny prawy
        glTexCoord2f(0.0f, 256.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); // Gorny lewy
        glEnd();

      /*  collectedPages++;
        newPage = true;
        map[x][y] = 0;*/

        break;
    default:
        break;




    }

    glDisable(GL_TEXTURE_2D);
}

void InGameState::renderWall(int x, int y) {

    glEnable(GL_TEXTURE_2D);

    if (map[x][y] == 1) {

        glBindTexture(GL_TEXTURE_2D, floorTextureID);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f + static_cast<float>(x), 10.0f, 1.0f + static_cast<float>(y)); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(.0f + static_cast<float>(x), 10.0f, 1.0f + static_cast<float>(y)); 

        glTexCoord2f(1.0f, 0.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(.0f + static_cast<float>(x), 10.0f, .0f + static_cast<float>(y)); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f + static_cast<float>(x), 10.0f, .0f + static_cast<float>(y)); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); 

        glTexCoord2f(1.0f, 1.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y)); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(.0f + static_cast<float>(x), 10.0f, 1.0f + static_cast<float>(y)); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(.0f + static_cast<float>(x), 10.0f, .0f + static_cast<float>(y)); 

        glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, .0f + static_cast<float>(y)); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f + static_cast<float>(x), 10.0f, .0f + static_cast<float>(y)); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f + static_cast<float>(x), 10.0f, 1.0f + static_cast<float>(y)); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f + static_cast<float>(x), 0.0f, 1.0f + static_cast<float>(y));
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void InGameState::render(sf::RenderWindow& window) {

    updateCamera();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            renderFloor(i, j);
        }
    }

    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            renderWall(i, j);
        }
    }

    glFlush();
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