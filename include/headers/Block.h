#pragma once

enum class BlockType
{
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    Sand = 4,
    Water = 5,
    // ... diğer bloklar
};

struct AtlasIndex
{
    int row;
    int col;
};

struct BlockInfo
{
    BlockType type;
    bool isSolid;       // Occlusion culling için
    AtlasIndex uvIndex; // Texture atlas konumu
};
