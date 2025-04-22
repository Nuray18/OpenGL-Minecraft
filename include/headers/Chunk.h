#include <vector>
#include <glm/glm.hpp>

using namespace glm;

class Chunk
{
public:
    static const int CHUNK_WIDTH = 16;
    static const int CHUNK_HEIGHT = 16;
    static const int CHUNK_DEPTH = 16;

    int chunkX, chunkZ; // Chunk pozisyonu (dünya koordinatında)

    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    void Generate();                                         // Chunk verisini üretir
    void Update();                                           // Chunk güncellemesi (mesela mesh yeniden oluşturma)
    void Render(unsigned int shaderProgram, int vertexSize); // Chunk'ı çizdirir

    int GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, int blockID);

    ivec2 GetChunkPosition() const;

private:
    int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH]; // 3D blok dizisi

    void GenerateTerrain(); // Yüzey şekli üretimi (Perlin vs. ekleyebilirsin)
};
