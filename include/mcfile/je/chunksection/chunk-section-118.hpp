#pragma once

namespace mcfile::je::chunksection {

class ChunkSection118 : public ChunkSection {
public:
    static std::shared_ptr<ChunkSection118> MakeEmpty(int sectionY, int dataVersion) {
        using namespace std;
        vector<shared_ptr<Block const>> blockPalette;
        blockPalette.push_back(Block::FromId(blocks::minecraft::air, dataVersion));
        vector<uint16_t> blockPaletteIndices(4096, 0);
        vector<biomes::BiomeId> biomePalette;
        vector<uint16_t> biomePaletteIndices;
        vector<uint8_t> blockLight;
        vector<uint8_t> skyLight;
        auto extra = make_shared<nbt::CompoundTag>();
        return shared_ptr<ChunkSection118>(new ChunkSection118(
            sectionY,
            blockPalette,
            blockPaletteIndices,
            biomePalette,
            biomePaletteIndices,
            2858,
            blockLight,
            skyLight,
            extra));
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const *section, int dataVersion) {
        using namespace std;
        if (!section) {
            return nullptr;
        }

        static std::unordered_set<std::u8string> const sExclude = {
            u8"Y",
            u8"block_states",
            u8"biomes",
            u8"BlockLight",
            u8"SkyLight",
        };

        auto yTag = section->byte(u8"Y");
        if (!yTag) {
            return nullptr;
        }
        int y = *yTag;

        auto blockStates = section->compoundTag(u8"block_states");
        vector<shared_ptr<Block const>> blockPalette;
        vector<uint16_t> blockPaletteIndices;
        if (blockStates) {
            auto blockPaletteTag = blockStates->listTag(u8"palette");
            if (!blockPaletteTag) {
                return nullptr;
            }
            for (auto entry : *blockPaletteTag) {
                auto tag = entry->asCompound();
                if (!tag) {
                    return nullptr;
                }
                auto name = tag->string(u8"Name");
                if (!name) {
                    return nullptr;
                }
                map<u8string, u8string> properties;
                auto propertiesTag = tag->compoundTag(u8"Properties");
                if (propertiesTag) {
                    for (auto p : *propertiesTag) {
                        u8string n = p.first;
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
                blockPalette.push_back(Block::FromNameAndProperties(*name, dataVersion, properties));
            }
            if (blockPalette.empty()) {
                return nullptr;
            }

            if (blockPalette.size() == 1) {
                blockPaletteIndices.resize(4096, 0);
            } else {
                auto blockData = blockStates->longArrayTag(u8"data");
                if (!blockData) {
                    return nullptr;
                }
                BlockStatesParser116::PaletteIndicesFromBlockStates(blockData->value(), blockPaletteIndices);
            }
        }

        auto biomes = section->compoundTag(u8"biomes");
        vector<mcfile::biomes::BiomeId> biomePalette;
        vector<uint16_t> biomePaletteIndices;
        if (biomes) {
            auto biomePaletteTag = biomes->listTag(u8"palette");
            if (!biomePaletteTag) {
                return nullptr;
            }
            for (auto entry : *biomePaletteTag) {
                auto tag = entry->asString();
                if (!tag) {
                    return nullptr;
                }
                auto b = biomes::Biome::FromName(tag->fValue);
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
                auto biomeData = biomes->longArrayTag(u8"data");
                if (!biomeData) {
                    return nullptr;
                }
                BlockStatesParser116::GenericPaletteIndicesFromBlockStates<4>(biomeData->value(), biomePaletteIndices);
            }
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

        return std::shared_ptr<ChunkSection>(new ChunkSection118(
            y,
            blockPalette,
            blockPaletteIndices,
            biomePalette,
            biomePaletteIndices,
            dataVersion,
            blockLight,
            skyLight,
            extra));
    }

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

    int y() const override {
        return fY;
    }

    bool setBlockAt(int offsetX, int offsetY, int offsetZ, std::shared_ptr<Block const> const &block) override {
        if (!block) {
            return false;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        return fBlocks.set(index, block);
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

    std::shared_ptr<mcfile::nbt::CompoundTag> toCompoundTag() const override {
        using namespace std;
        using namespace mcfile::nbt;

        auto root = make_shared<CompoundTag>();

        for (auto const &it : *fExtra) {
            root->set(it.first, it.second->clone());
        }

        int8_t i8 = Clamp<int8_t>(fY);
        uint8_t u8 = *(uint8_t *)&i8;
        root->set(u8"Y", make_shared<ByteTag>(u8));

        if (!fBiomes.empty()) {
            auto biomes = make_shared<CompoundTag>();
            auto biomePalette = make_shared<ListTag>(Tag::Type::String);
            shared_ptr<LongArrayTag> biomePaletteData;
            vector<biomes::BiomeId> palette;
            vector<uint16_t> index;
            fBiomes.copy(palette, index);
            BiomePalette::ShrinkToFit(palette, index);
            PackBiomePalette(palette, index, biomePalette, biomePaletteData, fDataVersion);
            biomes->set(u8"palette", biomePalette);
            if (biomePaletteData) {
                biomes->set(u8"data", biomePaletteData);
            }
            root->set(u8"biomes", biomes);
        }

        if (!fBlocks.empty()) {
            auto blockStates = make_shared<CompoundTag>();
            auto blockPalette = make_shared<ListTag>(Tag::Type::Compound);
            shared_ptr<LongArrayTag> blockPaletteData;
            vector<shared_ptr<Block const>> palette;
            vector<uint16_t> index;
            fBlocks.copy(palette, index);
            BlockPalette::ShrinkToFit(palette, index);
            PackBlockPalette(palette, index, blockPalette, blockPaletteData);
            blockStates->set(u8"palette", blockPalette);
            if (blockPaletteData) {
                blockStates->set(u8"data", blockPaletteData);
            }
            root->set(u8"block_states", blockStates);
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

    static void PackBlockPalette(std::vector<std::shared_ptr<Block const>> const &inPalette,
                                 std::vector<uint16_t> const &inIndices,
                                 std::shared_ptr<nbt::ListTag> &outPalette,
                                 std::shared_ptr<nbt::LongArrayTag> &outPackedIndices) {
        using namespace std;
        using namespace mcfile::nbt;

        for (auto const &value : inPalette) {
            outPalette->push_back(value->toCompoundTag());
        }
        if (outPalette->size() <= 1) {
            return;
        }
        vector<int64_t> packedData;
        BlockStatesParser116::BlockStatesFromPaletteIndices(outPalette->size(), inIndices, packedData);
        outPackedIndices.reset(new LongArrayTag(packedData));
    }

    static void PackBiomePalette(std::vector<biomes::BiomeId> const &inPalette,
                                 std::vector<uint16_t> const &inIndices,
                                 std::shared_ptr<nbt::ListTag> &outPalette,
                                 std::shared_ptr<nbt::LongArrayTag> &outPackedIndices,
                                 int dataVersion) {
        for (auto const &biome : inPalette) {
            auto name = biomes::Biome::Name(biome, dataVersion);
            outPalette->push_back(std::make_shared<mcfile::nbt::StringTag>(name));
        }
        size_t size = outPalette->size();
        if (size <= 1) {
            return;
        }

        size_t bitsPerIndex = (size_t)ceil(log2(size));
        std::vector<bool> bitset;
        bitset.reserve(64);
        std::vector<int64_t> packedData;
        for (size_t i = 0; i < inIndices.size(); i++) {
            uint16_t v = inIndices[i];
            if (bitset.size() + bitsPerIndex > 64) {
                uint64_t u = 0;
                for (size_t j = 0; j < bitset.size(); j++) {
                    uint64_t t = bitset[j] ? 1 : 0;
                    u |= t << j;
                }
                packedData.push_back(*(int64_t *)&u);
                bitset.clear();
            }
            for (int j = 0; j < bitsPerIndex; j++) {
                bitset.push_back((v & 1) == 1);
                v >>= 1;
            }
        }
        if (!bitset.empty()) {
            uint64_t u = 0;
            for (size_t j = 0; j < 64 && j < bitset.size(); j++) {
                uint64_t t = bitset[j] ? 1 : 0;
                u |= t << j;
            }
            packedData.push_back(*(int64_t *)&u);
        }
        outPackedIndices.reset(new nbt::LongArrayTag(packedData));
    }

    static size_t BlockIndex(int offsetX, int offsetY, int offsetZ) {
        assert(0 <= offsetX && offsetX < 16 && 0 <= offsetY && offsetY < 16 && 0 <= offsetZ && offsetZ < 16);
        return (offsetY << 8) + (offsetZ << 4) + offsetX;
    }

    static size_t BiomeIndex(int offsetX, int offsetY, int offsetZ) {
        assert(0 <= offsetX && offsetX < 16 && 0 <= offsetY && offsetY < 16 && 0 <= offsetZ && offsetZ < 16);
        int x = offsetX >> 2;
        int y = offsetY >> 2;
        int z = offsetZ >> 2;
        return (y << 4) + (z << 2) + x;
    }

    ChunkSection118 *clone() const override {
        using namespace std;
        unique_ptr<ChunkSection118> s(new ChunkSection118(fY, fDataVersion));
        copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(s->fBlockLight));
        copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(s->fSkyLight));
        s->fBlocks = fBlocks;
        if (fExtra) {
            s->fExtra = fExtra->copy();
        }
        s->fBiomes = fBiomes;
        return s.release();
    }

private:
    ChunkSection118(int y,
                    std::vector<std::shared_ptr<Block const>> const &blockPalette,
                    std::vector<uint16_t> const &blockPaletteIndices,
                    std::vector<mcfile::biomes::BiomeId> const &biomePalette,
                    std::vector<uint16_t> const &biomePaletteIndices,
                    int dataVersion,
                    std::vector<uint8_t> &blockLight,
                    std::vector<uint8_t> &skyLight,
                    std::shared_ptr<nbt::CompoundTag> extra)
        : fY(y)
        , fDataVersion(dataVersion)
        , fExtra(extra) {
        fBlockLight.swap(blockLight);
        fSkyLight.swap(skyLight);
        fBlocks.reset(blockPalette, blockPaletteIndices);
        fBiomes.reset(biomePalette, biomePaletteIndices);
    }

    ChunkSection118(int y, int dataVersion)
        : fY(y)
        , fDataVersion(dataVersion) {}

public:
    int const fY;
    BlockPalette fBlocks;
    BiomePalette fBiomes;
    int fDataVersion;
    std::shared_ptr<nbt::CompoundTag> fExtra;
};

} // namespace mcfile::je::chunksection
