#include "world.hpp"

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