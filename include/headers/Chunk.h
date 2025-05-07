#include <vector>
#include <glm/glm.hpp>

using namespace glm;

class Chunk
{
public:
    static const int CHUNK_WIDTH = 32;
    static const int CHUNK_HEIGHT = 32;
    static const int CHUNK_DEPTH = 32;

    int chunkX, chunkZ; // Chunk pozisyonu (dünya koordinatında)

    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    // private olan generateTerrain funcunu cagirir.
    void generate();                                         // Chunk verisini üretir
    void update();                                           // Chunk güncellemesi (mesela mesh yeniden oluşturma)
    void render(unsigned int shaderProgram, int vertexSize); // Chunk'ı çizdirir

    int getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, int blockID);

    ivec2 getChunkPosition() const;

    int randNoice(int min, int max);

private:
    int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH]; // 3D blok dizisi

    void generateTerrain(); // Yüzey şekli üretimi (Perlin vs. ekleyebilirsin)
};
