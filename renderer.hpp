#pragma once

#include <SDL3/SDL_render.h>
#include "math.hpp"

class World;
class Player;

void renderWorld(SDL_Renderer* renderer, const World& world, const Player& player, int width, int height, const RaycastResult& rayTarget);