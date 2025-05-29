#include "headers/TextRenderer.h"
// #include "headers/ft2build.h"
// #include FT_FREETYPE_H
#include <iostream>

TextRenderer::TextRenderer(int screenWidth, int screenHeight)
{
    loadTextShaders("src/shaders/TextVertex.glsl", "src/shaders/TextFragment.glsl");
}

void TextRenderer::LoadText(const char *fontPath, int fontSize)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
    }
}

void TextRenderer::RenderText(const string &text, float x, float y, float scale, vec3 color)
{
}

GLuint TextRenderer::loadTextShaders(const char *vertexPath, const char *fragmentPath)
{
    // 1. Shader dosyalarını oku
    ifstream vertexFile(vertexPath);
    ifstream fragmentFile(fragmentPath);
    stringstream vertexStream, fragmentStream;

    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    string vertexCode = vertexStream.str();
    string fragmentCode = fragmentStream.str();

    vertexFile.close();
    fragmentFile.close();

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
