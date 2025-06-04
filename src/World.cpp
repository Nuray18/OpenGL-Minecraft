#include "headers/World.h"

// Bu kodlarda iki tane culling kullandim:
// 1. Frustum culling -> Bu kamerada gorunmeyen chunk'lari kontrol eder yani bir buyuk cunklar ile calisir.
// 2. Occulusing culling -> Bu kamera da gorunen chunkin icindeki gorunmeyen blocklari kontrol eder.

struct FrustumPlane
{
    vec3 normal;
    float distance;
};

vector<FrustumPlane> extractFrustumPlanes(const glm::mat4 &m)
{
    vector<FrustumPlane> planes(6);

    // Left
    planes[0].normal.x = m[0][3] + m[0][0];
    planes[0].normal.y = m[1][3] + m[1][0];
    planes[0].normal.z = m[2][3] + m[2][0];
    planes[0].distance = m[3][3] + m[3][0];

    // Right
    planes[1].normal.x = m[0][3] - m[0][0];
    planes[1].normal.y = m[1][3] - m[1][0];
    planes[1].normal.z = m[2][3] - m[2][0];
    planes[1].distance = m[3][3] - m[3][0];

    // Bottom
    planes[2].normal.x = m[0][3] + m[0][1];
    planes[2].normal.y = m[1][3] + m[1][1];
    planes[2].normal.z = m[2][3] + m[2][1];
    planes[2].distance = m[3][3] + m[3][1];

    // Top
    planes[3].normal.x = m[0][3] - m[0][1];
    planes[3].normal.y = m[1][3] - m[1][1];
    planes[3].normal.z = m[2][3] - m[2][1];
    planes[3].distance = m[3][3] - m[3][1];

    // Near
    planes[4].normal.x = m[0][3] + m[0][2];
    planes[4].normal.y = m[1][3] + m[1][2];
    planes[4].normal.z = m[2][3] + m[2][2];
    planes[4].distance = m[3][3] + m[3][2];

    // Far
    planes[5].normal.x = m[0][3] - m[0][2];
    planes[5].normal.y = m[1][3] - m[1][2];
    planes[5].normal.z = m[2][3] - m[2][2];
    planes[5].distance = m[3][3] - m[3][2];

    // Normalize
    for (auto &plane : planes)
    {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }

    return planes;
}

World::World()
{
}

World::~World()
{
    for (auto &pair : chunks)
    {
        delete pair.second;
    }
}

// sadece oyuncunun pozisyonuna yakin olan chunklari uret.
void World::generateWorld(const vec3 &playerPosition, const mat4 &viewProjMatrix)
{
    ivec2 playerChunkCoord = calculateChunkCoord(playerPosition);
    newVisibleChunks.clear();

    // burda sadece x ekseni ve z(derinlik) ekseni boyunca chunk olusturuyoruz yani y ekseninde chunklar olusturmuyoruz.
    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++) // x ekseninde chunklar
    {
        for (int dz = -RENDER_DISTANCE; dz <= RENDER_DISTANCE; dz++) // y ekseninde chunklar
        {
            ivec2 chunkCoord(playerChunkCoord.x + dx, playerChunkCoord.y + dz);

            vec3 min = vec3(chunkCoord.x * CHUNK_WIDTH, 0, chunkCoord.y * CHUNK_DEPTH);
            vec3 max = min + vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH);

            if (isAABBInFrustum(min, max, viewProjMatrix))
            {
                // ✅ Ekranda görünen yani daha onceden yuklenmis chunk’lar “korunur”
                newVisibleChunks.insert(chunkCoord);

                // ➕ Ekranda görünen ama daha önce yüklenmemiş yeni chunk’lar generate edilir.
                if (chunks.find({chunkCoord.x, chunkCoord.y}) == chunks.end())
                {
                    chunks[{chunkCoord.x, chunkCoord.y}] = new Chunk(chunkCoord.x, chunkCoord.y);
                }
            }

            // burda culling ile ekliyoruz blocklari
        }
    }
}

void World::destroyChunk()
{
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        ivec2 chunkCoord = ivec2(it->first.first, it->first.second);
        // ❌ Ekranda görünmeyen chunk’lar silinir
        if (newVisibleChunks.find(chunkCoord) == newVisibleChunks.end())
        {
            delete (*it).second; // bu kullanim eski kullanimdir ve pointer ile degerin kendisine ulasir. Yenisi soyledir it->second. yani su anlama gelir yildiz ile sunu deriz go to that adress and take the second value
            it = chunks.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void World::destroyBlock()
{
}

ivec2 World::calculateChunkCoord(const vec3 &playerPosition)
{
    int chunkX = floor(playerPosition.x / CHUNK_SIZE);
    int chunkZ = floor(playerPosition.z / CHUNK_SIZE);
    return ivec2(chunkX, chunkZ);
}

// yeni chunklar olusturur.
// mantiksal dunya (logic, pozisyon, veri)
void World::update(const vec3 &playerPosition, const mat4 &viewProjMatrix)
{
    // eger chunk zaten gorunuyorsa -> dokunulmaz
    // sadece yeni chunk gorunuyorsa -> yuklenir
    generateWorld(playerPosition, viewProjMatrix);
    // artik gorunmeyen chunk varsa silinir
    destroyChunk();
    // gorunmeyen block varsa silinir
    destroyBlock();
}

// var olan chunklari gpu'ya cizer
// gorsel dunya(cizim, ekran)
void World::render(unsigned int shaderProgram, int vertexSize, const vec3 &playerPosition, const mat4 &viewProjMatrix)
{
    int playerChunkX = floor(playerPosition.x / CHUNK_SIZE);
    int playerChunkZ = floor(playerPosition.z / CHUNK_SIZE);

    for (auto &pair : chunks)
    {
        Chunk *chunk = pair.second;

        vec3 min = vec3(chunk->chunkX * CHUNK_WIDTH, 0, chunk->chunkZ * CHUNK_DEPTH);
        vec3 max = min + vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH);

        if (!isAABBInFrustum(min, max, viewProjMatrix))
            continue;

        // burda culling ile ekliyoruz

        // 🚀🧠 burda vector chunk istiyor parametre ben ise unordered map kullaniyorum.
        // chunk->render(shaderProgram, vertexSize);
    }
}

bool World::isAABBInFrustum(const glm::vec3 &min, const glm::vec3 &max, const glm::mat4 &viewProjMatrix)
{
    auto planes = extractFrustumPlanes(viewProjMatrix);
    for (const auto &plane : planes)
    {
        glm::vec3 p = min;
        if (plane.normal.x >= 0)
            p.x = max.x;
        if (plane.normal.y >= 0)
            p.y = max.y;
        if (plane.normal.z >= 0)
            p.z = max.z;

        if (glm::dot(plane.normal, p) + plane.distance < 0)
            return false;
    }
    return true;
}
