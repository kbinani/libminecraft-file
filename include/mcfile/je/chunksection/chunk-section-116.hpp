#pragma once

namespace mcfile::je::chunksection {

class ChunkSection116 : public ChunkSection113Base<BlockStatesParser116> {
public:
    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        vector<uint16_t> paletteIndices;
        auto extra = make_shared<nbt::CompoundTag>();
        return shared_ptr<ChunkSection116>(
            new ChunkSection116(sectionY,
                                palette,
                                paletteIndices,
                                extra));
    }

private:
    ChunkSection116(int y,
                    std::vector<std::shared_ptr<Block const>> const &palette,
                    std::vector<uint16_t> const &paletteIndices,
                    std::shared_ptr<nbt::CompoundTag> const &extra)
        : ChunkSection113Base<BlockStatesParser116>(y, palette, paletteIndices, extra) {
    }
};

} // namespace mcfile::je::chunksection
