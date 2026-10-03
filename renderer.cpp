#include "renderer.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <string_view>
#include <vector>
#include "block.hpp"
#include "mathutils.hpp"
#include "player.hpp"
#include "world.hpp"

namespace {
    constexpr float NEAR_Z = .05f;
    constexpr float FOV_DEG = 70.0f;
    constexpr float CROSSHAIR_SIZE = 7.0f;

    constexpr float ATLAS_TILES = 4.0f;
    constexpr float TILE_UV = 1.0f / ATLAS_TILES;

    struct FaceDef {
        Vec3i n;
        Vec3 corners[4];
        SDL_FPoint uvs[4];
        Vec3 center;
        float shade;
    };

    constexpr std::array<FaceDef, 6> FACES = {{ // TODO
        {{0, 1, 0}, {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}, {{0, 0}, {0, 1}, {1, 1}, {1, 0}}, {.5f, 1.0f, .5f}, 1.0f}, // top
        {{0, -1, 0}, {{0, 0, 1}, {0, 0, 0}, {1, 0, 0}, {1, 0, 1}}, {{0, 0}, {0, 1}, {1, 1}, {1, 0}}, {.5f, 0.0f, .5f}, .5f}, // bottom
        {{0, 0, 1}, {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}, {0, 0, 1}}, {{0, 1}, {0, 0}, {1, 0}, {1, 1}}, {.5f, .5f, 1.0f}, .85f}, // north
        {{0, 0, -1}, {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}, {{0, 1}, {0, 0}, {1, 0}, {1, 1}}, {.5f, .5f, 0.0f}, .85f}, // south
        {{1, 0, 0}, {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}}, {{0, 1}, {0, 0}, {1, 0}, {1, 1}}, {1.0f, .5f, .5f}, .7f}, // east
        {{-1, 0, 0}, {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}, {0, 0, 0}}, {{0, 1}, {0, 0}, {1, 0}, {1, 1}}, {0.0f, .5f, .5f}, .7f}, // west
    }};

    struct ProjVertex {
        Vec3 pos;
        SDL_FPoint uv;
    };

    struct RenderableFace {
        float depthSq;
        ProjVertex viewVerts[5];
        int numVerts;
        SDL_FColor color;
    };

    struct ViewMatrix {
        Vec3 eye;
        Vec3 right;
        Vec3 calc;
        Vec3 forward;

        inline Vec3 transform(const Vec3& position) const {
            Vec3 dist = position - eye;

            return {
                dist.x * right.x + dist.y * right.y + dist.z * right.z,
                dist.x * calc.x + dist.y * calc.y + dist.z * calc.z,
                dist.x * forward.x + dist.y * forward.y + dist.z * forward.z
            };
        }
    };

    std::vector<RenderableFace> faceBuffer;
    std::vector<std::pair<float, int>> sortBuffer; // (depthSq, index into faceBuffer)
    std::vector<SDL_Vertex> vertBuffer;
    std::vector<int> idxBuffer;

    inline SDL_FColor getShade(SDL_FColor color, float shade, float glow = 0.0f) {
        const float red = color.r * shade;
        const float green = color.g * shade;
        const float blue = color.b * shade;

        return SDL_FColor{ red + (1.0f - red) * glow, green + (1.0f - green) * glow, blue + (1.0f - blue) * glow, color.a };
    }

    int clipNear(const ProjVertex* shape, int n, ProjVertex* clipped) {
        int count = 0;

        for (int i = 0; i < n; ++i) {
            const ProjVertex& vert = shape[i];
            const ProjVertex& nextVert = shape[(i + 1) % n];
            bool inside = vert.pos.z >= NEAR_Z;
            bool nextInside = nextVert.pos.z >= NEAR_Z;

            if (inside != nextInside) {
                float interp = (NEAR_Z - vert.pos.z) / (nextVert.pos.z - vert.pos.z);

                clipped[count++] = {
                    {
                        vert.pos.x + interp * (nextVert.pos.x - vert.pos.x),
                        vert.pos.y + interp * (nextVert.pos.y - vert.pos.y),
                        NEAR_Z
                    }, {
                        vert.uv.x + interp * (nextVert.uv.x - vert.uv.x),
                        vert.uv.y + interp * (nextVert.uv.y - vert.uv.y)
                    }
                };
            }

            if (nextInside) clipped[count++] = nextVert;
        }

        return count;
    }

    void processFace(const World& world, const ViewMatrix& view, int x, int y, int z, int faceIdx, const BlockDef& def, float glow) {
        const FaceDef& face = FACES[faceIdx];

        const BlockDef& neighbor = getBlockDef(world.getBlock(x + face.n.x, y + face.n.y, z + face.n.z)); // neighbor culling
        if (neighbor.isSolid && !neighbor.isTransparent) return;

        const Vec3 toCam = { // backface culling
            view.eye.x - (x + face.center.x),
            view.eye.y - (y + face.center.y),
            view.eye.z - (z + face.center.z)
        };

        if (face.n.x * toCam.x + face.n.y * toCam.y + face.n.z * toCam.z <= 0.0f) return;

        const Vec3 origin = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
        Vec3 viewVerts[4];
        bool allIn = true;

        for (int i = 0; i < 4; ++i) {
            viewVerts[i] = view.transform(origin + face.corners[i]);

            if (viewVerts[i].z < NEAR_Z) allIn = false;
        }

        RenderableFace rf;
        rf.depthSq = toCam.x * toCam.x + toCam.y * toCam.y + toCam.z * toCam.z;

        if (allIn) {
            rf.numVerts = 4;

            std::copy(viewVerts, viewVerts + 4, rf.viewVerts);
        } else {
            rf.numVerts = clipNear(viewVerts, 4, rf.viewVerts);

            if (rf.numVerts < 3) return;
        }

        rf.color = getShade({1, 1, 1, 1}, face.shade, glow);

        faceBuffer.push_back(rf);
    }

    void drawFace(const RenderableFace& rf, float fovFactor, float halfW, float halfH, float screenW, float screenH) {
        SDL_Vertex projected[5];
        float minX = INF;
        float maxX = -INF;
        float minY = INF;
        float maxY = -INF;

        for (int i = 0; i < rf.numVerts; ++i) {
            float perspScale = fovFactor / rf.viewVerts[i].z;
            float perspX = halfW + rf.viewVerts[i].x * perspScale;
            float perspY = halfH - rf.viewVerts[i].y * perspScale;

            projected[i] = SDL_Vertex{
                { perspX, perspY }, rf.color, { 0.0f, 0.0f }
            };

            minX = std::min(minX, perspX);
            maxX = std::max(maxX, perspX);
            minY = std::min(minY, perspY);
            maxY = std::max(maxY, perspY);
        }

        if (maxX < 0.0f || minX > screenW || maxY < 0.0f || minY > screenH) return; // frustum culling

        const int baseIdx = static_cast<int>(vertBuffer.size());

        vertBuffer.insert(vertBuffer.end(), projected, projected + rf.numVerts);

        for (int i = 1; i < rf.numVerts - 1; ++i) {
            idxBuffer.push_back(baseIdx);
            idxBuffer.push_back(baseIdx + i);
            idxBuffer.push_back(baseIdx + i + 1);
        }
    }
}

void renderWorld(SDL_Renderer* renderer, SDL_Texture* atlas, const World& world, const Player& player, int width, int height, const RaycastResult& rayTarget) {
    const Vec3 forward = player.getForwardVector();
    const Vec3 right = player.getRightVector();
    const ViewMatrix view = { player.getEyePosition(), right, {
        right.y * forward.z - right.z * forward.y,
        right.z * forward.x - right.x * forward.z,
        right.x * forward.y - right.y * forward.x
    }, forward };

    const float screenW = static_cast<float>(width);
    const float screenH = static_cast<float>(height);
    const float halfW = screenW * .5f;
    const float halfH = screenH * .5f;
    const float fovFactor = halfH / std::tan(FOV_DEG * .5f * (PI / 180.0f));
    const float alpha = (std::sin(static_cast<float>(SDL_GetTicks()) * .005f) * .5f + .5f) * .35f + .1f;

    faceBuffer.clear();

    for (int x = 0; x < World::SIZE_X; ++x) {
        for (int y = 0; y < World::SIZE_Y; ++y) {
            for (int z = 0; z < World::SIZE_Z; ++z) {
                const BlockType type = world.getBlock(x, y, z);
                if (type == BlockType::AIR) continue;

                const BlockDef& def = getBlockDef(type);
                const float glow = (rayTarget.hit && rayTarget.blockPos == Vec3i{ x, y, z }) ? alpha : 0.0f;

                for (int f = 0; f < 6; ++f) processFace(world, view, x, y, z, f, def, glow);
            }
        }
    }

    sortBuffer.clear();

    for (int i = 0; i < static_cast<int>(faceBuffer.size()); ++i) sortBuffer.emplace_back(faceBuffer[i].depthSq, i);

    std::sort(sortBuffer.begin(), sortBuffer.end(), [](const auto& a, const auto& b) {
        return a.first > b.first; // painter's algo
    });

    vertBuffer.clear();
    idxBuffer.clear();

    for (const auto& entry : sortBuffer) drawFace(faceBuffer[entry.second], fovFactor, halfW, halfH, screenW, screenH);
    if (!idxBuffer.empty()) SDL_RenderGeometry(renderer, atlas, vertBuffer.data(), static_cast<int>(vertBuffer.size()), idxBuffer.data(), static_cast<int>(idxBuffer.size()));
}

void renderGUI(SDL_Renderer *renderer, const Player &player, int width, int height, float fps, std::string_view block, const char* driver) {
    double halfW = width * .5;
    double halfH = height * .5;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugTextFormat(renderer, 8.0, 8.0, "%.0f fps", fps);
    SDL_RenderDebugTextFormat(renderer, (width - (strlen(driver) + 1) * 8.0), 8.0, "%s", driver);
    SDL_RenderDebugTextFormat(renderer, (width - block.size() * 8.0) / 2.0, height - 16.0, "%.*s", static_cast<int>(block.size()), block.data());

    // crosshair
    SDL_RenderLine(renderer, halfW - CROSSHAIR_SIZE, halfH, halfW + CROSSHAIR_SIZE, halfH);
    SDL_RenderLine(renderer, halfW, halfH - CROSSHAIR_SIZE, halfW, halfH + CROSSHAIR_SIZE);
}