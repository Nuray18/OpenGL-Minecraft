#pragma once
#include "headers/Player.h"
#include "headers/World.h"
#include "headers/TextRenderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "sdl/SDL_ttf.h"
#include "sdl/SDL.h"

#include "glad/glad.h"

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
    void mouseCallback(float xPos, float yPos);
    void scrollCallback(float yOffset);
    void processInput();

    bool running() { return isRunning; }

    GLsizei vertexSize;

    SDL_Window *window;
    SDL_GLContext glContext;

    int screenWidth;
    int screenHeight;

    GameState gameState;

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

    // To move camera with same fp in each comp
    float deltaTime; // time between current frame and last frame
    float lastFrame;

    Player player;
    World world;

    TextRenderer *textRenderer;

    float fps;
    float fpsTimer;
    int fpsFrameCount;
    string fpsText;

    ivec2 lastPlayerChunk;
};