#pragma once

namespace mcfile::je::chunksection {

class BlockStatesParser116 {
public:
    static void PaletteIndicesFromBlockStates(std::vector<int64_t> const &blockStates, std::vector<uint16_t> &paletteIndices) {
        paletteIndices.resize(16 * 16 * 16);

        size_t const numBits = blockStates.size() * 64;
        size_t const bitsPerIndex = std::floor((double)numBits / 4096.0);
        size_t const palettesPerLong = std::floor(64.0 / (double)bitsPerIndex);
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        size_t index = 0;
        size_t uint64Index = 0;
        uint64_t v = *(uint64_t *)blockStates.data();

        for (int offsetY = 0; offsetY < 16; offsetY++) {
            for (int offsetZ = 0; offsetZ < 16; offsetZ++) {
                for (int offsetX = 0; offsetX < 16; offsetX++) {
                    uint64_t const paletteIndex = v & mask;

                    paletteIndices[index] = (uint16_t)(0xffff & paletteIndex);

                    index++;
                    v = v >> bitsPerIndex;

                    if (index % palettesPerLong == 0) {
                        uint64Index++;
                        if (blockStates.size() <= uint64Index) {
                            goto last;
                        }
                        v = *(uint64_t *)(blockStates.data() + uint64Index);
                    }
                }
            }
        }
    last:
        return;
    }

    static void BlockStatesFromPaletteIndices(size_t numPaletteEntries, std::vector<uint16_t> const &paletteIndices, std::vector<int64_t> &blockStates) {
        blockStates.clear();

        uint8_t bitsPerBlock;
        int blocksPerLong;
        if (numPaletteEntries <= 16) {
            bitsPerBlock = 4;
            blocksPerLong = 16;
        } else if (numPaletteEntries <= 32) {
            bitsPerBlock = 5;
            blocksPerLong = 12;
        } else if (numPaletteEntries <= 64) {
            bitsPerBlock = 6;
            blocksPerLong = 10;
        } else if (numPaletteEntries <= 128) {
            bitsPerBlock = 7;
            blocksPerLong = 9;
        } else if (numPaletteEntries <= 256) {
            bitsPerBlock = 8;
            blocksPerLong = 8;
        } else {
            bitsPerBlock = 16;
            blocksPerLong = 4;
        }
        uint64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerBlock);
        int count = 0;
        uint64_t v = 0;
        for (uint16_t idx : paletteIndices) {
            uint64_t t = (mask & (uint64_t)idx) << (count * bitsPerBlock);
            v |= t;
            count++;
            if (count == blocksPerLong) {
                blockStates.push_back(*(int64_t *)&v);
                count = 0;
                v = 0;
            }
        }
        if (count > 0) {
            blockStates.push_back(*(int64_t *)&v);
        }
    }

private:
    BlockStatesParser116() = delete;
};

} // namespace mcfile::je::chunksection
