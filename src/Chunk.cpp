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
    : chunkX(chunkX), chunkZ(chunkZ), vertexSize(0)
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
    generateMesh();
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

void Chunk::generateMesh()
{
    vector<float> vertices;

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_DEPTH; z++)
            {
                int block = getBlock(x, y, z);
                if (block == 0) // hava ise skip
                    continue;

                bool topVisible = (y + 1 >= CHUNK_HEIGHT) || (getBlock(x, y + 1, z) == 0);
                bool bottomVisible = (y - 1 < 0) || (getBlock(x, y - 1, z) == 0);
                bool leftVisible = (x - 1 < 0) || (getBlock(x - 1, y, z) == 0);
                bool rightVisible = (x + 1 >= CHUNK_WIDTH) || (getBlock(x + 1, y, z) == 0);
                bool frontVisible = (z - 1 < 0) || (getBlock(x, y, z - 1) == 0);
                bool backVisible = (z + 1 >= CHUNK_DEPTH) || (getBlock(x, y, z + 1) == 0);

                bool isCompletelyHidden =
                    !topVisible && !bottomVisible &&
                    !leftVisible && !rightVisible &&
                    !frontVisible && !backVisible;

                // Her yüz için UV'leri al
                UVRange BotUV = getUVRange(3, 1, 0);
                UVRange MidUV = getUVRange(3, 1, 1);
                UVRange topUV = getUVRange(3, 1, 2);

                float debugFlag = isCompletelyHidden ? 1.0f : 0.0f;

                if (frontVisible)
                {
                    float frontFace[] = {
                        x + -0.5f, y + -0.5f, z + -0.5f, MidUV.uMin, MidUV.vMin, debugFlag, // 0
                        x + 0.5f, y + -0.5f, z + -0.5f, MidUV.uMax, MidUV.vMin, debugFlag,  // 1
                        x + 0.5f, y + 0.5f, z + -0.5f, MidUV.uMax, MidUV.vMax, debugFlag,   // 2
                        x + 0.5f, y + 0.5f, z + -0.5f, MidUV.uMax, MidUV.vMax, debugFlag,   // 3
                        x + -0.5f, y + 0.5f, z + -0.5f, MidUV.uMin, MidUV.vMax, debugFlag,  // 4
                        x + -0.5f, y + -0.5f, z + -0.5f, MidUV.uMin, MidUV.vMin, debugFlag, // 5
                    };
                    vertices.insert(vertices.end(), std::begin(frontFace), std::end(frontFace));
                }

                if (backVisible)
                {
                    float backFace[] = {
                        x + -0.5f, y + -0.5f, z + 0.5f, MidUV.uMin, MidUV.vMin, debugFlag, // 6
                        x + 0.5f, y + -0.5f, z + 0.5f, MidUV.uMax, MidUV.vMin, debugFlag,  // 7
                        x + 0.5f, y + 0.5f, z + 0.5f, MidUV.uMax, MidUV.vMax, debugFlag,   // 8
                        x + 0.5f, y + 0.5f, z + 0.5f, MidUV.uMax, MidUV.vMax, debugFlag,   // 9
                        x + -0.5f, y + 0.5f, z + 0.5f, MidUV.uMin, MidUV.vMax, debugFlag,  // 10
                        x + -0.5f, y + -0.5f, z + 0.5f, MidUV.uMin, MidUV.vMin, debugFlag, // 11
                    };
                    vertices.insert(vertices.end(), std::begin(backFace), std::end(backFace));
                }

                if (leftVisible)
                {
                    float leftFace[] = {
                        x + -0.5f, y + 0.5f, z + 0.5f, MidUV.uMin, MidUV.vMax, debugFlag,   // 12
                        x + -0.5f, y + 0.5f, z + -0.5f, MidUV.uMax, MidUV.vMax, debugFlag,  // 13
                        x + -0.5f, y + -0.5f, z + -0.5f, MidUV.uMax, MidUV.vMin, debugFlag, // 14
                        x + -0.5f, y + -0.5f, z + -0.5f, MidUV.uMax, MidUV.vMin, debugFlag, // 15
                        x + -0.5f, y + -0.5f, z + 0.5f, MidUV.uMin, MidUV.vMin, debugFlag,  // 16
                        x + -0.5f, y + 0.5f, z + 0.5f, MidUV.uMin, MidUV.vMax, debugFlag,   // 17
                    };
                    vertices.insert(vertices.end(), std::begin(leftFace), std::end(leftFace));
                }

                if (rightVisible)
                {
                    float rightFace[] = {
                        x + 0.5f, y + 0.5f, z + 0.5f, MidUV.uMin, MidUV.vMax, debugFlag,   // 18
                        x + 0.5f, y + 0.5f, z + -0.5f, MidUV.uMax, MidUV.vMax, debugFlag,  // 19
                        x + 0.5f, y + -0.5f, z + -0.5f, MidUV.uMax, MidUV.vMin, debugFlag, // 20
                        x + 0.5f, y + -0.5f, z + -0.5f, MidUV.uMax, MidUV.vMin, debugFlag, // 21
                        x + 0.5f, y + -0.5f, z + 0.5f, MidUV.uMin, MidUV.vMin, debugFlag,  // 22
                        x + 0.5f, y + 0.5f, z + 0.5f, MidUV.uMin, MidUV.vMax, debugFlag,   // 23
                    };
                    vertices.insert(vertices.end(), std::begin(rightFace), std::end(rightFace));
                }

                if (bottomVisible)
                {
                    float bottomFace[] = {
                        x + -0.5f, y + -0.5f, z + -0.5f, BotUV.uMin, BotUV.vMax, debugFlag, // 24
                        x + 0.5f, y + -0.5f, z + -0.5f, BotUV.uMax, BotUV.vMax, debugFlag,  // 25
                        x + 0.5f, y + -0.5f, z + 0.5f, BotUV.uMax, BotUV.vMin, debugFlag,   // 26
                        x + 0.5f, y + -0.5f, z + 0.5f, BotUV.uMax, BotUV.vMin, debugFlag,   // 27
                        x + -0.5f, y + -0.5f, z + 0.5f, BotUV.uMin, BotUV.vMin, debugFlag,  // 28
                        x + -0.5f, y + -0.5f, z + -0.5f, BotUV.uMin, BotUV.vMax, debugFlag, // 29
                    };
                    vertices.insert(vertices.end(), std::begin(bottomFace), std::end(bottomFace));
                }

                if (topVisible)
                {
                    float topFace[] = {
                        x + -0.5f, y + 0.5f, z + -0.5f, topUV.uMin, topUV.vMax, debugFlag, // 30
                        x + 0.5f, y + 0.5f, z + -0.5f, topUV.uMax, topUV.vMax, debugFlag,  // 31
                        x + 0.5f, y + 0.5f, z + 0.5f, topUV.uMax, topUV.vMin, debugFlag,   // 32
                        x + 0.5f, y + 0.5f, z + 0.5f, topUV.uMax, topUV.vMin, debugFlag,   // 33
                        x + -0.5f, y + 0.5f, z + 0.5f, topUV.uMin, topUV.vMin, debugFlag,  // 34
                        x + -0.5f, y + 0.5f, z + -0.5f, topUV.uMin, topUV.vMax, debugFlag  // 35
                    };
                    vertices.insert(vertices.end(), std::begin(topFace), std::end(topFace));
                }
            }
        }
    }

    if (vertices.empty())
    {
        vertexSize = 0;
        return;
    }

    vertexSize = vertices.size() / 6; // Her vertex 6 float (x,y,z,u,v, debugFlag)

    // enable it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // upload the vertex data
    glGenBuffers(1, &VBO);              // create
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // open
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // upload the vertex attibute burda VBO ya vertex shader icin vertices ve indices verilerinin nasil kullanilacagini anlatir yani ilk deger iknci deger ne yapar gibi felan
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // fonksiyonu, VBO içindeki verinin nasıl işleneceğini OpenGL'e bildirir.
    glEnableVertexAttribArray(0);

    // texture UV attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // debug flag
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //  unenable it
    glBindVertexArray(0);
}

void Chunk::update()
{
    // Chunk içeriğini güncelle (örneğin mesh oluşturma)
}

void Chunk::render(unsigned int shaderProgram, const World *world) // chunks bir vectordur chunk pointeri saklayan vector
{
    if (vertexSize == 0)
        return; // Çizilecek vertex yok

    glUseProgram(shaderProgram);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkX * CHUNK_WIDTH, 0, chunkZ * CHUNK_DEPTH));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexSize);
    glBindVertexArray(0);
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

UVRange Chunk::getUVRange(int rowSize, int colSize, int targetIndex)
// in my texture atlas height is 48, width is 16. now totalRows means how many texture block we have in texture atlas, and rowIndex means which block we want to have with the index number of it.
{
    int currRow = targetIndex / colSize;
    int currCol = targetIndex % colSize;

    float uMin = 1.0f / colSize * currCol;
    float uMax = 1.0f / colSize + uMin;
    float vMin = 1.0f / rowSize * currRow;
    float vMax = 1.0f / rowSize + vMin;

    return {uMin, uMax, vMin, vMax};
}
