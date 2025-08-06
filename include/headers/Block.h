#pragma once

enum class BlockType
{
    Air = 0,
    Grass = 1,
    Sand = 2,
    Water = 3,
    Dirt = 4,
    Stone = 5,
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
