#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <string>

#include "block.hpp"
#include "mathutils.hpp"
#include "player.hpp"
#include "world.hpp"
#include "renderer.hpp"

namespace {
    constexpr float REACH = 4.0f;
    constexpr float BLOCKACT_INTERVAL = .2f;
    const std::string devLol = "by Lukiiy"; // lol yayyy
}

BlockType cycleSel(BlockType current, int add) {
    int n = static_cast<int>(BlockType::COUNT) - 1; // selectable blocks
    int i = static_cast<int>(current) - 1;

    i = ((i + add) % n + n) % n;

    return static_cast<BlockType>(i + 1);
}

AABB blockAABB(Vec3i pos) {
    return AABB{
        { static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z) },
        { static_cast<float>(pos.x + 1), static_cast<float>(pos.y + 1), static_cast<float>(pos.z + 1) }
    };
}

int main(int argc, char* argv[]) {
    const char* backend = "vulkan"; // preferred

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "--v") backend = "vulkan";
        else if (arg == "--o") backend = "opengl";
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, backend);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "error when initializing sdl: " << SDL_GetError() << std::endl;

        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Voxxum", 1280, 720, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        std::cerr << "error when init: " << SDL_GetError() << std::endl;
        SDL_Quit();

        return 1;
    }

    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowRelativeMouseMode(window, true);

    const char* driver = SDL_GetRendererName(renderer);
    World world = World();
    Player player = Player();
    auto ray = [&]() {
        return world.raycast(player.getEyePosition(), player.getForwardVector(), REACH);
    };

    float actTimer = 0.0f;
    Uint64 last = SDL_GetPerformanceCounter();
    const double freq = static_cast<double>(SDL_GetPerformanceFrequency());
    double fps = 0.0f;

    auto performBlockAction = [&](bool isLeft) {
        const RaycastResult result = ray();
        if (!result.hit) return;

        if (isLeft) {
            world.setBlock(result.blockPos.x, result.blockPos.y, result.blockPos.z, BlockType::AIR);
        } else {
            if (!player.getAABB(player.pos).intersects(blockAABB(result.placePos))) world.setBlock(result.placePos.x, result.placePos.y, result.placePos.z, player.selected);
        }
    };

    while (true) {
        Uint64 now = SDL_GetPerformanceCounter();
        float dt = static_cast<float>((now - last) / freq);

        last = now;
        if (dt > 0.0f) fps += (1.0f / dt - fps) * .1f;

        actTimer -= dt;

        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    goto quit;

                case SDL_EVENT_KEY_DOWN: {
                    if (e.key.repeat) break;

                    const SDL_Scancode key = e.key.scancode;
                    if (key == SDL_SCANCODE_ESCAPE) goto quit;

                    const int slot = key - SDL_SCANCODE_1 + 1;

                    if (slot >= 1 && slot < static_cast<int>(BlockType::COUNT)) player.selected = static_cast<BlockType>(slot);
                    break;
                }

                case SDL_EVENT_MOUSE_MOTION:
                    player.handleMouseLook(e.motion.xrel, e.motion.yrel);
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    if (e.wheel.y != 0.0f) player.selected = cycleSel(player.selected, e.wheel.y > 0 ? -1 : 1);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    if (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT) {
                        performBlockAction(e.button.button == SDL_BUTTON_LEFT);

                        actTimer = BLOCKACT_INTERVAL;
                    }

                    break;
                }
            }
        }

        if (actTimer <= 0.0f) {
            const SDL_MouseButtonFlags state = SDL_GetMouseState(nullptr, nullptr);
            const bool left = (state & SDL_BUTTON_LMASK) != 0;

            if (left || (state & SDL_BUTTON_RMASK) != 0) {
                performBlockAction(left);

                actTimer = BLOCKACT_INTERVAL;
            }
        }

        const bool* keys = SDL_GetKeyboardState(nullptr);

        player.updateInputs(keys, keys[SDL_SCANCODE_SPACE]);
        player.updatePhysics(dt, world);

        const RaycastResult target = ray();
        const std::string_view block = getBlockDef(player.selected).id;
        int w = 0;
        int h = 0;

        SDL_GetWindowSize(window, &w, &h);
        SDL_SetRenderDrawColor(renderer, 115, 184, 245, 255); // sky
        SDL_RenderClear(renderer);

        double halfW = w * .5;
        double halfH = h * .5;

        renderWorld(renderer, world, player, w, h, target);
        renderGUI(renderer, player, w, h, fps, block, driver);
        SDL_RenderDebugTextFormat(renderer, (w - (devLol.size() + 1) * 8.0), h - 16.0, "%s", devLol.c_str());

        SDL_RenderPresent(renderer);
    }

    quit:
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}