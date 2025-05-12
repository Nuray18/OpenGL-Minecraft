#include "headers/World.h"

World::World()
{
}

World::~World()
{
    for (auto &pair : chunks)
    {
        delete pair.second;
    }
}

void World::generateWorld(vec3 playerPosition)
{
    int playerChunkX = floor(playerPosition.x / CHUNK_SIZE);
    int playerChunkZ = floor(playerPosition.z / CHUNK_SIZE);

    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++)
    {
    }
}

void World::render(unsigned int shaderProgram, int vertexSize, vec3 playerPosition)
{
}

void World::update(vec3 playerPosition)
{
}

void World::destroyChunk(vec3 playerPosition)
{
}