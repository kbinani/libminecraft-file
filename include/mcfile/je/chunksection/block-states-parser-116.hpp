#pragma once

namespace mcfile::je::chunksection {

class BlockStatesParser116 {
public:
    template<size_t size>
    static void GenericPaletteIndicesFromBlockStates(std::vector<int64_t> const &blockStates, std::vector<uint16_t> &paletteIndices) {
        paletteIndices.resize(size * size * size);

        size_t const numBits = blockStates.size() * 64;
        size_t const bitsPerIndex = std::floor((double)numBits / (size * size * size));
        size_t const palettesPerLong = std::floor(64.0 / (double)bitsPerIndex);
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        size_t index = 0;
        size_t uint64Index = 0;
        uint64_t v = *(uint64_t *)blockStates.data();

        for (int offsetY = 0; offsetY < size; offsetY++) {
            for (int offsetZ = 0; offsetZ < size; offsetZ++) {
                for (int offsetX = 0; offsetX < size; offsetX++) {
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

    static void PaletteIndicesFromBlockStates(std::vector<int64_t> const &blockStates, std::vector<uint16_t> &paletteIndices) {
        GenericPaletteIndicesFromBlockStates<16>(blockStates, paletteIndices);
    }

    static void BlockStatesFromPaletteIndices(size_t numPaletteEntries, std::vector<uint16_t> const &paletteIndices, std::vector<int64_t> &blockStates) {
        if (numPaletteEntries <= 16) {
            PackPaletteIndexToI64<4, 16>(paletteIndices, blockStates);
        } else if (numPaletteEntries <= 32) {
            PackPaletteIndexToI64<5, 12>(paletteIndices, blockStates);
        } else if (numPaletteEntries <= 64) {
            PackPaletteIndexToI64<6, 10>(paletteIndices, blockStates);
        } else if (numPaletteEntries <= 128) {
            PackPaletteIndexToI64<7, 9>(paletteIndices, blockStates);
        } else if (numPaletteEntries <= 256) {
            PackPaletteIndexToI64<8, 8>(paletteIndices, blockStates);
        } else {
            PackPaletteIndexToI64<16, 4>(paletteIndices, blockStates);
        }
    }

    template<size_t BitsPerBlock, size_t BlocksPerLong>
    static void PackPaletteIndexToI64(std::vector<uint16_t> const &paletteIndices, std::vector<int64_t> &blockStates) {
        blockStates.clear();

        uint64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - BitsPerBlock);
        int count = 0;
        uint64_t v = 0;
        for (uint16_t idx : paletteIndices) {
            uint64_t t = (mask & (uint64_t)idx) << (count * BitsPerBlock);
            v |= t;
            count++;
            if (count == BlocksPerLong) {
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
