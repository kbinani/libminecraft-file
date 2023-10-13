#pragma once

namespace mcfile::je::chunksection {

class ChunkSection113 : public ChunkSection113Base<BlockStatesParser113> {
public:
    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        palette.push_back(make_shared<Block const>(u8"minecraft:air"));
        vector<uint16_t> paletteIndices(4096, 0);
        vector<uint8_t> blockLight;
        vector<uint8_t> skyLight;
        auto extra = make_shared<nbt::CompoundTag>();
        return shared_ptr<ChunkSection113>(
            new ChunkSection113(sectionY,
                                palette,
                                paletteIndices,
                                blockLight,
                                skyLight,
                                extra));
    }

    ChunkSection113 *clone() const override {
        using namespace std;
        unique_ptr<ChunkSection113> s(new ChunkSection113(fY));
        copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(s->fBlockLight));
        copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(s->fSkyLight));
        s->fBlocks = fBlocks;
        if (fExtra) {
            s->fExtra = fExtra->copy();
        }
        return s.release();
    }

private:
    ChunkSection113(int y,
                    std::vector<std::shared_ptr<Block const>> const &palette,
                    std::vector<uint16_t> const &paletteIndices,
                    std::vector<uint8_t> &blockLight,
                    std::vector<uint8_t> &skyLight,
                    std::shared_ptr<nbt::CompoundTag> const &extra)
        : ChunkSection113Base<BlockStatesParser113>(y, palette, paletteIndices, blockLight, skyLight, extra) {
    }

    explicit ChunkSection113(int y)
        : ChunkSection113Base<BlockStatesParser113>(y) {}
};

} // namespace mcfile::je::chunksection
