#include "headers/Chunk.h"
#include "headers/World.h"
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

void Chunk::render(unsigned int shaderProgram, int vertexSize, const World *world, bool debugOcclusion) // chunks bir vectirdur chunk pointeri saklayan vector
{
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int colorLoc = glGetUniformLocation(shaderProgram, "color");

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
                if (world->getBlockGlobal(worldX + 1, y, worldZ) == 0)
                    visible = true;
                if (world->getBlockGlobal(worldX - 1, y, worldZ) == 0)
                    visible = true;
                if (world->getBlockGlobal(worldX, y + 1, worldZ) == 0)
                    visible = true;
                if (world->getBlockGlobal(worldX, y - 1, worldZ) == 0)
                    visible = true;
                if (world->getBlockGlobal(worldX, y, worldZ + 1) == 0)
                    visible = true;
                if (world->getBlockGlobal(worldX, y, worldZ - 1) == 0)
                    visible = true;

                // ✨ Occlusion kontrolü
                if (!visible && !debugOcclusion)
                    continue; // sadece debug açık değilse gizli olanları çizme

                // block'u ciz
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldX, y, worldZ));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                if (visible)
                {
                    glUniform3f(colorLoc, -1.0f, -1.0f, -1.0f); // sadece texture kullan
                }
                else
                {
                    glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f); // görünmeyen blok = sarı
                }

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
