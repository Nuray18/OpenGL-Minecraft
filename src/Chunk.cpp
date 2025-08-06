#include "headers/Chunk.h"
#include "headers/World.h"

// eger yuzeye bitisik baska bir yuzey var ise o zaman current yuzey ve ona yapisik olan yuzey silinirler

using namespace std;

Chunk::Chunk(int chunkX, int chunkZ)
    : chunkX(chunkX), chunkZ(chunkZ), vertexSize(0)
{
    generate();
    cout << "Bu func calisti" << endl;
}

Chunk::~Chunk()
{
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

void Chunk::generate()
{
    generateTerrain();
    generateMesh();
}

void Chunk::generateTerrain()
// burda biz hava ile diger bloklar arasinda ayrim yapiyoruz
{
    float noiseScale = 0.005f; // noise frekansi (daha kucuk = daha genis tepeler)
    int heightScale = 20;      // dag yuksekligi
    int baseHeight = 8;        // deniz seviyesi

    // Örnek: basit düz dünya
    for (int x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (int z = 0; z < CHUNK_DEPTH; ++z)
        {

            int worldX = chunkX * CHUNK_WIDTH + x;
            int worldZ = chunkZ * CHUNK_DEPTH + z;

            // Simplex Noise (daha dogal arazi)
            float noiseValue = simplex(vec2(worldX * noiseScale, worldZ * noiseScale));

            // -1 ile 1 arasindaki degeri 0-1 arasina cevirir
            noiseValue = (noiseValue + 1.0f) * 0.5f;

            // baseHeight needs to be greater then height sometimes so that we can have water block.
            int height = baseHeight + static_cast<int>(floorf((noiseValue - 0.5f) * heightScale));

            for (int y = 0; y < CHUNK_HEIGHT; ++y)
            {
                if (y > height)
                {
                    // so the problem was that the baseHeight was always smaller than the height that is why the if's were imposible.
                    if (y <= baseHeight)
                        blocks[x][y][z] = BlockType::Water; // Deniz seviyesi altını suyla doldur
                    else
                        blocks[x][y][z] = BlockType::Air; // Deniz seviyesinin üstü hava
                }
                else if (y == height) // yüzey
                {
                    if (height <= baseHeight + 1)
                        blocks[x][y][z] = BlockType::Sand;
                    else
                        blocks[x][y][z] = BlockType::Grass;
                }
                else // y < height, yani alt katman
                {
                    blocks[x][y][z] = BlockType::Air;
                }
            }
        }
    }
}

void Chunk::generateMesh()
{
    vector<float> vertices;

    // Komşu chunk'ları al
    Chunk *leftChunk = world->getChunk(chunkX - 1, chunkZ);
    Chunk *rightChunk = world->getChunk(chunkX + 1, chunkZ);
    Chunk *frontChunk = world->getChunk(chunkX, chunkZ - 1);
    Chunk *backChunk = world->getChunk(chunkX, chunkZ + 1);

    int waterCount = 0;
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_DEPTH; z++)
            {
                if (blocks[x][y][z] == BlockType::Water)
                    waterCount++;

                BlockType block = getBlock(x, y, z);
                if (block == BlockType::Air) // hava ise skip
                    continue;

                // bunlar kendi chunklar icinden alabiliyoruz
                bool topVisible = (y + 1 >= CHUNK_HEIGHT) || (getBlock(x, y + 1, z) == BlockType::Air);
                bool bottomVisible = (y - 1 < 0) || (getBlock(x, y - 1, z) == BlockType::Air);

                // burda ise komsu chunkta olabilir left ve diger yan kisimlar
                bool leftVisible = (x - 1 < 0) || (getBlock(x - 1, y, z) == BlockType::Air);
                bool rightVisible = (x + 1 >= CHUNK_WIDTH) || (getBlock(x + 1, y, z) == BlockType::Air);
                bool frontVisible = (z - 1 < 0) || (getBlock(x, y, z - 1) == BlockType::Air);
                bool backVisible = (z + 1 >= CHUNK_DEPTH) || (getBlock(x, y, z + 1) == BlockType::Air);

                bool isCompletelyHidden =
                    !topVisible && !bottomVisible &&
                    !leftVisible && !rightVisible &&
                    !frontVisible && !backVisible;

                float debugFlag = isCompletelyHidden ? 1.0f : 0.0f;

                // Her yüz için UV'leri al
                UVRange BotUV = getUVRange(3, 3, block, 0);
                UVRange MidUV = getUVRange(3, 3, block, 1);
                UVRange topUV = getUVRange(3, 3, block, 2);

                // Checking for water first
                if (block == BlockType::Water)
                {
                    bool waterSurfaceVisible = (y + 1 >= CHUNK_HEIGHT) || (getBlock(x, y + 1, z) == BlockType::Air);

                    if (waterSurfaceVisible)
                    {
                        // sadece üst yüzey çiz (topFace)
                        float topFace[] = {
                            x + -0.5f, y + 0.5f, z + -0.5f, topUV.uMin, topUV.vMax, debugFlag,
                            x + 0.5f, y + 0.5f, z + -0.5f, topUV.uMax, topUV.vMax, debugFlag,
                            x + 0.5f, y + 0.5f, z + 0.5f, topUV.uMax, topUV.vMin, debugFlag,
                            x + 0.5f, y + 0.5f, z + 0.5f, topUV.uMax, topUV.vMin, debugFlag,
                            x + -0.5f, y + 0.5f, z + 0.5f, topUV.uMin, topUV.vMin, debugFlag,
                            x + -0.5f, y + 0.5f, z + -0.5f, topUV.uMin, topUV.vMax, debugFlag};
                        vertices.insert(vertices.end(), std::begin(topFace), std::end(topFace));
                    }

                    continue; // water için diğer yüzleri çizme
                }

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

    cout << "Chunk'da Water sayisi: " << waterCount << endl;

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

void Chunk::render(unsigned int shaderProgram)
{
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

BlockType Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH)
        return BlockType::Air; // gecersiz ise bos block olan Air gonderilir, func turu BlockType olmadan once -1 donuyordu int olarak
    return blocks[x][y][z];
}

void Chunk::setBlock(int x, int y, int z, BlockType blockID)
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

UVRange Chunk::getUVRange(int totalRows, int totalCols, BlockType blockType, int rowIndex)
// in my texture atlas height is 48, width is 16. now totalRows means how many texture block we have in texture atlas, and rowIndex means which block we want to have with the index number of it.
{
    int blockColumn = 0;
    switch (blockType)
    {
    case BlockType::Grass:
        blockColumn = 0;
        break;
    case BlockType::Sand:
        blockColumn = 1;
        break;
    case BlockType::Water:
        blockColumn = 2;
        break;
    default:
        blockColumn = 0;
        break;
    }

    float uMin = (1.0f / totalCols) * blockColumn;
    float uMax = uMin + (1.0f / totalCols);

    float vMin = (1.0f / totalRows) * rowIndex;
    float vMax = vMin + (1.0f / totalRows);

    return {uMin, uMax, vMin, vMax};
}
