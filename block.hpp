#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3_image/SDL_image.h>
#include <string>

enum class BlockType : uint8_t {
    AIR,
    GRASS,
    DIRT,
    STONE
};

inline constexpr size_t BLOCK_TYPE_COUNT = static_cast<size_t>(BlockType::STONE) + 1;

struct BlockDef {
    std::string id;
    bool isSolid;
    bool isTransparent;

    SDL_FColor base;
};

const BlockDef BLOCK_REGISTRY[BLOCK_TYPE_COUNT] = {
    {"air", false, true, {0, 0, 0, 0}},
    {"grass", true, false, {.34f, .73f, .21f, 1.0f}},
    {"dirt", true, false, {.45f, .3f, .18f, 1.0f}},
    {"stone", true, false, {.5f, .5f, .5f, 1.0f}}
};