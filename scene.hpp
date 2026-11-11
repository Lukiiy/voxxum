#pragma once
#include <SDL3/SDL.h>
#include <memory>

class Scene {
public:
    virtual ~Scene() = default;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
};

class SceneManager {
private:
    std::unique_ptr<Scene> current;
    std::unique_ptr<Scene> pending;

    bool shouldQuit = false;

public:
    template <typename T, typename... Args>
    void change(Args&&... args) {
        pending = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void update() {
        if (pending) {
            if (current) current->onExit();

            current = std::move(pending);

            current->onEnter();
        }
    }

    Scene* getCurrent() {
        return current.get();
    }

    void requestQuit() {
        shouldQuit = true;
    }

    bool isQuitting() const {
        return shouldQuit;
    }
};