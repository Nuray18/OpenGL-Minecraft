#pragma once
#include "Chunk.h"
#include <map>

using namespace std;

class World
{
public:
    World();
    ~World();
    void generateWorld(int radius);
    void render(unsigned int shaderProgram, int vertexSize);
    void update();

private:
    // location //chunk  so when we say chunks[{int x, int y}] = chunk* mean we are pointing to the chunk that is in the location of x and z
    map<pair<int, int>, Chunk *> chunks;
    // means chunks[{x ,y}] -> chunk
};
