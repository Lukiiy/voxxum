#pragma once
#include <SDL3/SDL.h>

class SceneManager;

class Scene {
public:
    virtual ~Scene() = default;

    virtual void init(SDL_Window* window) = 0;
    virtual void handleEvent(const SDL_Event& e, SceneManager& sm) = 0;
    virtual void update(float dt, SceneManager& sm) = 0;
    virtual void render(SDL_Window* window, int w, int h) = 0;
    virtual void finish() = 0;
};