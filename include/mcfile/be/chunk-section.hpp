#pragma once

namespace mcfile::be {

class ChunkSection {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const {
        auto const index = paletteIndexAt(offsetX, offsetY, offsetZ);
        if (!index) {
            return nullptr;
        }
        return fPalette[*index];
    }

    static std::shared_ptr<ChunkSection> Parse(std::string const& data) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        vector<uint8_t> buffer;
        copy(data.begin(), data.end(), back_inserter(buffer));

        auto bs = make_shared<ByteStream>(buffer);
        InputStreamReader sr(bs, {.fLittleEndian = true});

        uint8_t version;
        if (!sr.read(&version)) {
            return nullptr;
        }

        uint8_t numLayers;
        if (!sr.read(&numLayers)) {
            return nullptr;
        }

        uint8_t bitsPerBlock;
        if (!sr.read(&bitsPerBlock)) {
            return nullptr;
        }
        bitsPerBlock = bitsPerBlock / 2;

        int blocksPerWord = 32 / bitsPerBlock;
        int numWords;
        if (4096 % blocksPerWord == 0) {
            numWords = 4096 / blocksPerWord;
        } else {
            numWords = (int)ceilf(4096.0 / blocksPerWord);
        }
        int numBytes = numWords * 4;
        vector<uint8_t> indexBuffer(numBytes);
        if (!sr.read(indexBuffer)) {
            return nullptr;
        }

        uint32_t const mask = ~((~((uint32_t)0)) << bitsPerBlock);
        vector<uint16_t> index;
        index.reserve(4096);
        auto indexBufferStream = make_shared<ByteStream>(indexBuffer);
        InputStreamReader sr2(indexBufferStream, {.fLittleEndian = true});
        for (int i = 0; i < numWords; i++) {
            uint32_t word;
            sr2.read(&word);
            for (int j = 0; j < blocksPerWord && index.size() < 4096; j++) {
                uint16_t v = word & mask;
                index.push_back(v);
                word = word >> bitsPerBlock;
            }
        }
        assert(index.size() == 4096);

        uint32_t numPaletteEntries;
        if (!sr.read(&numPaletteEntries)) {
            return nullptr;
        }

        vector<shared_ptr<Block const>> palette;
        palette.reserve(numPaletteEntries);

        for (uint32_t i = 0; i < numPaletteEntries; i++) {
            auto tag = make_shared<CompoundTag>();
            uint8_t type;
            sr.read(&type);
            string empty;
            sr.read(empty);
            tag->read(sr);
            if (!tag->valid()) {
                return nullptr;
            }
            auto name = tag->string("name");
            auto states = tag->compoundTag("states");
            auto version = tag->int32("version");
            if (!name || !version) {
                return nullptr;
            }
            auto block = make_shared<Block const>(*name, states, *version);
            palette.push_back(block);
        }

        return shared_ptr<ChunkSection>(new ChunkSection(palette, index));
    }

private:
    ChunkSection(std::vector<std::shared_ptr<Block const>>& palette, std::vector<uint16_t>& paletteIndices) {
        fPalette.swap(palette);
        fPaletteIndices.swap(paletteIndices);
    }

    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0 || fPaletteIndices.size() <= index) {
            return std::nullopt;
        }

        int paletteIndex = fPaletteIndices[index];
        if (paletteIndex < 0 || fPalette.size() <= paletteIndex) {
            return std::nullopt;
        }

        return (size_t)paletteIndex;
    }

    static int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return offsetY * 16 * 16 + offsetZ * 16 + offsetX;
    }

private:
    std::vector<std::shared_ptr<Block const>> fPalette;
    std::vector<uint16_t> fPaletteIndices;
};

}
