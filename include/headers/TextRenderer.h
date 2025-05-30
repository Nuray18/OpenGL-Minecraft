#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "headers/stb_easy_font.h"

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
