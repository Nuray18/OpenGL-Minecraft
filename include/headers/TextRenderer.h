#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;
using namespace glm;

struct Character
{
    GLuint TextureID;
    ivec2 Size;
    ivec2 Bearing;
    GLuint Advance;
};

class TextRenderer
{
public:
    map<GLchar, Character> Characters;

    GLuint VAO, VBO;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    

    GLuint loadTextShaders(const char *vertexPath, const char *fragmentPath);
    TextRenderer(int screenWidth, int screenHeight);
    ~TextRenderer();

    void LoadText(const string &fontPath, int fontSize);
    void RenderText(const string &text, float x, float y, float scale, vec3 color);
};
