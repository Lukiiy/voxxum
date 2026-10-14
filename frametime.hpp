#pragma once

#include <SDL3/SDL_timer.h>
#include <cstdint>

class FrameTime {
public:
    explicit FrameTime(float second = .5f) : window(second), frequency(static_cast<double>(SDL_GetPerformanceFrequency())), last(SDL_GetPerformanceCounter()) {}

    float tick() {
        const Uint64 now = SDL_GetPerformanceCounter();
        const double deltaTime = static_cast<double>(now - last) / frequency;

        last = now;
        accum += deltaTime;
        ++frames;

        if (accum >= window) {
            fps = static_cast<float>(frames / accum);
            avgFrameMs = static_cast<float>(accum / frames * 1000);
            accum = 0.0;
            frames = 0;
        }

        return static_cast<float>(deltaTime);
    }

    float getFps() const {
        return fps;
    }

    float getFrameMs() const {
        return avgFrameMs;
    }

private:
    float window;
    double frequency;
    Uint64 last;
    double accum = 0.0;
    uint32_t frames = 0;
    float fps = 0.0f;
    float avgFrameMs = 0.0f;
};