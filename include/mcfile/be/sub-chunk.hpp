#pragma once

namespace mcfile::be {

class SubChunk {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const {
        auto const index = paletteIndexAt(offsetX, offsetY, offsetZ);
        if (!index) {
            return nullptr;
        }
        return fPalette[*index];
    }

    std::shared_ptr<Block const> blockAtUnchecked(int offsetX, int offsetY, int offsetZ) const {
        if (fPalette.empty()) {
            return nullptr;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        auto paletteIndex = fPaletteIndices[index];
        return fPalette[paletteIndex];
    }

    static std::shared_ptr<SubChunk> Parse(std::string const &data, int8_t chunkY, Encoding encoding) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        auto bs = make_shared<ByteInputStream>(data);
        InputStreamReader sr(bs, encoding);

        uint8_t version;
        if (!sr.read(&version)) {
            return nullptr;
        }

        // version values:
        // 1.0.0.16: 0
        // 1.1.5.0: 0
        // 1.2.10.2: 0
        // 1.4.2.0: 8
        // 1.10.1.1: 9

        if (version == 9) {
            return ParseVersion9(sr, chunkY);
        } else if (version == 8) {
            return ParseVersion8(sr, chunkY);
        } else {
            return ParseClassic(sr, chunkY);
        }
    }

    static int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return (offsetX * 16 + offsetZ) * 16 + offsetY;
    }

private:
    static std::shared_ptr<SubChunk> ParseClassic(mcfile::stream::InputStreamReader &sr, int8_t chunkY) {
        using namespace std;

        array<uint8_t, 4096> id;
        array<uint8_t, 4096> data;
        for (int i = 0; i < 4096; i++) {
            if (!sr.read(id.data() + i)) {
                return nullptr;
            }
        }
        for (int i = 0; i < 4096; i += 2) {
            uint8_t v;
            if (!sr.read(&v)) {
                return nullptr;
            }
            data[i] = 0xf & v;
            data[i + 1] = 0xf & (v >> 4);
        }
        vector<uint16_t> indices;
        vector<shared_ptr<Block const>> palette;
        vector<uint16_t> waterIndices;
        vector<shared_ptr<Block const>> waterPalette;
        map<pair<uint8_t, uint8_t>, uint16_t> paletteLut;
        for (size_t i = 0; i < 4096; i++) {
            uint8_t blockId = id[i];
            uint8_t val = data[i];
            auto key = make_pair(blockId, val);
            auto found = paletteLut.find(key);
            if (found == paletteLut.end()) {
                uint16_t index = (uint16_t)palette.size();
                auto block = make_shared<Block>(Block::BlockNameById(blockId), make_shared<nbt::CompoundTag>(), std::nullopt, val);
                palette.push_back(block);
                paletteLut[key] = index;
                indices.push_back(index);
            } else {
                indices.push_back(found->second);
            }
        }
        return shared_ptr<SubChunk>(new SubChunk(palette, indices, waterPalette, waterIndices, chunkY, 0));
    }

    static std::shared_ptr<SubChunk> ParseVersion9(mcfile::stream::InputStreamReader &sr, int8_t chunkY) {
        using namespace std;

        uint8_t numLayers;
        if (!sr.read(&numLayers)) {
            return nullptr;
        }
        int8_t cy;
        if (!sr.read(&cy)) {
            return nullptr;
        }
        if (cy != chunkY) {
            return nullptr;
        }
        if (numLayers == 0) {
            return nullptr;
        }
        vector<uint16_t> indices;
        vector<shared_ptr<Block const>> palette;
        if (!ParsePalette(sr, indices, palette)) {
            return nullptr;
        }
        vector<uint16_t> waterIndices;
        vector<shared_ptr<Block const>> waterPalette;
        if (numLayers > 1) {
            if (!ParsePalette(sr, waterIndices, waterPalette)) {
                return nullptr;
            }
        }
        return shared_ptr<SubChunk>(new SubChunk(palette, indices, waterPalette, waterIndices, chunkY, 9));
    }

    static std::shared_ptr<SubChunk> ParseVersion8(mcfile::stream::InputStreamReader &sr, int8_t chunkY) {
        using namespace std;

        uint8_t numLayers;
        if (!sr.read(&numLayers)) {
            return nullptr;
        }
        if (numLayers == 0) {
            return nullptr;
        }
        vector<uint16_t> indices;
        vector<shared_ptr<Block const>> palette;
        if (!ParsePalette(sr, indices, palette)) {
            return nullptr;
        }
        if (indices.size() != 4096 || palette.empty()) {
            return nullptr;
        }
        vector<uint16_t> waterIndices;
        vector<shared_ptr<Block const>> waterPalette;
        if (numLayers > 1) {
            if (!ParsePalette(sr, waterIndices, waterPalette)) {
                return nullptr;
            }
        }
        return shared_ptr<SubChunk>(new SubChunk(palette, indices, waterPalette, waterIndices, chunkY, 8));
    }

    static bool ParsePalette(mcfile::stream::InputStreamReader &sr, std::vector<uint16_t> &index, std::vector<std::shared_ptr<Block const>> &palette) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        uint8_t bitsPerBlock;
        if (!sr.read(&bitsPerBlock)) {
            return false;
        }
        if (bitsPerBlock == 0) {
            // layer exists, but no data.
            return true;
        }
        bitsPerBlock = bitsPerBlock / 2;

        int blocksPerWord = 32 / bitsPerBlock;
        int numWords;
        if (4096 % blocksPerWord == 0) {
            numWords = 4096 / blocksPerWord;
        } else {
            numWords = (int)ceilf(4096.0 / blocksPerWord);
        }
        vector<uint32_t> indexBuffer(numWords);
        for (int i = 0; i < numWords; i++) {
            uint32_t v;
            if (!sr.read(&v)) {
                return false;
            }
            indexBuffer[i] = v;
        }

        uint32_t const mask = ~((~((uint32_t)0)) << bitsPerBlock);
        index.resize(4096);
        int k = 0;
        for (int i = 0; i < numWords; i++) {
            uint32_t word = indexBuffer[i];
            for (int j = 0; j < blocksPerWord && k < 4096; j++) {
                uint16_t v = (word >> (bitsPerBlock * j)) & mask;
                index[k++] = v;
            }
        }

        uint32_t numPaletteEntries;
        if (!sr.read(&numPaletteEntries)) {
            return false;
        }
        if (numPaletteEntries > 4096) {
            return false;
        }

        palette.reserve(numPaletteEntries);

        for (uint32_t i = 0; i < numPaletteEntries; i++) {
            auto tag = CompoundTag::Read(sr);
            if (!tag) {
                return false;
            }
            auto block = Block::FromCompound(*tag);
            if (!block) {
                return false;
            }
            palette.push_back(block);
        }

        return true;
    }

    SubChunk(
        std::vector<std::shared_ptr<Block const>> &palette, std::vector<uint16_t> &paletteIndices,
        std::vector<std::shared_ptr<Block const>> &waterPalette, std::vector<uint16_t> &waterPaletteIndices,
        int8_t chunkY,
        uint8_t chunkVersion)
        : fChunkY(chunkY)
        , fChunkVersion(chunkVersion) {
        fPalette.swap(palette);
        fPaletteIndices.swap(paletteIndices);
        fWaterPalette.swap(waterPalette);
        fWaterPaletteIndices.swap(waterPaletteIndices);
    }

    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0 || fPaletteIndices.size() <= index) {
            return std::nullopt;
        }

        uint16_t paletteIndex = fPaletteIndices[index];
        if (fPalette.size() <= paletteIndex) {
            return std::nullopt;
        }

        return (size_t)paletteIndex;
    }

public:
    std::vector<std::shared_ptr<Block const>> fPalette;
    std::vector<uint16_t> fPaletteIndices;
    std::vector<std::shared_ptr<Block const>> fWaterPalette;
    std::vector<uint16_t> fWaterPaletteIndices;
    int8_t const fChunkY;
    uint8_t const fChunkVersion;
};

} // namespace mcfile::be
