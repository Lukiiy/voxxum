#pragma once
#include "scene.hpp"
#include <memory>

class SceneManager {
private:
    SDL_Window* window = nullptr;
    std::unique_ptr<Scene> current;
    bool shouldQuit = false;

public:
    SceneManager(SDL_Window* window) : window(window) {}

    template <typename T, typename... Args>
    void change(Args&&... args) {
        if (current) current->finish();

        current = std::make_unique<T>(std::forward<Args>(args)...);

        current->init(window);
    }

    void quit() {
        shouldQuit = true;
    }

    bool isQuitting() const {
        return shouldQuit;
    }

    Scene* getActive() {
        return current.get();
    }
};