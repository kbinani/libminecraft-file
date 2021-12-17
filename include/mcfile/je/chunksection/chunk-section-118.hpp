#pragma once

namespace mcfile::je::chunksection {

class ChunkSection118 : public ChunkSection {
public:
    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> blockPalette;
        vector<uint16_t> blockPaletteIndices;
        vector<biomes::BiomeId> biomePalette;
        vector<uint16_t> biomePaletteIndices;
        auto extra = make_shared<nbt::CompoundTag>();
        return shared_ptr<ChunkSection>(new ChunkSection118(
            sectionY,
            blockPalette,
            blockPaletteIndices,
            biomePalette,
            biomePaletteIndices,
            2858,
            extra));
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const *section, int dataVersion) {
        using namespace std;
        if (!section) {
            return nullptr;
        }

        static std::unordered_set<std::string> const sExclude = {
            "Y",
            "block_states",
            "biomes",
        };

        auto yTag = section->byte("Y");
        if (!yTag) {
            return nullptr;
        }
        int y = *yTag;

        auto blockStates = section->compoundTag("block_states");
        vector<shared_ptr<Block const>> blockPalette;
        vector<uint16_t> blockPaletteIndices;
        if (blockStates) {
            auto blockPaletteTag = blockStates->listTag("palette");
            if (!blockPaletteTag) {
                return nullptr;
            }
            for (auto entry : *blockPaletteTag) {
                auto tag = entry->asCompound();
                if (!tag) {
                    return nullptr;
                }
                auto name = tag->string("Name");
                if (!name) {
                    return nullptr;
                }
                map<string, string> properties;
                auto propertiesTag = tag->compoundTag("Properties");
                if (propertiesTag) {
                    for (auto p : *propertiesTag) {
                        string n = p.first;
                        if (n.empty()) {
                            continue;
                        }
                        auto v = p.second->asString();
                        if (!v) {
                            continue;
                        }
                        properties.insert(make_pair(n, v->fValue));
                    }
                }
                blockPalette.push_back(make_shared<Block const>(*name, properties));
            }
            if (blockPalette.empty()) {
                return nullptr;
            }

            if (blockPalette.size() == 1) {
                blockPaletteIndices.resize(4096, 0);
            } else {
                auto blockData = blockStates->longArrayTag("data");
                if (!blockData) {
                    return nullptr;
                }
                BlockStatesParser116::PaletteIndicesFromBlockStates(blockData->value(), blockPaletteIndices);
            }
        }

        auto biomes = section->compoundTag("biomes");
        vector<mcfile::biomes::BiomeId> biomePalette;
        vector<uint16_t> biomePaletteIndices;
        if (biomes) {
            auto biomePaletteTag = biomes->listTag("palette");
            if (!biomePaletteTag) {
                return nullptr;
            }
            for (auto entry : *biomePaletteTag) {
                auto tag = entry->asString();
                if (!tag) {
                    return nullptr;
                }
                auto b = biomes::FromName(tag->fValue);
                if (!b) {
                    return nullptr;
                }
                biomePalette.push_back(*b);
            }
            if (biomePalette.empty()) {
                return nullptr;
            }

            if (biomePalette.size() == 1) {
                biomePaletteIndices.resize(4 * 4 * 4, 0);
            } else {
                auto biomeData = biomes->longArrayTag("data");
                if (!biomeData) {
                    return nullptr;
                }
                BlockStatesParser116::GenericPaletteIndicesFromBlockStates<4>(biomeData->value(), biomePaletteIndices);
            }
        }

        auto extra = std::make_shared<nbt::CompoundTag>();
        for (auto it : *section) {
            if (sExclude.find(it.first) == sExclude.end()) {
                extra->set(it.first, it.second->clone());
            }
        }

        return std::shared_ptr<ChunkSection>(new ChunkSection118(
            y,
            blockPalette,
            blockPaletteIndices,
            biomePalette,
            biomePaletteIndices,
            dataVersion,
            extra));
    }

    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        auto const index = BlockIndex(offsetX, offsetY, offsetZ);
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

    int y() const override {
        return fY;
    }

    int rawY() const override {
        return fY;
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
        return fBlocks.set(*index, block);
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

        if (!fBiomes.empty()) {
            auto biomes = make_shared<CompoundTag>();
            auto biomePalette = make_shared<ListTag>(Tag::Type::String);
            shared_ptr<LongArrayTag> biomePaletteData;
            vector<biomes::BiomeId> palette;
            vector<uint16_t> index;
            fBiomes.copy(palette, index);
            BiomePalette::ShrinkToFit(palette, index);
            PackPalette<biomes::BiomeId, shared_ptr<StringTag>>(
                palette, index,
                [this](biomes::BiomeId biome) -> shared_ptr<nbt::StringTag> {
                    auto name = biomes::Name(biome, fDataVersion);
                    return make_shared<StringTag>(name);
                },
                biomePalette, biomePaletteData);
            biomes->set("palette", biomePalette);
            if (biomePaletteData) {
                biomes->set("data", biomePaletteData);
            }
            root->set("biomes", biomes);
        }

        if (!fBlocks.empty()) {
            auto blockStates = make_shared<CompoundTag>();
            auto blockPalette = make_shared<ListTag>(Tag::Type::Compound);
            shared_ptr<LongArrayTag> blockPaletteData;
            vector<shared_ptr<Block const>> palette;
            vector<uint16_t> index;
            fBlocks.copy(palette, index);
            BlockPalette::ShrinkToFit(palette, index);
            PackPalette<shared_ptr<Block const>, shared_ptr<CompoundTag>>(
                palette, index,
                [this](shared_ptr<Block const> const &block) -> shared_ptr<nbt::CompoundTag> {
                    auto tag = make_shared<CompoundTag>();
                    tag->set("Name", make_shared<StringTag>(block->fName));
                    auto props = make_shared<CompoundTag>();
                    for (auto it : block->fProperties) {
                        props->set(it.first, make_shared<StringTag>(it.second));
                    }
                    tag->set("Properties", props);
                    return tag;
                },
                blockPalette, blockPaletteData);
            blockStates->set("palette", blockPalette);
            if (blockPaletteData) {
                blockStates->set("data", blockPaletteData);
            }
            root->set("block_states", blockStates);
        }

        return root;
    }

    void eachBlockPalette(std::function<bool(Block const &)> visitor) const override {
        fBlocks.eachValue([visitor](std::shared_ptr<Block const> const &v) {
            if (!v) {
                return true;
            }
            return visitor(*v);
        });
    }

    template<class ValueType, class PaletteType>
    static void PackPalette(std::vector<ValueType> const &inPalette,
                            std::vector<uint16_t> const &inIndices,
                            std::function<PaletteType(ValueType v)> valueToPalette,
                            std::shared_ptr<nbt::ListTag> &outPalette,
                            std::shared_ptr<nbt::LongArrayTag> &outPackedIndices) {
        using namespace std;
        using namespace mcfile::nbt;

        for (ValueType const &value : inPalette) {
            outPalette->push_back(valueToPalette(value));
        }
        if (outPalette->size() <= 1) {
            return;
        }
        vector<int64_t> packedData;
        BlockStatesParser116::BlockStatesFromPaletteIndices(outPalette->size(), inIndices, packedData);
        outPackedIndices.reset(new LongArrayTag(packedData));
    }

private:
    ChunkSection118(int y,
                    std::vector<std::shared_ptr<Block const>> const &blockPalette,
                    std::vector<uint16_t> const &blockPaletteIndices,
                    std::vector<mcfile::biomes::BiomeId> const &biomePalette,
                    std::vector<uint16_t> const &biomePaletteIndices,
                    int dataVersion,
                    std::shared_ptr<nbt::CompoundTag> extra)
        : fY(y)
        , fDataVersion(dataVersion)
        , fExtra(extra) {
        fBlocks.reset(blockPalette, blockPaletteIndices);
        fBiomes.reset(biomePalette, biomePaletteIndices);
    }

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

private:
    int const fY;
    BlockPalette fBlocks;
    BiomePalette fBiomes;
    int fDataVersion;
    std::shared_ptr<nbt::CompoundTag> fExtra;
};

} // namespace mcfile::je::chunksection
