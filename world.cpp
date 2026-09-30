#include "block.hpp"

constexpr int SIZE_X = 32;
constexpr int SIZE_Y = 16;
constexpr int SIZE_Z = 32;

class World {
public:
    BlockType blocks[SIZE_X][SIZE_Y][SIZE_Z];

    World() {
        for (int x = 0; x < SIZE_X; ++x) {
            for (int y = 0; y < SIZE_Y; ++y) {
                for (int z = 0; z < SIZE_Z; ++z) {
                    blocks[x][y][z] = BlockType::AIR; // init
                }
            }
        }

        for (int x = 0; x < SIZE_X; ++x) { // terrain
            for (int z = 0; z < SIZE_Z; ++z) {
                blocks[x][0][z] = BlockType::STONE;
                blocks[x][1][z] = BlockType::DIRT;
                blocks[x][2][z] = BlockType::DIRT;
                blocks[x][3][z] = BlockType::GRASS;
            }
        }
    }

    bool inBounds(int x, int y, int z) const {
        return x >= 0 && x < SIZE_X && y >= 0 && y < SIZE_Y && z >= 0 && z < SIZE_Z;
    }

    BlockType getBlock(int x, int y, int z) const {
        if (!inBounds(x, y, z)) return BlockType::AIR;

        return blocks[x][y][z];
    }

    bool setBlock(int x, int y, int z, BlockType type) {
        if (!inBounds(x, y, z)) return false;

        blocks[x][y][z] = type;

        return true;
    }

    bool isSolid(int x, int y, int z) const {
        BlockType type = getBlock(x, y, z);

        return BLOCK_REGISTRY[static_cast<size_t>(type)].isSolid;
    }

    bool isTransparent(int x, int y, int z) const {
        BlockType type = getBlock(x, y, z);

        return BLOCK_REGISTRY[static_cast<size_t>(type)].isTransparent;
    }
};