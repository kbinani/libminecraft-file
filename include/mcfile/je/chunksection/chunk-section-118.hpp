#pragma once

namespace mcfile::je::chunksection {

class ChunkSection118 : public ChunkSection {
public:
    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        return nullptr;
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const *section, int dataVersion) {
        using namespace std;
        if (!section) {
            return nullptr;
        }
        auto yTag = section->byte("Y");
        if (!yTag) {
            return nullptr;
        }
        int y = *yTag;

        auto blockStates = section->compoundTag("block_states");
        if (!blockStates) {
            return nullptr;
        }
        auto blockPaletteTag = blockStates->listTag("palette");
        if (!blockPaletteTag) {
            return nullptr;
        }
        vector<shared_ptr<Block const>> blockPalette;
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

        vector<uint16_t> blockPaletteIndices;
        if (blockPalette.size() == 1) {
            blockPaletteIndices.resize(4096, 0);
        } else {
            auto blockData = blockStates->longArrayTag("data");
            if (!blockData) {
                return nullptr;
            }
            BlockStatesParser116::PaletteIndicesFromBlockStates(blockData->value(), blockPaletteIndices);
        }

        auto biomes = section->compoundTag("biomes");
        if (!biomes) {
            return nullptr;
        }
        auto biomePaletteTag = biomes->listTag("palette");
        if (!biomePaletteTag) {
            return nullptr;
        }
        vector<mcfile::biomes::BiomeId> biomePalette;
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

        vector<uint16_t> biomePaletteIndices;
        if (biomePalette.size() == 1) {
            biomePaletteIndices.resize(4 * 4 * 4, 0);
        } else {
            auto biomeData = biomes->longArrayTag("data");
            if (!biomeData) {
                return nullptr;
            }
            BlockStatesParser116::GenericPaletteIndicesFromBlockStates<4>(biomeData->value(), biomePaletteIndices);
        }

        vector<uint8_t> blockLight;
        auto blockLightTag = section->query("BlockLight")->asByteArray();
        if (blockLightTag) {
            blockLight = blockLightTag->value();
        }

        vector<uint8_t> skyLight;
        auto skyLightTag = section->query("SkyLight")->asByteArray();
        if (skyLightTag) {
            skyLight = skyLightTag->value();
        }

        return std::shared_ptr<ChunkSection>(new ChunkSection118(
            y,
            blockPalette,
            blockPaletteIndices,
            biomePalette,
            biomePaletteIndices,
            blockLight,
            skyLight,
            dataVersion));
    }

    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        auto const index = paletteIndexAt(offsetX, offsetY, offsetZ);
        if (!index) {
            return nullptr;
        }
        return fBlockPalette[*index];
    }

    uint8_t blockLightAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return 0;
        }
        int const bitIndex = index * 4;
        int const byteIndex = bitIndex / 8;
        if (fBlockLight.size() <= byteIndex) {
            return 0;
        }
        int const bitOffset = bitIndex - 8 * byteIndex;
        uint8_t const v = fBlockLight[byteIndex];
        return (v >> bitOffset) & 0xF;
    }

    uint8_t skyLightAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return 0;
        }
        int const bitIndex = index * 4;
        int const byteIndex = bitIndex / 8;
        if (fSkyLight.size() <= byteIndex) {
            return 0;
        }
        int const bitOffset = bitIndex - 8 * byteIndex;
        uint8_t const v = fSkyLight[byteIndex];
        return (v >> bitOffset) & 0xF;
    }

    blocks::BlockId blockIdAt(int offsetX, int offsetY, int offsetZ) const override {
        auto const &block = blockAt(offsetX, offsetY, offsetZ);
        if (!block) {
            return blocks::minecraft::air;
        }
        return block->fId;
    }

    int y() const override {
        return fY;
    }

    int rawY() const override {
        return fY;
    }

    std::vector<std::shared_ptr<Block const>> const &palette() const override {
        return fBlockPalette;
    }

    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0 || fBlockPaletteIndices.size() <= index) {
            return std::nullopt;
        }

        int paletteIndex = fBlockPaletteIndices[index];
        if (paletteIndex < 0 || fBlockPalette.size() <= paletteIndex) {
            return std::nullopt;
        }

        return (size_t)paletteIndex;
    }

    bool setBlockAt(int offsetX, int offsetY, int offsetZ, std::shared_ptr<Block const> const &block) override {
        using namespace std;
        if (!block) {
            return false;
        }
        int index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return false;
        }
        if (fBlockPaletteIndices.size() != 4096) {
            fBlockPaletteIndices.resize(4096);
            auto air = make_shared<Block>("minecraft:air");
            fBlockPalette.push_back(air);
        }
        int idx = -1;
        string s = block->toString();
        for (int i = 0; i < fBlockPalette.size(); i++) {
            if (s == fBlockPalette[i]->toString()) {
                idx = i;
                break;
            }
        }
        if (idx < 0) {
            idx = fBlockPalette.size();
            fBlockPalette.push_back(block);
        }
        fBlockPaletteIndices[index] = idx;
        return true;
    }

    std::optional<biomes::BiomeId> biomeAt(int offsetX, int offsetY, int offsetZ) const override {
        using namespace std;
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return nullopt;
        }
        int x = offsetX / 4;
        int y = offsetY / 4;
        int z = offsetZ / 4;
        int index = (y * 4 + z) * 4 + x;
        if (fBiomePaletteIndices.size() <= index) {
            return nullopt;
        }
        return fBiomePaletteIndices[index];
    }

    bool setBiomeAt(int offsetX, int offsetY, int offsetZ, biomes::BiomeId biome) override {
        using namespace std;
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return false;
        }
        int x = offsetX / 4;
        int y = offsetY / 4;
        int z = offsetZ / 4;
        int index = (y * 4 + z) * 4 + x;
        if (fBiomePaletteIndices.size() <= index) {
            return false;
        }
        int idx = -1;
        for (int i = 0; i < fBiomePalette.size(); i++) {
            if (fBiomePalette[i] == biome) {
                idx = i;
                break;
            }
        }
        if (idx < 0) {
            idx = fBiomePalette.size();
            fBiomePalette.push_back(biome);
        }
        fBiomePaletteIndices[index] = idx;
        return true;
    }

    std::shared_ptr<mcfile::nbt::CompoundTag> toCompoundTag() const override {
        using namespace std;
        using namespace mcfile::nbt;

        auto root = make_shared<CompoundTag>();

        root->set("Y", make_shared<ByteTag>(fY));
        if (!fSkyLight.empty()) {
            vector<uint8_t> copy = fSkyLight;
            root->set("SkyLight", make_shared<ByteArrayTag>(copy));
        }
        if (!fBlockLight.empty()) {
            vector<uint8_t> copy = fBlockLight;
            root->set("BlockLight", make_shared<ByteArrayTag>(copy));
        }

        auto biomes = make_shared<CompoundTag>();
        auto biomePalette = make_shared<ListTag>(Tag::Type::String);
        shared_ptr<LongArrayTag> biomePaletteData;
        PackPalette<biomes::BiomeId, biomes::BiomeId, shared_ptr<StringTag>>(
            fBiomePalette, fBiomePaletteIndices,
            [this](biomes::BiomeId biome) -> shared_ptr<nbt::StringTag> {
                auto name = biomes::Name(biome, fDataVersion);
                return make_shared<StringTag>(name);
            },
            [](biomes::BiomeId biome) -> biomes::BiomeId {
                return biome;
            },
            biomePalette, biomePaletteData);
        biomes->set("palette", biomePalette);
        if (biomePaletteData) {
            biomes->set("data", biomePaletteData);
        }
        root->set("biomes", biomes);

        auto blockStates = make_shared<CompoundTag>();
        auto blockPalette = make_shared<ListTag>(Tag::Type::Compound);
        shared_ptr<LongArrayTag> blockPaletteData;
        PackPalette<shared_ptr<Block const>, string, shared_ptr<CompoundTag>>(
            fBlockPalette, fBlockPaletteIndices,
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
            [](shared_ptr<Block const> const &block) -> string {
                return block->toString();
            },
            blockPalette, blockPaletteData);
        blockStates->set("palette", blockPalette);
        if (blockPaletteData) {
            blockStates->set("data", blockPaletteData);
        }
        root->set("block_sates", blockStates);

        return root;
    }

    template<class ValueType, class ValueHashType, class PaletteType>
    static void PackPalette(std::vector<ValueType> const &inPalette,
                            std::vector<uint16_t> const &inIndices,
                            std::function<PaletteType(ValueType v)> valueToPalette,
                            std::function<ValueHashType(ValueType)> valueToHash,
                            std::shared_ptr<nbt::ListTag> &outPalette,
                            std::shared_ptr<nbt::LongArrayTag> &outPackedIndices) {
        using namespace std;
        using namespace mcfile::nbt;

        vector<ValueHashType> paletteValues;
        vector<uint16_t> indices;

        for (uint16_t index : inIndices) {
            ValueType value = inPalette[index];
            ValueHashType hash = valueToHash(value);
            auto found = find(paletteValues.begin(), paletteValues.end(), hash);
            uint16_t idx;
            if (found == paletteValues.end()) {
                idx = outPalette->size();
                paletteValues.push_back(hash);
                outPalette->push_back(valueToPalette(value));
            } else {
                idx = distance(paletteValues.begin(), found);
            }
            indices.push_back(idx);
        }
        if (outPalette->size() <= 1) {
            return;
        }
        vector<int64_t> packedData;
        BlockStatesParser116::BlockStatesFromPaletteIndices(outPalette->size(), indices, packedData);
        outPackedIndices.reset(new LongArrayTag(packedData));
    }

private:
    ChunkSection118(int y,
                    std::vector<std::shared_ptr<Block const>> const &blockPalette,
                    std::vector<uint16_t> const &blockPaletteIndices,
                    std::vector<mcfile::biomes::BiomeId> const &biomePalette,
                    std::vector<uint16_t> const &biomePaletteIndices,
                    std::vector<uint8_t> const &blockLight,
                    std::vector<uint8_t> const &skyLight,
                    int dataVersion)
        : fY(y)
        , fBlockPalette(blockPalette)
        , fBlockPaletteIndices(blockPaletteIndices)
        , fBiomePalette(biomePalette)
        , fBiomePaletteIndices(biomePaletteIndices)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight)
        , fDataVersion(dataVersion) {}

    static int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return offsetY * 16 * 16 + offsetZ * 16 + offsetX;
    }

private:
    int const fY;
    std::vector<std::shared_ptr<Block const>> fBlockPalette;
    std::vector<uint16_t> fBlockPaletteIndices;
    std::vector<mcfile::biomes::BiomeId> fBiomePalette;
    std::vector<uint16_t> fBiomePaletteIndices;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
    int fDataVersion;
};

} // namespace mcfile::je::chunksection
