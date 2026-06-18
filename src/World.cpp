#include "headers/World.h"
#include "headers/Chunk.h"

// Bu kodlarda iki tane culling kullandim:
// 1. Frustum culling -> Bu kamerada gorunmeyen chunk'lari kontrol eder yani chunklar ile calisir.
// 2. Occulusing culling -> Bu kamera da gorunen chunkin icindeki gorunmeyen blocklari kontrol eder.

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

struct FrustumPlane
{
    vec3 normal;
    float distance;
};

// Kamera matrisinden 6 düzlemi çıkarır (görüş alanı tanımı)
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

// kamera gorus acisinda olan veya olmayan chunk lari kontrol eder eger disinda ise cizmez. (SIMD) AVX ile calisir. To calculate 8 floats at the same time
// Mathematics of planes and AABBs
// SIMD optimization with AVX
// Early-out logic (returns false immediately if outside)
bool World::isAABBInFrustum(const glm::vec3 &min, const glm::vec3 &max, const glm::mat4 &viewProjMatrix)
{
    auto planes = extractFrustumPlanes(viewProjMatrix);

    for (const auto &plane : planes)
    {
        // AVX ile plane normal değerlerini yükle (x, y, z)
        __m256 normal = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, plane.normal.z, plane.normal.y, plane.normal.x, 0.0f);

        // AVX ile p vektörünü yükle (pozitif/negatif yön seçimi)
        __m256 p = _mm256_set_ps(
            0.0f, 0.0f, 0.0f, 0.0f,
            (plane.normal.z >= 0 ? max.z : min.z),
            (plane.normal.y >= 0 ? max.y : min.y),
            (plane.normal.x >= 0 ? max.x : min.x),
            0.0f);

        // Dot product (x, y, z) paralel çarpma
        __m256 mul = _mm256_mul_ps(normal, p);

        // Dot değerlerini yatay toplama (x+y+z)
        __m128 hi = _mm256_extractf128_ps(mul, 1);
        __m128 lo = _mm256_castps256_ps128(mul);
        __m128 sum = _mm_add_ps(lo, hi);
        sum = _mm_hadd_ps(sum, sum);
        sum = _mm_hadd_ps(sum, sum);

        float dot = _mm_cvtss_f32(sum) + plane.distance;

        // Eğer chunk frustum dışındaysa çizme
        // Is this point on the visible side of the plane?
        if (dot < 0.0f)
            return false;
    }
    return true;
}

void World::generateWorld(const vec3 &playerPosition, const mat4 &viewProjMatrix)
{
    pair<int, int> playerChunkCoord = calculateChunkCoord(playerPosition);
    // Yeni frame için "görünür chunk" listesini sıfırlarız.
    newVisibleChunks.clear();

    // burda sadece x ekseni ve z(derinlik) ekseni boyunca chunk olusturuyoruz yani y ekseninde chunklar olusturmuyoruz.
    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++) // x ekseninde chunklar
    {
        for (int dz = -RENDER_DISTANCE; dz <= RENDER_DISTANCE; dz++) // y ekseninde chunklar
        {
            pair<int, int> chunkCoord(playerChunkCoord.first + dx, playerChunkCoord.second + dz);

            // Bu min ve max, chunk’ın Axis-Aligned Bounding Box (AABB) sınırlarını belirler.
            vec3 min = vec3(chunkCoord.first * CHUNK_WIDTH, 0, chunkCoord.second * CHUNK_DEPTH);
            vec3 max = min + vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH);

            newVisibleChunks.insert(chunkCoord);

            // eger chunk icerisinde yok ise
            if (chunks.find(chunkCoord) == chunks.end())
            {
                // Cache'te varsa cache'ten al
                auto cacheIt = chunkCache.find(chunkCoord);
                // Burda cache icinde var ise RAM'a gitme ihtiyaci kalmaz
                if (cacheIt != chunkCache.end())
                {
                    // chunka ekle
                    chunks[chunkCoord] = cacheIt->second;

                    // ekledikten sonra LRU'dan sil
                    cacheOrder.erase(cacheMap[chunkCoord]);
                    cacheMap.erase(chunkCoord);
                    chunkCache.erase(cacheIt);
                }
                else
                // burda RAM'a gideriz ve yeni olustururuz
                {
                    // Ne aktifte ne de cache'te varsa yeni oluştur
                    chunks[chunkCoord] = new Chunk(chunkCoord.first, chunkCoord.second, this);
                }
            }
            // ✅ Böylece aynı chunk birkaç saniye içinde tekrar görünürse, yeniden generate etmek yerine cache’ten alıyoruz.
        }
    }
}

// 👁️ Görünmeyen chunk’ları tespit edip onları aktif chunk listesinden çıkarıp cache’e atmaktır.
void World::destroyChunk()
{
    // it->first means x and y {x, y} like this AND it->second means Chunk* WHEN we say it->first->second we mean y value.
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        pair<int, int> chunkCoord = it->first;

        // eger chunk kordinatlari gorunen chunk listesinde degil ise cacheye ekle
        if (newVisibleChunks.find(chunkCoord) == newVisibleChunks.end())
        {
            // Cache'ye chunk ekle
            chunkCache[chunkCoord] = it->second;       // chunki cache icerisinde x ve y yerine koyuyoruz map ozelligi
            cacheOrder.push_back(chunkCoord);          // sona ekle, bu ekleme ile gelen kordinatlardaki chunkin sirasini korumus oluyoruz.
            cacheMap[chunkCoord] = --cacheOrder.end(); // iterator'u sakla

            // Eğer kapasiteyi aştıysa en eskiyi sil (LRU)
            if (cacheOrder.size() > CHUNK_CACHE_SIZE)
            {
                pair<int, int> oldest = cacheOrder.front();
                cacheOrder.pop_front();    // listeden çıkar
                delete chunkCache[oldest]; // belleği temizle // RAM den de cikar
                chunkCache.erase(oldest);  // map'ten çıkar // sonra cacheden de sil
                cacheMap.erase(oldest);    // iterator'u da sil
            }
            // Aktif chunk listesinden çıkar ancak memory'de duruyor ordan silmedik cunku cacheye ekledik.
            it = chunks.erase(it); // chunks tanda siliyoruz cache'ye ekledikten sonra ve it de diger elemana geciyor
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

// bu bize playerin durdugu chunk'i verir yani bulundugu chunk kordinatini
pair<int, int> World::calculateChunkCoord(const vec3 &playerPosition)
{
    int chunkX = floor(playerPosition.x / CHUNK_SIZE);
    int chunkZ = floor(playerPosition.z / CHUNK_SIZE);
    return pair<int, int>(chunkX, chunkZ);
}

// yeni chunklar olusturur.
// mantiksal dunya (logic, pozisyon, veri)
void World::update(const vec3 &playerPosition, const mat4 &viewProjMatrix)
{
    generateWorld(playerPosition, viewProjMatrix); // gorunenleri belirle

    destroyChunk(); // belirlenenler disindakileri sil.

    destroyBlock();
}

// gorsel dunya(cizim, ekran), var olan chunklari gpu'ya cizer, bu function sadece gorunen bulunduran chunksi cizer.
void World::render(unsigned int shaderProgram, const mat4 &viewProjMatrix)
{
    for (auto &pair : chunks)
    {
        Chunk *chunk = pair.second;

        vec3 min = vec3(chunk->chunkX * CHUNK_WIDTH, 0, chunk->chunkZ * CHUNK_DEPTH);
        vec3 max = min + vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_DEPTH);

        // gorunmuyor ise cizme
        if (!isAABBInFrustum(min, max, viewProjMatrix))
            continue;

        // ciz
        chunk->render(shaderProgram);
    }
}

// bu ise chunkin kendisine point eden pointer donduruyor yani direk chunki aliyoruz.
// ve kontrol ile bize chunk var ise chunki dondurur chunk memoryde hic yok ise null dondurur.
// iki sekilde kontrol edilir ilk once chunks in icinde var mi ona bakilir eger yok ise ikinci olarak cachenin icinde var mi ona bakilir eger yok ise null doner.
Chunk *World::getChunk(int chunkX, int chunkZ)
{
    auto it = chunks.find({chunkX, chunkZ});
    if (it != chunks.end())
    {
        cout << "chunks icinde var" << endl;
        return it->second; // Chunk pointer döndür
    }

    auto cacheIt = chunkCache.find({chunkX, chunkZ});
    if (cacheIt != chunkCache.end())
    {
        cout << "Cache icinde var" << endl;
        return cacheIt->second;
    }

    cout << "Null donderildi bu chunk hic yok" << endl;
    return nullptr; // Chunk yoksa null döndür
}

// Direkt block döndürüyor.
BlockType World::getBlockGlobal(int worldX, int y, int worldZ) const
{
    int chunkX = floor((float)worldX / CHUNK_WIDTH);
    int chunkZ = floor((float)worldZ / CHUNK_DEPTH);

    int localX = worldX % CHUNK_WIDTH;
    int localZ = worldZ % CHUNK_DEPTH;

    if (localX < 0)
        localX += CHUNK_WIDTH;
    if (localZ < 0)
        localZ += CHUNK_DEPTH;

    auto it = chunks.find({chunkX, chunkZ});
    if (it != chunks.end())
    {
        // chunkin icindeki istenilern blocku alir
        return it->second->getBlock(localX, y, localZ);
    }

    return BlockType::Air; // chunk yoksa hava kabul et.
}

bool World::isChunkActive(int chunkX, int chunkZ)
{
    return chunks.find({chunkX, chunkZ}) != chunks.end();
}