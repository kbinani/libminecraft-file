#pragma once

namespace mcfile::je::chunksection {

template<class BlockStatesParser>
class ChunkSection113Base : public ChunkSection {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        auto idx = BlockIndex(offsetX, offsetY, offsetZ);
        if (!idx) {
            return nullptr;
        }
        auto ret = fBlocks.get(*idx);
        if (ret) {
            return *ret;
        } else {
            return nullptr;
        }
    }

    std::optional<int> blockPaletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        auto idx = BlockIndex(offsetX, offsetY, offsetZ);
        if (!idx) {
            return std::nullopt;
        }
        return fBlocks.index(*idx);
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

    int y() const override {
        return fY > 251 ? fY - 256 : fY;
    }

    int rawY() const override {
        return fY;
    }

    std::optional<biomes::BiomeId> biomeAt(int offsetX, int offsetY, int offsetZ) const override {
        return std::nullopt;
    }

    bool setBiomeAt(int offsetX, int offsetY, int offsetZ, biomes::BiomeId biome) override {
        return false;
    }

    void fill(biomes::BiomeId biome) override {
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
        root->set("Y", make_shared<ByteTag>(u8));

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

    void eachBlockPalette(std::function<bool(Block const &)> visitor) const override {
        fBlocks.eachValue([visitor](std::shared_ptr<Block const> const &v) {
            if (!v) {
                return true;
            }
            return visitor(*v);
        });
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const *section) {
        if (!section) {
            return nullptr;
        }

        static std::unordered_set<std::string> const sExclude = {
            "Y",
            "Palette",
            "BlockStates",
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

        auto extra = std::make_shared<nbt::CompoundTag>();
        for (auto it : *section) {
            if (sExclude.find(it.first) == sExclude.end()) {
                extra->set(it.first, it.second->clone());
            }
        }

        return std::shared_ptr<ChunkSection>(new ChunkSection113Base((int)yTag->fValue,
                                                                     palette,
                                                                     paletteIndices,
                                                                     extra));
    }

protected:
    ChunkSection113Base(int y,
                        std::vector<std::shared_ptr<Block const>> const &palette,
                        std::vector<uint16_t> const &paletteIndices,
                        std::shared_ptr<nbt::CompoundTag> const &extra)
        : fY(y)
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

public:
    int const fY;
    BlockPalette fBlocks;
    std::shared_ptr<nbt::CompoundTag> fExtra;
};

} // namespace mcfile::je::chunksection
