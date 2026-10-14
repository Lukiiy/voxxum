#pragma once

#include "column.hpp"
#include <FastNoiseLite.h>
#include <algorithm>

class TerrainGen {
public:
    static void generateColumn(ChunkColumn& column, int colX, int colZ) {
        column.x = colX;
        column.z = colZ;

        for (int x = 0; x < 16; ++x) {
            for (int z = 0; z < 16; ++z) {
                int worldX = colX * 16 + x;
                int worldZ = colZ * 16 + z;
                int height = calculateHeight(worldX, worldZ);

                column.setBlock(x, 0, z, BlockType::BEDROCK);

                for (int y = 1; y < height - 4; ++y) column.setBlock(x, y, z, BlockType::STONE);
                for (int y = std::max(1, height - 4); y < height; ++y) column.setBlock(x, y, z, BlockType::DIRT);

                column.setBlock(x, height, z, BlockType::GRASS);
            }
        }
    }

    static void setSeed(int seed) {
        noise().SetSeed(seed);
    }

private:
    static FastNoiseLite& noise() {
        static FastNoiseLite noise = [] {
            FastNoiseLite it;

            it.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            it.SetFractalType(FastNoiseLite::FractalType_FBm);
            it.SetFractalOctaves(4);
            it.SetFrequency(.01f);

            return it;
        }();

        return noise;
    }

    static int calculateHeight(int x, int z) {
        float noi = noise().GetNoise(static_cast<float>(x), static_cast<float>(z));

        return static_cast<int>(64 + noi * 30.0f);
    }
};