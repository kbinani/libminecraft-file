#pragma once

namespace mcfile::je::chunksection {

class ChunkSection116 : public ChunkSection113Base<BlockStatesParser116> {
public:
    static std::shared_ptr<ChunkSection116> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        palette.push_back(make_shared<Block const>("minecraft:air"));
        vector<uint16_t> paletteIndices(4096, 0);
        auto extra = make_shared<nbt::CompoundTag>();
        return shared_ptr<ChunkSection116>(
            new ChunkSection116(sectionY,
                                palette,
                                paletteIndices,
                                extra));
    }

    std::optional<biomes::BiomeId> biomeAt(int offsetX, int offsetY, int offsetZ) const override {
        auto index = BiomeIndex(offsetX, offsetY, offsetZ);
        if (!index) {
            return std::nullopt;
        }
        return fBiomes.get(*index);
    }

    bool setBiomeAt(int offsetX, int offsetY, int offsetZ, biomes::BiomeId biome) override {
        auto index = BiomeIndex(offsetX, offsetY, offsetZ);
        if (!index) {
            return false;
        }
        return fBiomes.set(*index, biome);
    }

    void fill(biomes::BiomeId biome) override {
        fBiomes.fill(biome);
    }

private:
    ChunkSection116(int y,
                    std::vector<std::shared_ptr<Block const>> const &palette,
                    std::vector<uint16_t> const &paletteIndices,
                    std::shared_ptr<nbt::CompoundTag> const &extra)
        : ChunkSection113Base<BlockStatesParser116>(y, palette, paletteIndices, extra) {
    }

    static std::optional<size_t> BiomeIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        int x = offsetX / 4;
        int y = offsetY / 4;
        int z = offsetZ / 4;
        return (y * 4 + z) * 4 + x;
    }

public:
    BiomePalette fBiomes;
};

} // namespace mcfile::je::chunksection
