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

void World::generateWorld(int radius)
{
    // burda sadece x ekseni ve z(derinlik) ekseni boyunca chunk olusturuyoruz yani y ekseninde chunklar olusturmuyoruz.
    for (int x = -radius; x <= radius; x++)
    {
        for (int z = -radius; z <= radius; z++)
        {
            if (chunks.find({x, z}) == chunks.end())
            {
                // create a chunk
                chunks[{x, z}] = new Chunk(x, z); // in here we create a chunk then we put it in the chunks map
            }
        }
    }
}

void World::update()
{
}

void World::render(unsigned int shaderProgram, int vertexSize)
{
    for (auto &pair : chunks)
    {
        pair.second->render(shaderProgram, vertexSize); // second -> Chunk*
    }
}