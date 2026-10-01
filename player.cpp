#include "player.hpp"
#include "world.hpp"
#include <algorithm>
#include <cmath>

Vec3 Player::getForwardVector() const {
    float rotY = yaw * DEG2RAD;
    float rotPitch = pitch * DEG2RAD;

    return {
        std::cos(rotPitch) * std::cos(rotY),
        std::sin(rotPitch),
        std::cos(rotPitch) * std::sin(rotY)
    };
}

Vec3 Player::getRightVector() const {
    float rotY = (yaw + 90.0f) * DEG2RAD;

    return {
        std::cos(rotY),
        0.0f,
        std::sin(rotY)
    };
}

AABB Player::getAABB(Vec3 pos) const {
    float halfW = width / 2.0f;

    return AABB{
        { pos.x - halfW, pos.y, pos.z - halfW },
        { pos.x + halfW, pos.y + height, pos.z + halfW }
    };
}

void Player::handleMouseLook(float dx, float dy, float sensitivity) {
    yaw += dx * sensitivity;
    pitch = std::clamp(pitch - dy * sensitivity, -89.0f, 89.0f);
}

void Player::updateInputs(const bool* keys, bool jumpPressed) {
    float rotY = yaw * DEG2RAD;
    Vec3 forward = { std::cos(rotY), 0.0f, std::sin(rotY) };
    Vec3 right = getRightVector();
    Vec3 moveDir;

    if (keys[SDL_SCANCODE_W]) moveDir = moveDir + forward;
    if (keys[SDL_SCANCODE_S]) moveDir = moveDir - forward;
    if (keys[SDL_SCANCODE_D]) moveDir = moveDir + right;
    if (keys[SDL_SCANCODE_A]) moveDir = moveDir - right;

    float len = std::sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z);
    if (len > ERRMARGIN) {
        moveDir.x /= len;
        moveDir.z /= len;
    }

    vel.x = moveDir.x * SPEED;
    vel.z = moveDir.z * SPEED;

    if (jumpPressed && grounded) {
        vel.y = JUMP;
        grounded = false;
    }
}

void Player::updatePhysics(float dt, const World& world) {
    dt = std::min(dt, .05f);

    vel.y -= GRAVITY * dt;

    pos.x += vel.x * dt;
    resolveCollisions(world, Axis::X);

    pos.y += vel.y * dt;
    resolveCollisions(world, Axis::Y);

    pos.z += vel.z * dt;
    resolveCollisions(world, Axis::Z);

    if (pos.y < -30.0f) {
        pos = {16.0f, 10.0f, 16.0f};
        vel = {};
    }
}

void Player::resolveCollisions(const World& world, Axis axis) {
    if (axis == Axis::Y) grounded = false;

    AABB box = getAABB(pos);
    int minX = static_cast<int>(std::floor(box.min.x)), maxX = static_cast<int>(std::floor(box.max.x));
    int minY = static_cast<int>(std::floor(box.min.y)), maxY = static_cast<int>(std::floor(box.max.y));
    int minZ = static_cast<int>(std::floor(box.min.z)), maxZ = static_cast<int>(std::floor(box.max.z));
    float halfW = width / 2.0f;

    for (int x = minX; x <= maxX; ++x)
    for (int y = minY; y <= maxY; ++y)
    for (int z = minZ; z <= maxZ; ++z) {
        if (!world.isSolid(x, y, z)) continue;

        switch (axis) {
            case Axis::X:
                if (vel.x > 0) pos.x = x - halfW - ERRMARGIN; else if (vel.x < 0) pos.x = x + 1.0f + halfW + ERRMARGIN;

                vel.x = 0.0f;
                break;
            case Axis::Y:
                if (vel.y < 0) {
                    pos.y = y + 1.0f;
                    grounded = true;
                } else if (vel.y > 0) pos.y = y - height - ERRMARGIN;

                vel.y = 0.0f;
                break;
            case Axis::Z:
                if (vel.z > 0) pos.z = z - halfW - ERRMARGIN; else if (vel.z < 0) pos.z = z + 1.0f + halfW + ERRMARGIN;

                vel.z = 0.0f;
                break;
        }

        return;
    }
}