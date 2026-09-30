#pragma once

#include "block.hpp"
#include "mathutils.hpp"

class World {
public:
    static constexpr int SIZE_X = 32;
    static constexpr int SIZE_Y = 16;
    static constexpr int SIZE_Z = 32;

    World();

    bool inBounds(int x, int y, int z) const {
        return x >= 0 && x < SIZE_X && y >= 0 && y < SIZE_Y && z >= 0 && z < SIZE_Z;
    }

    BlockType getBlock(int x, int y, int z) const {
        return inBounds(x, y, z) ? blocks[x][y][z] : BlockType::AIR;
    }

    bool setBlock(int x, int y, int z, BlockType type);

    bool isSolid(int x, int y, int z) const {
        return getBlockDef(getBlock(x, y, z)).isSolid;
    }

    bool isTransparent(int x, int y, int z) const {
        return getBlockDef(getBlock(x, y, z)).isTransparent;
    }

    RaycastResult raycast(Vec3 origin, Vec3 dir, float maxDist) const;

private:
    BlockType blocks[SIZE_X][SIZE_Y][SIZE_Z] {};
};