#pragma once

#include "block.hpp"
#include "column.hpp"
#include "mathutils.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

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
    std::string saveDir;
    std::unordered_map<uint64_t, ChunkColumn> loadedColumns;
    std::unordered_set<uint64_t> dirtySect;

    inline uint64_t getColumnKey(int32_t x, int32_t z) const {
        return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(z);
    }
};