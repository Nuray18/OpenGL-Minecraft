#pragma once
#include "headers/Player.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glad/glad.h>
#include <sdl/SDL.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;
using namespace glm;

struct UVRange
{
    float uMin;
    float uMax;
    float vMin;
    float vMax;
};

enum class GameState
{
    PLAY,
    EXIT
};

class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    void init(const char *title, int x, int y, int w, int h, Uint32 flags);
    void handleEvents(SDL_Event &event);
    void gameLoop();
    void render();
    void setupOpenGL(); // OpenGL ayarlari
    GLuint loadShaders(const char *vertexPath, const char *fragmentPath);
    UVRange getUVRange(int rowSize, int colSize, int targetIndex);
    unsigned int loadTexture(const char *path);
    void checkShaderErrors();
    void checkErrors();
    void mouseCallback(SDL_Window *window, double xRel, double yRel); // Rel means Relative mostly used in fps games.
    void scrollCallback(SDL_Window *window, double xOffset, double yOffset);

    bool running() { return isRunning; }

    SDL_Window *window;
    SDL_GLContext glContext;

    int screenWidth;
    int screenHeight;

    GameState gameState;

    Player *player; // Player classina point eden player pointer

    glm::mat4 transform;
    glm::vec3 cubePositions[10];

    bool isRunning;

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    unsigned int texture1;

    unsigned int EBO;
    unsigned int VAO; // vertex array object
    unsigned int VBO; // vertex buffer object

    // define camera
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraSpeed;
    // To move camera with same fp in each comp
    float deltaTime; // time between current frame and last frame
    float lastFrame;
    // To move camera freely in the world.
    float lastX;
    float lastY;
    bool firstMouse;
    GLfloat fov;
    float yaw;
    float pitch;
};