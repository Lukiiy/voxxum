#pragma once

#include <SDL3/SDL.h>

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