#include "Block.h"

BlockInfo blockDatabase[] = {
    {BlockType::Air, false, {0, 0}}, // Air: boş blok
    {BlockType::Grass, true, {1, 0}},
    {BlockType::Dirt, true, {2, 0}},
    {BlockType::Stone, true, {3, 0}},
    {BlockType::Sand, true, {4, 0}},
    {BlockType::Water, false, {5, 0}},
};
