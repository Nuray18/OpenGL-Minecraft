enum class BlockType
{
    Air,
    Grass,
    Dirt,
    Stone
};

struct Block
{
    BlockType type = BlockType::Air;

    Block() = default;
    Block(BlockType t) : type(t) {}

    bool isTransparent() const
    {
        return type == BlockType::Air;
    }

    bool isSolid() const
    {
        return type != BlockType::Air;
    }
};