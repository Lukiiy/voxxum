#pragma once

#include "block.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>
#include <bit>

class SquareChunk {
public:
    static constexpr int SIZE = 16;
    static constexpr int VOLUME = SIZE * SIZE * SIZE;
    static constexpr int ENTRY_BITS = 64; // uint64_t * 8

    SquareChunk() : bitsPerBlock(0) {
        palette.push_back(BlockType::AIR);
    }

    BlockType getBlock(int x, int y, int z) const {
        uint32_t palIdx = getPaletteIndex(getIndex(x, y, z));

        return (palIdx < palette.size()) ? palette[palIdx] : BlockType::AIR;
    }

    void setBlock(int x, int y, int z, BlockType type) {
        uint32_t palIdx = 0;
        auto found = std::find(palette.begin(), palette.end(), type);

        if (found != palette.end()) {
            palIdx = static_cast<uint32_t>(std::distance(palette.begin(), found));
        } else {
            palIdx = static_cast<uint32_t>(palette.size());

            palette.push_back(type);

            uint8_t required = std::max<uint8_t>(1, static_cast<uint8_t>(std::bit_width(palette.size() - 1)));
            if (required > bitsPerBlock) resizeBitArray(required);
        }

        setPaletteIndex(getIndex(x, y, z), palIdx);
    }

    void serialize(std::vector<uint8_t>& buffer) const {
        buffer.push_back(bitsPerBlock);

        uint16_t palSize = static_cast<uint16_t>(palette.size());
        const uint8_t* palBytes = reinterpret_cast<const uint8_t*>(&palSize);

        buffer.insert(buffer.end(), palBytes, palBytes + sizeof(uint16_t));

        for (BlockType block : palette) buffer.push_back(static_cast<uint8_t>(block));

        uint32_t dataSize = static_cast<uint32_t>(data.size());
        const uint8_t* dataSizeBytes = reinterpret_cast<const uint8_t*>(&dataSize);

        buffer.insert(buffer.end(), dataSizeBytes, dataSizeBytes + sizeof(uint32_t));

        if (dataSize > 0) {
            const uint8_t* raw = reinterpret_cast<const uint8_t*>(data.data());

            buffer.insert(buffer.end(), raw, raw + (dataSize * sizeof(uint64_t)));
        }
    }

    void deserialize(const uint8_t*& ptr) {
        bitsPerBlock = *ptr++;

        uint16_t palSize = 0;
        std::memcpy(&palSize, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);

        palette.resize(palSize);
        for (uint16_t i = 0; i < palSize; ++i) palette[i] = static_cast<BlockType>(*ptr++);

        uint32_t dataSize = 0;
        std::memcpy(&dataSize, ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        data.resize(dataSize);
        if (dataSize > 0) {
            std::memcpy(data.data(), ptr, dataSize * sizeof(uint64_t));

            ptr += dataSize * sizeof(uint64_t);
        }
    }

private:
    std::vector<BlockType> palette;
    std::vector<uint64_t> data;
    uint8_t bitsPerBlock;

    inline size_t getIndex(int x, int y, int z) const {
        return (y * SIZE * SIZE) + (z * SIZE) + x;
    }

    uint32_t getPaletteIndex(size_t idx) const {
        if (bitsPerBlock == 0) return 0;

        size_t bitIdx = idx * bitsPerBlock;
        size_t arrayIdx = bitIdx / ENTRY_BITS;
        size_t bitOffset = bitIdx % ENTRY_BITS;
        uint64_t mask = (1ULL << bitsPerBlock) - 1;
        uint64_t val = (data[arrayIdx] >> bitOffset) & mask;

        if (bitOffset + bitsPerBlock > ENTRY_BITS) {
            size_t bitsRead = ENTRY_BITS - bitOffset;
            size_t bitsRemain = bitsPerBlock - bitsRead;
            uint64_t remainMask = (1ULL << bitsRemain) - 1; // crazy

            val |= (data[arrayIdx + 1] & remainMask) << bitsRead;
        }

        return static_cast<uint32_t>(val);
    }

    void setPaletteIndex(size_t idx, uint32_t paletteIdx) {
        if (bitsPerBlock == 0) return;

        size_t bitIdx = idx * bitsPerBlock;
        size_t arrayIdx = bitIdx / ENTRY_BITS;
        size_t bitOffset = bitIdx % ENTRY_BITS;
        uint64_t mask = (1ULL << bitsPerBlock) - 1;

        data[arrayIdx] &= ~(mask << bitOffset);
        data[arrayIdx] |= (static_cast<uint64_t>(paletteIdx) & mask) << bitOffset;

        if (bitOffset + bitsPerBlock > ENTRY_BITS) {
            size_t bitsWritten = ENTRY_BITS - bitOffset;
            size_t bitsRemain = bitsPerBlock - bitsWritten;
            uint64_t remainMask = (1ULL << bitsRemain) - 1;

            data[arrayIdx + 1] &= ~remainMask;
            data[arrayIdx + 1] |= ((static_cast<uint64_t>(paletteIdx) >> bitsWritten) & remainMask);
        }
    }

    void resizeBitArray(uint8_t newBits) {
        if (bitsPerBlock == newBits) return;

        std::vector<uint32_t> tempIdx(VOLUME);
        for (size_t i = 0; i < VOLUME; ++i) tempIdx[i] = getPaletteIndex(i);

        bitsPerBlock = newBits;

        data.assign((VOLUME * bitsPerBlock + ENTRY_BITS - 1) / ENTRY_BITS, 0ULL);
        for (size_t i = 0; i < VOLUME; ++i) setPaletteIndex(i, tempIdx[i]);
    }
};