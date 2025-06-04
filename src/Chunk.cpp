#include "headers/Chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h> // OpenGL fonksiyonları için
#include <iostream>

using namespace std;

// extern GLuint VAO;
// extern GLuint shaderProgram;
// extern GLuint texture1;
// extern int vertexSize; // float sayısı, setupOpenGL'de ayarlanıyor

Chunk::Chunk(int chunkX, int chunkZ)
    : chunkX(chunkX), chunkZ(chunkZ)
{
    generate();
    cout << "Bu func calisti" << endl;
}

Chunk::~Chunk()
{
    // Kaynak temizleme işlemleri gerekirse buraya
}

void Chunk::generate()
{
    generateTerrain();
}

void Chunk::generateTerrain()
// burda biz hava ile diger bloklar arasinda ayrim yapiyoruz
{
    // Örnek: basit düz dünya
    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int z = 0; z < CHUNK_DEPTH; ++z)
        {

            int worldX = chunkX * CHUNK_WIDTH + x;
            int worldZ = chunkZ * CHUNK_DEPTH + z;

            // Noise for the world
            int height = (sin(worldX * 0.01) + cos(worldZ * 0.1)) * 4 + 8; // 4 ile 13 arasi bir deger.

            for (int y = 0; y < CHUNK_HEIGHT; ++y)
            {
                if (y <= height)
                    blocks[x][y][z] = 1; // Toprak/taş gibi bir şey
                else
                    blocks[x][y][z] = 0; // Hava
            }
        }
    }
}

void Chunk::update()
{
    // Chunk içeriğini güncelle (örneğin mesh oluşturma)
}

void Chunk::render(unsigned int shaderProgram, int vertexSize, const vector<Chunk *> &chunks)
{
    int modelLoc = glGetUniformLocation(shaderProgram, "model");

    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < CHUNK_DEPTH; ++z)
            {
                if (getBlock(x, y, z) == 0)
                    continue;

                int worldX = chunkX * CHUNK_WIDTH + x; // herhagi bir locationda hangi chunk olursa olsun sadece bir chunk odakli degil tum chunklar icin odakli sistem
                int worldZ = chunkZ * CHUNK_DEPTH + z;

                bool visible = false;
                if (getBlockGlobal(worldX + 1, y, worldZ, chunks) == 0)
                    visible = true;
                if (getBlockGlobal(worldX - 1, y, worldZ, chunks) == 0)
                    visible = true;
                if (getBlockGlobal(worldX, y + 1, worldZ, chunks) == 0)
                    visible = true;
                if (getBlockGlobal(worldX, y - 1, worldZ, chunks) == 0)
                    visible = true;
                if (getBlockGlobal(worldX, y, worldZ + 1, chunks) == 0)
                    visible = true;
                if (getBlockGlobal(worldX, y, worldZ - 1, chunks) == 0)
                    visible = true;

                if (!visible)
                    continue;

                // block'u ciz
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldX, y, worldZ));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexSize / 3));
            }
        }
    }
}

int Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH)
        return -1;
    return blocks[x][y][z];
}

void Chunk::setBlock(int x, int y, int z, int blockID)
{
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH)
        return;
    blocks[x][y][z] = blockID;
}

glm::ivec2 Chunk::getChunkPosition() const
{
    return glm::ivec2(chunkX, chunkZ);
}

int Chunk::randNoice(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int Chunk::getBlockGlobal(int worldX, int y, int worldZ, const vector<Chunk *> &chunks) const
{
    int targetChunkX = worldX / CHUNK_WIDTH;
    int targetChunkZ = worldZ / CHUNK_DEPTH;

    int localX = worldX % CHUNK_WIDTH;
    int localZ = worldZ % CHUNK_DEPTH;

    if (localX < 0)
        localX += CHUNK_WIDTH;

    if (localZ < 0)
        localZ += CHUNK_DEPTH;

    for (const Chunk *chunk : chunks)
    {
        ivec2 pos = chunk->getChunkPosition();
        if (pos.x == targetChunkX && pos.y == targetChunkZ)
        {
            return chunk->getBlock(localX, y, localZ);
        }
    }

    return 0; // komsu chunk yoksa hava say.
}
