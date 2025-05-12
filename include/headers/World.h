#pragma once
#include "Chunk.h"
#include <map>

using namespace std;

class World
{
public:
    World();
    ~World();
    void generateWorld(vec3 playerPosition);
    void render(unsigned int shaderProgram, int vertexSize, vec3 playerPosition);
    void update(vec3 playerPosition);
    bool isChuckLoaded(int chunkX, int chunkY);
    void destroyChunk(vec3 playerPosition);

private:
    // location //chunk  so when we say chunks[{int x, int y}] = chunk* mean we are pointing to the chunk that is in the location of x and z
    map<pair<int, int>, Chunk *> chunks;
    // means chunks[{x ,y}] -> chunk
};
