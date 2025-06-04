#pragma once
#include "Chunk.h"
#include "ivec2_hash.h"
#include <map>
#include <unordered_set>
#include <functional> // bu specialization yazmak icindir
#include <vector>
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
    void render(unsigned int shaderProgram, int vertexSize, const vec3 &playerPosition, const mat4 &viewProjMatrix);
    void update(const vec3 &playerPosition, const mat4 &viewProjMatrix);
    bool isChuckLoaded(int chunkX, int chunkY);
    void destroyChunk();
    void destroyBlock();
    ivec2 calculateChunkCoord(const vec3 &playerPosition);
    bool isAABBInFrustum(const glm::vec3 &min, const glm::vec3 &max, const glm::mat4 &viewProjMatrix);

private:
    // location //chunk  so when we say chunks[{int x, int y}] = chunk* mean we are pointing to the chunk that is in the location of x and z
    map<pair<int, int>, Chunk *> chunks;
    // means chunks[{x ,y}] -> chunk
    unordered_set<ivec2> newVisibleChunks;
};
