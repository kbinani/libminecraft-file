#pragma once

namespace mcfile::je {

class ChunkSectionEmpty : public ChunkSection {
public:
    explicit ChunkSectionEmpty(int y)
        : fY(y) {
    }

    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        return nullptr;
    }

    virtual std::shared_ptr<Block const> blockAtUnchecked(int offsetX, int offsetY, int offsetZ) const override {
        return nullptr;
    }

    int y() const override {
        return fY;
    }

    std::optional<biomes::BiomeId> biomeAt(int offsetX, int offsetY, int offsetZ) const override {
        return std::nullopt;
    }

    bool setBiomeAt(int offsetX, int offsetY, int offsetZ, biomes::BiomeId biome) override {
        return true;
    }

    bool setBiomes(biomes::BiomeId biomesXYZ[4][4][4]) override {
        return true;
    }

    void fill(mcfile::biomes::BiomeId biome) override {}

    std::optional<int> blockPaletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        return std::nullopt;
    }

    void eachBlockPalette(std::function<bool(std::shared_ptr<Block const> const &block, size_t index)> visitor) const override {
    }

    bool setBlockAt(int offsetX, int offsetY, int offsetZ, std::shared_ptr<Block const> const &block) override {
        return true;
    }

    std::shared_ptr<mcfile::nbt::CompoundTag> toCompoundTag(Dimension d, int dataVersion) const override {
        auto ret = std::make_shared<nbt::CompoundTag>();

        int8_t h = fY;
        ret->set(u8"Y", std::make_shared<nbt::ByteTag>(*(uint8_t *)&h));

        if (fSkyLight.size() == 2048) {
            auto skyLight = std::make_shared<nbt::ByteArrayTag>();
            copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(skyLight->fValue));
            ret->set(u8"SkyLight", skyLight);
        }

        return ret;
    }

    ChunkSectionEmpty *clone() const override {
        using namespace std;
        auto s = make_unique<ChunkSectionEmpty>(fY);
        copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(s->fBlockLight));
        copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(s->fSkyLight));
        return s.release();
    }

private:
    int const fY;
};

} // namespace mcfile::je
