#pragma once

#include <SDL3/SDL_render.h>
#include <string_view>
#include "mathutils.hpp"

class World;
class Player;

void renderWorld(SDL_Renderer* renderer, SDL_Texture* atlas, const World& world, const Player& player, int width, int height, const RaycastResult& rayTarget);
void renderGUI(SDL_Renderer* renderer, const Player& player, int width, int height, float fps, std::string_view block, const char* driver);