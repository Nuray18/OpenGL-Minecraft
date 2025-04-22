#pragma once
#include "Chunk.h"
#include <map>

class World
{
public:
    World();
    void update();
    void render(GLuint shader);

    Block getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);

private:
    std::map<std::pair<int, int>, Chunk *> chunks;

    Chunk *getChunk(int chunkX, int chunkZ) const;
    void loadChunk(int chunkX, int chunkZ);
};
