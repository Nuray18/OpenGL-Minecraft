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
    Generate();
    cout << "Bu func calisti" << endl;
}

Chunk::~Chunk()
{
    // Kaynak temizleme işlemleri gerekirse buraya
}

void Chunk::Generate()
{
    GenerateTerrain();
}

void Chunk::GenerateTerrain()
{
    // Örnek: basit düz dünya
    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int z = 0; z < CHUNK_DEPTH; ++z)
        {
            for (int y = 0; y < CHUNK_HEIGHT; ++y)
            {
                if (y < 64)
                    blocks[x][y][z] = 1; // Toprak/taş gibi bir şey
                else
                    blocks[x][y][z] = 0; // Hava
            }
        }
    }
}

void Chunk::Update()
{
    // Chunk içeriğini güncelle (örneğin mesh oluşturma)
}

void Chunk::Render(unsigned int shaderProgram, int vertexSize)
{
    mat4 model = glm::translate(mat4(1.0f), vec3(chunkX * CHUNK_WIDTH, 0, chunkZ * CHUNK_DEPTH));

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexSize / 3));
}

int Chunk::GetBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH)
        return -1;
    return blocks[x][y][z];
}

void Chunk::SetBlock(int x, int y, int z, int blockID)
{
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH)
        return;
    blocks[x][y][z] = blockID;
}

glm::ivec2 Chunk::GetChunkPosition() const
{
    return glm::ivec2(chunkX, chunkZ);
}
