#pragma once

#include "column.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

struct HeaderEntry {
    uint32_t offset = 0;
    uint32_t length = 0;
};

class RegionFile {
public:
    static constexpr int SIZE = 32;
    static constexpr int HEADER_ENTRIES = SIZE * SIZE;
    static constexpr size_t HEADER_SIZE_BYTES = HEADER_ENTRIES * sizeof(HeaderEntry);

    static std::string getPath(const std::string& directory, int regionX, int regionZ) {
        return directory + "/r." + std::to_string(regionX) + "." + std::to_string(regionZ) + ".region";
    }

    static bool loadColumn(const std::string& directory, int colX, int colZ, ChunkColumn& column) {
        std::string path = getPath(directory, regionCoord(colX), regionCoord(colZ));
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) return false;

        uint64_t fileSize = static_cast<uint64_t>(file.tellg()); // the cursor's at the end, which would in theory return the size as bytes
        if (fileSize < HEADER_SIZE_BYTES) return false;

        HeaderEntry entry;
        file.seekg(headerIndex(colX, colZ) * sizeof(HeaderEntry));
        file.read(reinterpret_cast<char*>(&entry), sizeof(entry));

        if (!file || entry.offset < HEADER_SIZE_BYTES || entry.length == 0 || static_cast<uint64_t>(entry.offset) + entry.length > fileSize) return false; // the !file means short read

        std::vector<uint8_t> bytes(entry.length); // resize
        file.seekg(entry.offset);
        file.read(reinterpret_cast<char*>(bytes.data()), entry.length);

        if (!file) return false; // short read 2

        column.deserialize(bytes);
        return true;
    }

    static bool saveColumn(const std::string& directory, const ChunkColumn& column) {
        std::filesystem::create_directories(directory);
        std::string path = getPath(directory, regionCoord(column.x), regionCoord(column.z));

        if (!std::filesystem::exists(path)) {
            std::ofstream create(path, std::ios::binary);
            std::vector<HeaderEntry> empty(HEADER_ENTRIES);

            create.write(reinterpret_cast<const char*>(empty.data()), HEADER_SIZE_BYTES);
            if (!create) return false;
        }

        std::fstream file(path, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) return false;

        int idx = headerIndex(column.x, column.z);
        HeaderEntry entry;

        file.seekg(idx * sizeof(HeaderEntry));
        file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
        if (!file) return false;

        std::vector<uint8_t> bytes = column.serialize();
        uint32_t length = static_cast<uint32_t>(bytes.size());
        uint32_t writeOff;

        if (entry.offset >= HEADER_SIZE_BYTES && length <= entry.length) {
            writeOff = entry.offset; // reuse
        } else {
            file.seekp(0, std::ios::end); // append

            writeOff = static_cast<uint32_t>(file.tellp());
        }

        file.seekp(writeOff);
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        if (!file) return false;

        entry.offset = writeOff;
        entry.length = length;

        file.seekp(idx * sizeof(HeaderEntry));
        file.write(reinterpret_cast<const char*>(&entry), sizeof(entry)); // update entry

        return static_cast<bool>(file);
    }

private:
    static int regionCoord(int col) {
        return col >> 5;
    }

    static int localCoord(int col) {
        return col & (SIZE - 1);
    }

    static int headerIndex(int colX, int colZ) {
        return localCoord(colX) + localCoord(colZ) * SIZE;
    }
};