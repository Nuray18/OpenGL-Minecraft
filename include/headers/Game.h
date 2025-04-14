#pragma once
#include "headers/Player.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <sdl/SDL.h>
#include <fstream>
#include <sstream>

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
    void render(float deltaTime);
    void setupOpenGL(); // OpenGL ayarlari
    GLuint loadShaders(const char *vertexPath, const char *fragmentPath);
    UVRange GetUVRange(int rowSize, int colSize, int targetIndex);
    unsigned int LoadTexture(const char *path);
    void CheckShaderErrors();
    void CheckErrors();

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
};