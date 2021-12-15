#pragma once

namespace mcfile::je::chunksection {

template<class BlockStatesParser>
class ChunkSection113Base : public ChunkSection {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (!index) {
            return nullptr;
        }
        auto ret = fBlocks.get(*index);
        if (ret) {
            return *ret;
        } else {
            return nullptr;
        }
    }

    bool setBlockAt(int offsetX, int offsetY, int offsetZ, std::shared_ptr<Block const> const &block) override {
        using namespace std;
        if (!block) {
            return false;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (!index) {
            return false;
        }
        if (fBlocks.empty()) {
            auto air = std::make_shared<Block>("minecraft:air");
            fBlocks.set(0, air);
        }
        return fBlocks.set(*index, block);
    }

    uint8_t blockLightAt(int offsetX, int offsetY, int offsetZ) const override {
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (!index) {
            return 0;
        }
        int const bitIndex = *index * 4;
        int const byteIndex = bitIndex / 8;
        if (fBlockLight.size() <= byteIndex) {
            return 0;
        }
        int const bitOffset = bitIndex - 8 * byteIndex;
        uint8_t const v = fBlockLight[byteIndex];
        return (v >> bitOffset) & 0xF;
    }

    uint8_t skyLightAt(int offsetX, int offsetY, int offsetZ) const override {
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (!index) {
            return 0;
        }
        int const bitIndex = *index * 4;
        int const byteIndex = bitIndex / 8;
        if (fSkyLight.size() <= byteIndex) {
            return 0;
        }
        int const bitOffset = bitIndex - 8 * byteIndex;
        uint8_t const v = fSkyLight[byteIndex];
        return (v >> bitOffset) & 0xF;
    }

    int y() const override {
        return fY > 251 ? fY - 256 : fY;
    }

    int rawY() const override {
        return fY;
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

    std::shared_ptr<mcfile::nbt::CompoundTag> toCompoundTag() const override {
        using namespace std;
        using namespace mcfile::nbt;
        auto root = make_shared<CompoundTag>();

        for (auto const &it : *fExtra) {
            root->set(it.first, it.second->clone());
        }

        root->set("Y", make_shared<ByteTag>(fY));

        if (!fBlockLight.empty()) {
            vector<uint8_t> buf;
            copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(buf));
            root->set("BlockLight", make_shared<ByteArrayTag>(buf));
        }

        if (!fSkyLight.empty()) {
            vector<uint8_t> buf;
            copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(buf));
            root->set("SkyLight", make_shared<ByteArrayTag>(buf));
        }

        vector<uint16_t> index;
        vector<shared_ptr<Block const>> palette;
        fBlocks.copy(palette, index);

        if (!index.empty()) {
            vector<int64_t> blockStates;
            BlockStatesParser::BlockStatesFromPaletteIndices(palette.size(), index, blockStates);
            root->set("BlockStates", make_shared<LongArrayTag>(blockStates));
        }

        if (!palette.empty()) {
            auto paletteTag = make_shared<ListTag>(Tag::Type::Compound);
            for (auto p : palette) {
                paletteTag->push_back(p->toCompoundTag());
            }
            root->set("Palette", paletteTag);
        }

        return root;
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const *section) {
        if (!section) {
            return nullptr;
        }

        static std::unordered_set<std::string> const sExclude = {
            "Y",
            "Palette",
            "BlockStates",
            "BlockLight",
            "SkyLight",
        };
        auto yTag = section->query("Y")->asByte();
        if (!yTag) {
            return nullptr;
        }
        if (15 < yTag->fValue) {
            return nullptr;
        }

        auto paletteTag = section->query("Palette")->asList();
        std::vector<std::shared_ptr<Block const>> palette;
        if (paletteTag) {
            for (auto entry : *paletteTag) {
                auto tag = entry->asCompound();
                if (!tag) {
                    return nullptr;
                }
                auto nameTag = tag->query("Name")->asString();
                if (!nameTag) {
                    return nullptr;
                }
                std::map<std::string, std::string> properties;
                auto propertiesTag = tag->query("Properties")->asCompound();
                if (propertiesTag) {
                    for (auto p : *propertiesTag) {
                        std::string n = p.first;
                        if (n.empty()) {
                            continue;
                        }
                        auto v = p.second->asString();
                        if (!v) {
                            continue;
                        }
                        properties.insert(std::make_pair(n, v->fValue));
                    }
                }
                palette.push_back(std::make_shared<Block const>(nameTag->fValue, properties));
            }
        }

        auto blockStatesTag = section->query("BlockStates")->asLongArray();
        std::vector<uint16_t> paletteIndices;
        if (blockStatesTag) {
            BlockStatesParser::PaletteIndicesFromBlockStates(blockStatesTag->value(), paletteIndices);
        }

        std::vector<uint8_t> blockLight;
        auto blockLightTag = section->query("BlockLight")->asByteArray();
        if (blockLightTag) {
            blockLight = blockLightTag->value();
        }

        std::vector<uint8_t> skyLight;
        auto skyLightTag = section->query("SkyLight")->asByteArray();
        if (skyLightTag) {
            skyLight = skyLightTag->value();
        }

        auto extra = std::make_shared<nbt::CompoundTag>();
        for (auto it : *section) {
            if (sExclude.find(it.first) == sExclude.end()) {
                extra->set(it.first, it.second->clone());
            }
        }

        return std::shared_ptr<ChunkSection>(new ChunkSection113Base((int)yTag->fValue,
                                                                     palette,
                                                                     paletteIndices,
                                                                     blockLight,
                                                                     skyLight,
                                                                     extra));
    }

protected:
    ChunkSection113Base(int y,
                        std::vector<std::shared_ptr<Block const>> const &palette,
                        std::vector<uint16_t> const &paletteIndices,
                        std::vector<uint8_t> const &blockLight,
                        std::vector<uint8_t> const &skyLight,
                        std::shared_ptr<nbt::CompoundTag> const &extra)
        : fY(y)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight)
        , fExtra(extra) {
        fBlocks.reset(palette, paletteIndices);
    }

private:
    static std::optional<size_t> BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        return offsetY * 16 * 16 + offsetZ * 16 + offsetX;
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
    int const fY;
    BlockPalette fBlocks;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
    std::shared_ptr<nbt::CompoundTag> fExtra;
    BiomePalette fBiomes;
};

} // namespace mcfile::je::chunksection
