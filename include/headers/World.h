#pragma once
#include "Chunk.h"
#include "pair_hash.h"

#include <map>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <functional> // bu specialization yazmak icindir
#include <vector>
#include <immintrin.h> // SSE/AVX komutları için(SIMD)

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class World
{
public:
    World();
    ~World();
    void generateWorld(const vec3 &playerPosition, const mat4 &viewProjMatrix);
    void render(unsigned int shaderProgram, const mat4 &viewProjMatrix);
    void update(const vec3 &playerPosition, const mat4 &viewProjMatrix);
    // bool isChuckLoaded(int chunkX, int chunkY);
    void destroyChunk();
    void destroyBlock();
    pair<int, int> calculateChunkCoord(const vec3 &playerPosition);
    bool isAABBInFrustum(const glm::vec3 &min, const glm::vec3 &max, const glm::mat4 &viewProjMatrix);
    BlockType getBlockGlobal(int worldX, int y, int worldZ) const;
    Chunk *getChunk(int chunkX, int chunkZ);
    Chunk *getCacheChunk(int chunkX, int chunkZ);
    bool isChunkActive(int chunkX, int chunkZ);

private:
    // location //chunk  so when we say chunks[{int x, int y}] = chunk* mean we are pointing to the chunk that is in the location of x and z
    unordered_map<pair<int, int>, Chunk *, pair_hash> chunks;
    // means chunks[{x ,y}] -> chunk
    unordered_set<pair<int, int>, pair_hash> newVisibleChunks; // obje yok sadece kordinatlar

    unordered_map<pair<int, int>, Chunk *, pair_hash> chunkCache;
    // LRU sırasını tutan liste: en eski başta, en yeni sonda
    list<pair<int, int>> cacheOrder;
    unordered_map<pair<int, int>, list<pair<int, int>>::iterator, pair_hash> cacheMap;
};
