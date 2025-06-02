#include "headers/World.h"

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
void World::generateWorld(vec3 playerPosition)
{
    int playerChunkX = floor(playerPosition.x / CHUNK_SIZE);
    int playerChunkZ = floor(playerPosition.z / CHUNK_SIZE);

    // burda sadece x ekseni ve z(derinlik) ekseni boyunca chunk olusturuyoruz yani y ekseninde chunklar olusturmuyoruz.
    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++)
    {
        for (int dz = -RENDER_DISTANCE; dz <= RENDER_DISTANCE; dz++)
        {
            int x = playerChunkX + dx;
            int z = playerChunkZ + dz;

            if (chunks.find({x, z}) == chunks.end())
            {
                // create a chunk
                chunks[{x, z}] = new Chunk(x, z); // in here we create a chunk then we put it in the chunks map
            }
        }
    }
}

void World::destroyChunk(vec3 playerPosition)
{
    int playerChunkX = floor(playerPosition.x / CHUNK_SIZE);
    int playerChunkZ = floor(playerPosition.z / CHUNK_SIZE);

    for (auto it = chunks.begin(); it != chunks.end();)
    {
        int chunkX = it->first.first;
        int chunkZ = it->first.second;

        if (abs(chunkX - playerChunkX) > RENDER_DISTANCE && abs(chunkZ - playerChunkZ) > RENDER_DISTANCE)
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

ivec2 World::calculateChunkCoord(const vec3 &playerPosition)
{
    int chunkX = floor(playerPosition.x / CHUNK_SIZE);
    int chunkZ = floor(playerPosition.z / CHUNK_SIZE);
    return ivec2(chunkX, chunkZ);
}

// yeni chunklar olusturur.
// mantiksal dunya (logic, pozisyon, veri)
void World::update(vec3 playerPosition)
{
    generateWorld(playerPosition);
    destroyChunk(playerPosition);
}

// var olan chunklari gpu'ya cizer
// gorsel dunya(cizim, ekran)
void World::render(unsigned int shaderProgram, int vertexSize, vec3 playerPosition, mat4 viewProjMatrix)
{
    int playerChunkX = floor(playerPosition.x / CHUNK_SIZE);
    int playerChunkZ = floor(playerPosition.z / CHUNK_SIZE);

    for (auto &pair : chunks)
    {
        Chunk *chunk = pair.second;

        // 1️⃣ Chunk’ın dünya pozisyonu (orta noktası)
        vec3 chunkCenter(
            chunk->chunkX * CHUNK_WIDTH + CHUNK_WIDTH / 2.0f,
            CHUNK_HEIGHT / 2.0f,
            chunk->chunkZ * CHUNK_DEPTH + CHUNK_DEPTH / 2.0f);

        // 2️⃣ Chunk’ın yarıçapı (diyelim en büyük boyut yarı uzunluğu)
        float radius = sqrt((CHUNK_WIDTH / 2.0f) * (CHUNK_WIDTH / 2.0f) +
                            (CHUNK_HEIGHT / 2.0f) * (CHUNK_HEIGHT / 2.0f) +
                            (CHUNK_DEPTH / 2.0f) * (CHUNK_DEPTH / 2.0f));

        // 3️⃣ Basit Sphere-Frustum test (AABB de yapabilirsin ama sphere basit)
        if (!isSphereInFrustum(viewProjMatrix, chunkCenter, radius))
        {
            // Bu chunk kamerada degil, cizme
            continue;
        }

        // 4️⃣ Chunk’ı çiz
        chunk->render(shaderProgram, vertexSize);
    }
}

bool World::isSphereInFrustum(const mat4 &viewProjMatrix, const vec3 &center, float radius)
{
    vec4 clipSpacePos = viewProjMatrix * vec4(center, 1.0f);
    float w = clipSpacePos.w;

    // NDC space: x, y, z ∈ [-w, w]
    if (clipSpacePos.x < -w - radius || clipSpacePos.x > w + radius)
        return false;
    if (clipSpacePos.y < -w - radius || clipSpacePos.y > w + radius)
        return false;
    if (clipSpacePos.z < -w - radius || clipSpacePos.z > w + radius)
        return false;

    return true;
}