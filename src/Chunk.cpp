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

void Chunk::render(unsigned int shaderProgram, int vertexSize)
{
    int modelLoc = glGetUniformLocation(shaderProgram, "model");

    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < CHUNK_DEPTH; ++z)
            {
                // block'u ciz
                if (blocks[x][y][z] != 0) // boş değilse
                {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkX * CHUNK_WIDTH + x, y, chunkZ * CHUNK_DEPTH + z));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexSize / 3));
                }
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
