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
        return fBiomes.get(index);
    }

    bool setBiomeAt(int offsetX, int offsetY, int offsetZ, biomes::BiomeId biome) override {
        auto index = BiomeIndex(offsetX, offsetY, offsetZ);
        return fBiomes.set(index, biome);
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

    static size_t BiomeIndex(int offsetX, int offsetY, int offsetZ) {
        assert(0 <= offsetX && offsetX < 16 && 0 <= offsetY && offsetY < 16 && 0 <= offsetZ && offsetZ < 16);
        int x = offsetX >> 2;
        int y = offsetY >> 2;
        int z = offsetZ >> 2;
        return (y << 4) + (z << 2) + x;
    }

public:
    BiomePalette fBiomes;
};

} // namespace mcfile::je::chunksection
