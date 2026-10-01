#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3_image/SDL_image.h>
#include <array>
#include <string_view>

enum class BlockType : uint8_t {
    AIR,
    GRASS,
    DIRT,
    STONE,
    COUNT
};

inline constexpr size_t BLOCK_TYPE_COUNT = static_cast<size_t>(BlockType::COUNT);

struct TileCoord {
    int x = 0;
    int y = 0;
};

struct CubeTextures {
    TileCoord top{};
    TileCoord side{};
    TileCoord bottom{};

    constexpr CubeTextures() = default;

    constexpr explicit CubeTextures(TileCoord all) : top(all), side(all), bottom(all) {}
    constexpr CubeTextures(TileCoord top, TileCoord side, TileCoord bottom) : top(top), side(side), bottom(bottom) {}
};

struct BlockDef {
    std::string_view id;
    bool isSolid;
    bool isTransparent;

    SDL_FColor base;
};

inline constexpr std::array<BlockDef, BLOCK_TYPE_COUNT> BLOCK_REGISTRY = {{
    {"air", false, true, {0, 0, 0, 0}},
    {"grass", true, false, {.34f, .73f, .21f, 1.0f}},
    {"dirt", true, false, {.45f, .3f, .18f, 1.0f}},
    {"stone", true, false, {.5f, .5f, .5f, 1.0f}}
}};

inline constexpr const BlockDef& getBlockDef(BlockType type) {
    return BLOCK_REGISTRY[static_cast<size_t>(type)];
}