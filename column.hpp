#pragma once

#include "chunk.hpp"
#include <cstring>
#include <unordered_map>
#include <vector>
#include <zstd.h>

class ChunkColumn {
public:
    int x = 0;
    int z = 0;
    bool dirty = false;

    std::unordered_map<int, SquareChunk> sections;

    BlockType getBlock(int x, int y, int z) const {
        auto it = sections.find(y >> 4);
        if (it == sections.end()) return BlockType::AIR;

        return it->second.getBlock(x, y & 15, z);
    }

    void setBlock(int x, int y, int z, BlockType type) {
        auto it = sections.find(y >> 4);
        if (it == sections.end()) {
            if (type == BlockType::AIR) return;

            it = sections.try_emplace(y >> 4).first;
        }

        it->second.setBlock(x, y & 15, z, type);
        dirty = true;
    }

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> raw;
        const uint8_t* xBytes = reinterpret_cast<const uint8_t*>(&x);
        const uint8_t* zBytes = reinterpret_cast<const uint8_t*>(&z);

        raw.insert(raw.end(), xBytes, xBytes + sizeof(int));
        raw.insert(raw.end(), zBytes, zBytes + sizeof(int));

        uint32_t secSize = static_cast<uint32_t>(sections.size());
        const uint8_t* secBytes = reinterpret_cast<const uint8_t*>(&secSize);

        raw.insert(raw.end(), secBytes, secBytes + sizeof(uint32_t));

        for (const auto& [chunkY, chunk] : sections) {
            const uint8_t* yBytes = reinterpret_cast<const uint8_t*>(&chunkY);

            raw.insert(raw.end(), yBytes, yBytes + sizeof(int));
            chunk.serialize(raw);
        }

        std::vector<uint8_t> compressed(ZSTD_compressBound(raw.size()));
        size_t cSize = ZSTD_compress(compressed.data(), compressed.capacity(), raw.data(), raw.size(), 3);

        compressed.resize(cSize);

        return compressed;
    }

    void deserialize(const std::vector<uint8_t>& data) {
        unsigned long long const decompressSize = ZSTD_getFrameContentSize(data.data(), data.size());
        std::vector<uint8_t> raw(decompressSize);

        ZSTD_decompress(raw.data(), raw.size(), data.data(), data.size());

        const uint8_t* dataMem = raw.data();

        std::memcpy(&x, dataMem, sizeof(int));
        dataMem += sizeof(int);

        std::memcpy(&z, dataMem, sizeof(int));
        dataMem += sizeof(int);

        uint32_t secCount = 0;
        std::memcpy(&secCount, dataMem, sizeof(uint32_t));
        dataMem += sizeof(uint32_t);

        sections.clear();
        for (uint32_t i = 0; i < secCount; ++i) {
            int chunkY = 0;

            std::memcpy(&chunkY, dataMem, sizeof(int));
            dataMem += sizeof(int);

            SquareChunk chunk;

            chunk.deserialize(dataMem);
            sections[chunkY] = chunk;
        }
    }
};