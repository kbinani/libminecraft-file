#pragma once

namespace mcfile::je::chunksection {

class ChunkSection113 : public ChunkSection113Base<BlockStatesParser113> {
public:
    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        vector<uint16_t> paletteIndices;
        auto extra = make_shared<nbt::CompoundTag>();
        return shared_ptr<ChunkSection113>(
            new ChunkSection113(sectionY,
                                palette,
                                paletteIndices,
                                extra));
    }

private:
    ChunkSection113(int y,
                    std::vector<std::shared_ptr<Block const>> const &palette,
                    std::vector<uint16_t> const &paletteIndices,
                    std::shared_ptr<nbt::CompoundTag> const &extra)
        : ChunkSection113Base<BlockStatesParser113>(y, palette, paletteIndices, extra) {
    }
};

} // namespace mcfile::je::chunksection
