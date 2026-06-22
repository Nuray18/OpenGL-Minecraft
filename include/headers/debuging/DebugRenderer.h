#pragma once
#include "headers/physics/AABB.h"
#include "sdl/SDL_ttf.h"
#include "sdl/SDL.h"
#include "glad/glad.h"

#include <iostream>

class DebugRenderer
{
public:
    static void init();
    static void drawAABB(const AABB &box);

private:
    static unsigned int VAO;
    static unsigned int VBO;
};