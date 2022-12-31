#pragma once

namespace mcfile::be {
class BiomeSection {
public:
    void fill(biomes::BiomeId biome) {
        fPalette.clear();
        fPalette.push_back(biome);
        std::fill_n(fIndex, 4096, 0);
    }

    std::optional<biomes::BiomeId> get(int localX, int localY, int localZ) const {
        auto index = Index(localX, localY, localZ);
        assert(index);
        if (!index) {
            return std::nullopt;
        }
        int idx = fIndex[*index];
        if (idx < 0 || fPalette.size() <= idx) {
            return std::nullopt;
        }
        return fPalette[idx];
    }

    std::optional<biomes::BiomeId> getUnchecked(int localX, int localY, int localZ) const {
        if (fPalette.empty()) {
            return std::nullopt;
        }
        auto index = Index(localX, localY, localZ);
        int idx = fIndex[*index];
        return fPalette[idx];
    }

    void set(int localX, int localY, int localZ, biomes::BiomeId biome) {
        auto index = Index(localX, localY, localZ);
        assert(index);
        if (!index) {
            return;
        }
        int idx = -1;
        for (int i = 0; i < fPalette.size(); i++) {
            if (fPalette[i] == biome) {
                idx = i;
                break;
            }
        }
        if (idx < 0) {
            idx = fPalette.size();
            fPalette.push_back(biome);
        }
        fIndex[*index] = idx;
    }

    bool set2D(biomes::BiomeId biomesXZ[16][16]) {
        using namespace std;
        fPalette.clear();
        int8_t lookup[biomes::minecraft::minecraft_max_biome_id];
        fill_n(lookup, biomes::minecraft::minecraft_max_biome_id, -1);
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                auto id = biomesXZ[x][z];
                if (id >= biomes::minecraft::minecraft_max_biome_id) {
                    return false;
                }
                int8_t i = lookup[id];
                if (i < 0) {
                    i = fPalette.size();
                    lookup[id] = i;
                    fPalette.push_back(id);
                }
            }
        }
        int k = 0;
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                auto id = biomesXZ[x][z];
                int8_t i = lookup[id];
                for (int y = 0; y < 16; y++, k++) {
                    fIndex[k] = i;
                }
            }
        }
        return true;
    }

    [[nodiscard]] bool encode(std::string *d) const {
        using namespace std;
        if (!d) {
            return false;
        }

        d->clear();
        auto s = make_shared<stream::ByteStream>();
        stream::OutputStreamWriter w(s, Endian::Little);

        if (fPalette.empty()) {
            return false;
        } else if (fPalette.size() == 1) {
            if (!w.write((uint8_t)1)) {
                return false;
            }
            uint32_t b = Biome::ToUint32(fPalette[0]);
            if (!w.write(b)) {
                return false;
            }
        } else {
            int bitsPerBlock = (int)std::ceil(std::log2(fPalette.size()));
            uint8_t format = 1 + bitsPerBlock * 2;
            if (!w.write(format)) {
                return false;
            }
            vector<bool> buffer;
            for (int i = 0; i < 4096; i++) {
                uint16_t idx = fIndex[i];
                for (int j = 0; j < bitsPerBlock; j++) {
                    buffer.push_back((idx & 0x1) == 0x1);
                    idx = idx >> 1;
                }
            }
            assert(buffer.size() % 8 == 0);
            int blocksPerDword = 32 / bitsPerBlock;
            int bitsUsedPerDword = bitsPerBlock * blocksPerDword;
            for (int i = 0; i < buffer.size(); i += bitsUsedPerDword) {
                uint32_t v = 0;
                for (int j = 0; j < bitsUsedPerDword; j++) {
                    if (buffer[i + j]) {
                        v = v | (0x1 << j);
                    }
                }
                if (!w.write(v)) {
                    return false;
                }
            }
            vector<bool>().swap(buffer);
            if (!w.write((uint32_t)fPalette.size())) {
                return false;
            }
            for (biomes::BiomeId biome : fPalette) {
                uint32_t b = Biome::ToUint32(biome);
                if (!w.write(b)) {
                    return false;
                }
            }
        }

        vector<uint8_t> tmp;
        s->drain(tmp);
        copy(tmp.begin(), tmp.end(), back_inserter(*d));
        return true;
    }

    static std::shared_ptr<BiomeSection> Decode(std::string const &data, size_t *offset) {
        using namespace std;
        size_t ptr = *offset;
        if (data.size() <= ptr) {
            return nullptr;
        }
        auto ret = make_shared<BiomeSection>();
        uint8_t format = *(uint8_t *)(data.data() + ptr);
        ptr += 1;
        if (format == 1) {
            if (data.size() < ptr + 4) {
                return nullptr;
            }
            uint32_t raw = Mem::Read<uint32_t>(data, ptr);
            biomes::BiomeId biome = Biome::FromUint32(raw);
            ret->fill(biome);
            *offset = ptr + 4;
            return ret;
        } else if (format == 0) {
            return nullptr;
        }

        int bitsPerBlock = format - 1;
        if (bitsPerBlock % 2 != 0) {
            return nullptr;
        }
        bitsPerBlock /= 2;

        int blocksPerDword = 32 / bitsPerBlock;
        int numDwords = (4096 + (4096 % blocksPerDword)) / blocksPerDword;
        int numBytes = 4 * numDwords;
        if (data.size() < ptr + numBytes) {
            return nullptr;
        }
        uint16_t mask = ~(uint16_t(0xffff) << bitsPerBlock);
        size_t p = 0;
        for (int i = 0; i < numDwords; i++) {
            uint32_t v = Mem::Read<uint32_t>(data, ptr + i * 4);
            for (int j = 0; j < blocksPerDword && p < 4096; j++, p++) {
                uint16_t idx = mask & (v >> (j * bitsPerBlock));
                ret->fIndex[p] = idx;
            }
        }
        ptr += numBytes;
        if (data.size() < ptr + 4) {
            return nullptr;
        }
        uint16_t maxIndex = *std::max_element(ret->fIndex, ret->fIndex + 4096);
        uint32_t numPaletteEntries = Mem::Read<uint32_t>(data, ptr);
        if (maxIndex >= numPaletteEntries) {
            return nullptr;
        }
        ptr += 4;
        if (data.size() < ptr + 4 * numPaletteEntries) {
            return nullptr;
        }
        if (numPaletteEntries > 4096) {
            return nullptr;
        }
        for (int i = 0; i < numPaletteEntries; i++) {
            uint32_t raw = Mem::Read<uint32_t>(data, ptr);
            ptr += 4;
            biomes::BiomeId biome = Biome::FromUint32(raw);
            ret->fPalette.push_back(biome);
        }
        *offset = ptr;
        return ret;
    }

private:
    static std::optional<uint32_t> Index(int localX, int localY, int localZ) {
        if (localX < 0 || 16 <= localX || localY < 0 || 16 <= localY || localZ < 0 || 16 <= localZ) {
            return std::nullopt;
        }
        return (localX * 16 + localZ) * 16 + localY;
    }

private:
    std::vector<biomes::BiomeId> fPalette;
    uint16_t fIndex[4096];
};
} // namespace mcfile::be
