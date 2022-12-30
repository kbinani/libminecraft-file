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

    bool setBiomes(biomes::BiomeId biomesXYZ[4][4][4]) override {
        using namespace std;
        for (int y = 0; y < 4; y++) {
            for (int z = 0; z < 4; z++) {
                for (int x = 0; x < 4; x++) {
                    if (biomesXYZ[x][y][z] >= biomes::minecraft::minecraft_max_biome_id) {
                        return false;
                    }
                }
            }
        }
        vector<biomes::BiomeId> palette;
        vector<uint16_t> index(64);
        int8_t lookup[biomes::minecraft::minecraft_max_biome_id];
        fill_n(lookup, biomes::minecraft::minecraft_max_biome_id, -1);
        int k = 0;
        for (int y = 0; y < 4; y++) {
            for (int z = 0; z < 4; z++) {
                for (int x = 0; x < 4; x++, k++) {
                    biomes::BiomeId id = biomesXYZ[x][y][z];
                    int8_t i = lookup[id];
                    if (i < 0) {
                        i = (int8_t)palette.size();
                        lookup[id] = i;
                        palette.push_back(id);
                    }
                    index[k] = i;
                }
            }
        }
        return fBiomes.reset(palette, index);
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
