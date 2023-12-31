#pragma once

namespace mcfile::je::chunksection {

template<class BlockStatesParser>
class ChunkSection113Base : public ChunkSection {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        auto idx = BlockIndex(offsetX, offsetY, offsetZ);
        auto ret = fBlocks.get(idx);
        if (ret) {
            return *ret;
        } else {
            return nullptr;
        }
    }

    std::shared_ptr<Block const> blockAtUnchecked(int offsetX, int offsetY, int offsetZ) const override {
        if (fBlocks.empty()) {
            return nullptr;
        }
        auto idx = BlockIndex(offsetX, offsetY, offsetZ);
        return fBlocks.getUnchecked(idx);
    }

    std::optional<int> blockPaletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        auto idx = BlockIndex(offsetX, offsetY, offsetZ);
        return fBlocks.index(idx);
    }

    bool setBlockAt(int offsetX, int offsetY, int offsetZ, std::shared_ptr<Block const> const &block) override {
        if (!block) {
            return false;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (fBlocks.empty()) {
            auto air = Block::FromId(blocks::minecraft::air, block->fDataVersion);
            fBlocks.set(0, air);
        }
        return fBlocks.set(index, block);
    }

    int y() const override {
        return fY > 251 ? fY - 256 : fY;
    }

    std::optional<biomes::BiomeId> biomeAt(int offsetX, int offsetY, int offsetZ) const override {
        return std::nullopt;
    }

    bool setBiomeAt(int offsetX, int offsetY, int offsetZ, biomes::BiomeId biome) override {
        return false;
    }

    bool setBiomes(biomes::BiomeId biomesXYZ[4][4][4]) override {
        return false;
    }

    void fill(biomes::BiomeId biome) override {
    }

    std::shared_ptr<mcfile::nbt::CompoundTag> toCompoundTag(Dimension d) const override {
        using namespace std;
        using namespace mcfile::nbt;
        auto root = make_shared<CompoundTag>();

        for (auto const &it : *fExtra) {
            root->set(it.first, it.second->clone());
        }

        int8_t i8 = Clamp<int8_t>(fY);
        uint8_t u8 = *(uint8_t *)&i8;
        root->set(u8"Y", make_shared<ByteTag>(u8));

        vector<uint16_t> index;
        vector<shared_ptr<Block const>> palette;
        fBlocks.copy(palette, index);

        if (!index.empty()) {
            vector<int64_t> blockStates;
            BlockStatesParser::BlockStatesFromPaletteIndices(palette.size(), index, blockStates);
            root->set(u8"BlockStates", make_shared<LongArrayTag>(blockStates));
        }

        if (!palette.empty()) {
            auto paletteTag = make_shared<ListTag>(Tag::Type::Compound);
            for (auto p : palette) {
                paletteTag->push_back(p->toCompoundTag());
            }
            root->set(u8"Palette", paletteTag);
        }

        if (fBlockLight.size() == 2048) {
            auto blockLight = make_shared<ByteArrayTag>();
            copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(blockLight->fValue));
            root->set(u8"BlockLight", blockLight);
        }

        if (fSkyLight.size() == 2048) {
            auto skyLight = make_shared<ByteArrayTag>();
            copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(skyLight->fValue));
            root->set(u8"SkyLight", skyLight);
        }

        return root;
    }

    void eachBlockPalette(std::function<bool(std::shared_ptr<Block const> const &, size_t)> visitor) const override {
        fBlocks.eachValue([visitor](std::shared_ptr<Block const> const &v, size_t i) {
            return visitor(v, i);
        });
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const *section, int dataVersion) {
        if (!section) {
            return nullptr;
        }

        static std::unordered_set<std::u8string> const sExclude = {
            u8"Y",
            u8"Palette",
            u8"BlockStates",
            u8"BlockLight",
            u8"SkyLight",
        };
        auto yTag = section->query(u8"Y")->asByte();
        if (!yTag) {
            return nullptr;
        }
        if (15 < yTag->fValue) {
            return nullptr;
        }

        auto paletteTag = section->query(u8"Palette")->asList();
        std::vector<std::shared_ptr<Block const>> palette;
        if (paletteTag) {
            for (auto entry : *paletteTag) {
                auto tag = entry->asCompound();
                if (!tag) {
                    return nullptr;
                }
                auto nameTag = tag->query(u8"Name")->asString();
                if (!nameTag) {
                    return nullptr;
                }
                std::map<std::u8string, std::u8string> properties;
                auto propertiesTag = tag->query(u8"Properties")->asCompound();
                if (propertiesTag) {
                    for (auto p : *propertiesTag) {
                        std::u8string n = p.first;
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
                palette.push_back(Block::FromNameAndProperties(nameTag->fValue, dataVersion, properties));
            }
        }

        auto blockStatesTag = section->query(u8"BlockStates")->asLongArray();
        std::vector<uint16_t> paletteIndices;
        if (blockStatesTag) {
            BlockStatesParser::PaletteIndicesFromBlockStates(blockStatesTag->value(), paletteIndices);
        }

        std::vector<uint8_t> blockLight;
        auto blockLightTag = section->byteArrayTag(u8"BlockLight");
        if (blockLightTag && blockLightTag->fValue.size() == 2048) {
            blockLightTag->fValue.swap(blockLight);
        }

        std::vector<uint8_t> skyLight;
        auto skyLightTag = section->byteArrayTag(u8"SkyLight");
        if (skyLightTag && skyLightTag->fValue.size() == 2048) {
            skyLightTag->fValue.swap(skyLight);
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

    ChunkSection113Base *clone() const override {
        using namespace std;
        unique_ptr<ChunkSection113Base> s(new ChunkSection113Base(fY));
        copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(s->fBlockLight));
        copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(s->fSkyLight));
        s->fBlocks = fBlocks;
        if (fExtra) {
            s->fExtra = fExtra->copy();
        }
        return s.release();
    }

protected:
    ChunkSection113Base(int y,
                        std::vector<std::shared_ptr<Block const>> const &palette,
                        std::vector<uint16_t> const &paletteIndices,
                        std::vector<uint8_t> &blockLight,
                        std::vector<uint8_t> &skyLight,
                        std::shared_ptr<nbt::CompoundTag> const &extra)
        : fY(y)
        , fExtra(extra) {
        fBlocks.reset(palette, paletteIndices);
        fBlockLight.swap(blockLight);
        fSkyLight.swap(skyLight);
    }

    explicit ChunkSection113Base(int y)
        : fY(y) {}

private:
    static size_t BlockIndex(int offsetX, int offsetY, int offsetZ) {
        assert(0 <= offsetX && offsetX < 16 && 0 <= offsetY && offsetY < 16 && 0 <= offsetZ && offsetZ < 16);
        return (offsetY << 8) + (offsetZ << 4) + offsetX;
    }

public:
    int const fY;
    BlockPalette fBlocks;
    std::shared_ptr<nbt::CompoundTag> fExtra;
};

} // namespace mcfile::je::chunksection
