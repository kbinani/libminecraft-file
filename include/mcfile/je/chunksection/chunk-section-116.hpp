#pragma once

namespace mcfile::je::chunksection {

class ChunkSection116 : public ChunkSection113Base<BlockStatesParser116>
{
public:
    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        vector<uint16_t> paletteIndices;
        vector<uint8_t> blockLight;
        vector<uint8_t> skyLight(2048, 0xff);
        return shared_ptr<ChunkSection116>(
            new ChunkSection116(sectionY,
                                palette,
                                paletteIndices,
                                blockLight,
                                skyLight));
    }

private:
    ChunkSection116(int y,
                    std::vector<std::shared_ptr<Block const>> const& palette,
                    std::vector<uint16_t> const& paletteIndices,
                    std::vector<uint8_t> const& blockLight,
                    std::vector<uint8_t> const& skyLight)
        : ChunkSection113Base<BlockStatesParser116>(y, palette, paletteIndices, blockLight, skyLight)
    {
    }
};

} // namespace mcfile::detail
