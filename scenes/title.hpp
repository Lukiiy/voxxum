#pragma once
#include "../scene.hpp"
#include "../sceneManager.hpp"
#include "gameScene.hpp"
#include <string>

class TitleScene : public Scene {
private:
    SDL_Renderer* renderer = nullptr;
    std::string saveDir;

public:
    TitleScene(const std::string& directory) : saveDir(directory) {}

    void init(SDL_Window* window) override {
        SDL_SetWindowSize(window, 400, 400);
        SDL_SetWindowResizable(window, false);

        renderer = SDL_CreateRenderer(window, nullptr);
    }

    void handleEvent(const SDL_Event& e, SceneManager& manager) override {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            SDL_Window* win = SDL_GetRenderWindow(renderer);

            if (win) {
                SDL_SetWindowSize(win, 1280, 720);
                SDL_SetWindowResizable(win, true);
            }

            manager.change<GameScene>(saveDir);
        }

        if (e.type == SDL_EVENT_QUIT) manager.quit();
    }

    void render(int w, int h) override { // TODO really awesome
        if (!renderer) return;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderScale(renderer, 3.0f, 3.0f);
        SDL_RenderDebugText(renderer, 44.0f, 50.0f, "voxxum");
        SDL_SetRenderScale(renderer, 1.5f, 1.5f);
        SDL_RenderDebugText(renderer, 10.0f, 125.0f, "press any key");
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);
        SDL_RenderPresent(renderer);
    }

    void finish() override {
        if (renderer) {
            SDL_DestroyRenderer(renderer);

            renderer = nullptr;
        }
    }
};