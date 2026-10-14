#include "world.hpp"
#include "region.hpp"
#include "terrainGen.hpp"
#include <SDL3/SDL_log.h>
#include <cmath>
#include <unordered_set>

World::World(std::string path) : saveDir(std::move(path)) {
    std::error_code err;
    std::filesystem::create_directories(std::filesystem::path(saveDir) / "region", err);

    if (err) SDL_Log("Cannot create save dir %s: %s", saveDir.c_str(), err.message().c_str());
}

ChunkColumn& World::getOrLoadColumn(int colX, int colZ) {
    uint64_t key = getColumnKey(colX, colZ);
    auto it = loadedColumns.find(key);

    if (it != loadedColumns.end()) return it->second;

    ChunkColumn col;
    col.x = colX;
    col.z = colZ;

    bool loaded = RegionFile::loadColumn(saveDir + "/region", colX, colZ, col);
    if (!loaded) TerrainGen::generateColumn(col, colX, colZ);

    auto [insertedIt, success] = loadedColumns.emplace(key, std::move(col));
    return insertedIt->second;
}

BlockType World::getBlock(int x, int y, int z) {
    ChunkColumn& col = getOrLoadColumn(x >> 4, z >> 4);

    return col.getBlock(x & 15, y, z & 15);
}

bool World::setBlock(int x, int y, int z, BlockType type) {
    int colX = x >> 4;
    int colZ = z >> 4;
    ChunkColumn& col = getOrLoadColumn(colX, colZ);

    col.setBlock(x & 15, y, z & 15, type);

    dirtySect.insert(getColumnKey(colX, colZ));
    return true;
}

RaycastResult World::raycast(Vec3 source, Vec3 dir, float max) const {
    RaycastResult result;

    Vec3i pos = {
        static_cast<int>(std::floor(source.x)),
        static_cast<int>(std::floor(source.y)),
        static_cast<int>(std::floor(source.z))
    };

    Vec3 deltaDist = {
        std::abs(dir.x) < EPSILION ? INF : std::abs(1.0f / dir.x),
        std::abs(dir.y) < EPSILION ? INF : std::abs(1.0f / dir.y),
        std::abs(dir.z) < EPSILION ? INF : std::abs(1.0f / dir.z)
    };

    Vec3i step;
    Vec3 sideDist;

    if (dir.x < 0) {
        step.x = -1;
        sideDist.x = (source.x - pos.x) * deltaDist.x;
    } else {
        step.x = 1;
        sideDist.x = (pos.x + 1.0f - source.x) * deltaDist.x;
    }

    if (dir.y < 0) {
        step.y = -1;
        sideDist.y = (source.y - pos.y) * deltaDist.y;
    } else {
        step.y = 1;
        sideDist.y = (pos.y + 1.0f - source.y) * deltaDist.y;
    }

    if (dir.z < 0) {
        step.z = -1;
        sideDist.z = (source.z - pos.z) * deltaDist.z;
    } else {
        step.z = 1;
        sideDist.z = (pos.z + 1.0f - source.z) * deltaDist.z;
    }

    Vec3i lastPos = pos;
    float dist = 0.0f;

    while (dist < max) {
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            dist = sideDist.x;
            sideDist.x += deltaDist.x;
            lastPos = pos;
            pos.x += step.x;
        } else if (sideDist.y < sideDist.z) {
            dist = sideDist.y;
            sideDist.y += deltaDist.y;
            lastPos = pos;
            pos.y += step.y;
        } else {
            dist = sideDist.z;
            sideDist.z += deltaDist.z;
            lastPos = pos;
            pos.z += step.z;
        }

        if (isSolid(pos.x, pos.y, pos.z)) {
            result.hit = true;
            result.blockPos = pos;
            result.placePos = lastPos;

            return result;
        }
    }

    return result;
}