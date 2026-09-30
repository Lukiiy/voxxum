#include "world.hpp"
#include <cmath>

World::World() {
    for (int x = 0; x < SIZE_X; ++x) { // terrain
        for (int z = 0; z < SIZE_Z; ++z) {
            blocks[x][0][z] = BlockType::STONE;
            blocks[x][1][z] = BlockType::DIRT;
            blocks[x][2][z] = BlockType::DIRT;
            blocks[x][3][z] = BlockType::GRASS;
        }
    }
}

bool World::setBlock(int x, int y, int z, BlockType type) {
    if (!inBounds(x, y, z)) return false;

    blocks[x][y][z] = type;

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