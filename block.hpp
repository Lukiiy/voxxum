#pragma once

#include "textureAtlas.hpp"
#include <SDL3/SDL_pixels.h>
#include <array>
#include <string_view>

enum class BlockType : uint8_t {
    AIR,
    GRASS,
    DIRT,
    STONE,
    COBBLESTONE,
    BEDROCK,
    COUNT
};

inline constexpr size_t BLOCK_TYPE_COUNT = static_cast<size_t>(BlockType::COUNT);

struct BlockDef {
    std::string_view id;
    bool isSolid;
    bool isTransparent;

    CubeTextures base;
};

inline constexpr std::array<BlockDef, BLOCK_TYPE_COUNT> BLOCK_REGISTRY = {{
    {"air", false, true, CubeTextures{}},
    {"grass", true, false, CubeTextures({1, 0}, {0, 0}, {2, 0})},
    {"dirt", true, false, CubeTextures({2, 0})},
    {"stone", true, false, CubeTextures({3, 0})},
    {"cobblestone", true, false, CubeTextures({0, 1})},
    {"bedrock", true, false, CubeTextures({1, 1})}
}};

inline constexpr const BlockDef& getBlockDef(BlockType type) {
    return BLOCK_REGISTRY[static_cast<size_t>(type)];
}