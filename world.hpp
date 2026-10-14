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
    World(std::string path);

    BlockType getBlock(int x, int y, int z);
    bool setBlock(int x, int y, int z, BlockType type);

    bool isSolid(int x, int y, int z) {
        return getBlockDef(getBlock(x, y, z)).isSolid;
    }

    bool isTransparent(int x, int y, int z) {
        return getBlockDef(getBlock(x, y, z)).isTransparent;
    }

    RaycastResult raycast(Vec3 origin, Vec3 dir, float maxDist);
    ChunkColumn& getOrLoadColumn(int colX, int colZ);

    const std::unordered_map<uint64_t, ChunkColumn>& getLoadedColumns() const {
        return loadedColumns;
    }

    void saveAll();

private:
    std::string saveDir;
    std::unordered_map<uint64_t, ChunkColumn> loadedColumns;
    std::unordered_set<uint64_t> dirtySect;

    inline uint64_t getColumnKey(int32_t x, int32_t z) const {
        return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(z);
    }
};