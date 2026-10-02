#pragma once

#include <SDL3/SDL_scancode.h>
#include "block.hpp"
#include "mathutils.hpp"

class World;

class Player { // TODO https://minecraft.wiki/w/Player
public:
    static constexpr float SPEED = 4.317f;
    static constexpr float JUMP = 8.4f;
    static constexpr float GRAVITY = 28.0f;

    Vec3 pos = { 16.0f, 6.0f, 16.0f };
    Vec3 vel = { 0.0f, 0.0f, 0.0f };

    float yaw = -90.0f;
    float pitch = 0.0f;
    float width = .5f;
    float height = 1.8f;
    float eyeHeight = 1.62f;
    bool grounded = false;

    BlockType selected = BlockType::GRASS;

    Vec3 getEyePosition() const {
        return { pos.x, pos.y + eyeHeight, pos.z };
    }

    Vec3 getForwardVector() const;
    Vec3 getRightVector() const;
    AABB getAABB(Vec3 pos) const;

    void handleMouseLook(float dx, float dy, float sensitivity = .15f);
    void updateInputs(const bool* keys, bool jumpPressed);
    void updatePhysics(float dt, const World& world);

private:
    void resolveCollisions(const World& world, Axis axis);
};