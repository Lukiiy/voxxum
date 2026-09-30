#include <cstddef>
#include <cstdint>

enum class BlockType : uint8_t {
    AIR,
    GRASS,
    DIRT,
    STONE
};

const int BLOCK_TYPE_COUNT = static_cast<size_t>(BlockType::STONE) + 1;