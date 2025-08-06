#pragma once
#include "headers/Config.h"
#include "headers/Block.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
#include <iostream>

using namespace glm;

class World; // left here

struct UVRange
{
    float uMin;
    float uMax;
    float vMin;
    float vMax;
};

class Chunk
{
public:
    Chunk(int chunkX, int chunkZ);
    ~Chunk();
    // private olan generateTerrain funcunu cagirir.
    void generate(); // Chunk verisini üretir
    void update();   // Chunk güncellemesi (mesela mesh yeniden oluşturma)
    void render(unsigned int shaderProgram);
    void setBlock(int x, int y, int z, BlockType blockID);
    BlockType getBlock(int x, int y, int z) const;
    int randNoice(int min, int max);
    ivec2 getChunkPosition() const;
    UVRange getUVRange(int totalRows, int totalCols, BlockType blockType, int rowIndex);

    int chunkX, chunkZ; // Chunk pozisyonu (dünya koordinatında)
private:
    void generateTerrain(); // Yüzey şekli üretimi (Perlin vs. ekleyebilirsin)
    void generateMesh();

    BlockType blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH]; // 3D blok dizisi

    GLsizei vertexSize;

    unsigned int VAO; // vertex array object
    unsigned int VBO; // vertex buffer object

    World *world;
};
