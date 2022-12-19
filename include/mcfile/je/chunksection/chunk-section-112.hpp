#pragma once

namespace mcfile::je::chunksection {

class ChunkSection112 : public ChunkSection {
public:
    static void MakeChunkSections(std::shared_ptr<nbt::ListTag> const &tags,
                                  int chunkX, int chunkZ,
                                  std::vector<std::shared_ptr<nbt::CompoundTag>> const &tileEntities,
                                  std::vector<std::shared_ptr<ChunkSection>> &out) {
        using namespace std;

        vector<shared_ptr<ChunkSection112>> rawSections;
        for (auto const &it : *tags) {
            auto section = it->asCompound();
            if (!section) {
                continue;
            }
            auto const &converted = MakeRawSection(section);
            if (converted) {
                rawSections.push_back(converted);
            }
        }

        ChunkSection112::Migrate(rawSections, chunkX, chunkZ, tileEntities, out);
    }

    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        palette.push_back(make_shared<Block const>("minecraft:air"));
        vector<uint16_t> indices(4096, 0);
        vector<uint8_t> blockLight;
        vector<uint8_t> skyLight(2048, 0xff);
        return shared_ptr<ChunkSection112>(new ChunkSection112(sectionY, palette, indices, blockLight, skyLight));
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

    std::optional<int> blockPaletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        auto idx = BlockIndex(offsetX, offsetY, offsetZ);
        return fBlocks.index(idx);
    }

    int y() const override {
        return fY;
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

    void eachBlockPalette(std::function<bool(Block const &)> visitor) const override {
        fBlocks.eachValue([visitor](std::shared_ptr<Block const> const &v) {
            if (!v) {
                return true;
            }
            return visitor(*v);
        });
    }

private:
    ChunkSection112(int y,
                    std::vector<std::shared_ptr<Block const>> const &palette, std::vector<uint16_t> const &paletteIndices,
                    std::vector<uint8_t> const &blockLight,
                    std::vector<uint8_t> const &skyLight)
        : fY(y)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight) {
        fBlocks.reset(palette, paletteIndices);
    }

    static std::shared_ptr<ChunkSection112> MakeRawSection(nbt::CompoundTag const *section) {
        if (!section) {
            return nullptr;
        }
        auto yTag = section->query("Y")->asByte();
        if (!yTag) {
            return nullptr;
        }

        auto blocksTag = section->query("Blocks")->asByteArray();
        if (!blocksTag) {
            return nullptr;
        }
        std::vector<uint8_t> blocks = blocksTag->value();
        blocks.resize(4096);

        auto dataTag = section->query("Data")->asByteArray();
        if (!dataTag) {
            return nullptr;
        }
        std::vector<uint8_t> data = dataTag->value();
        data.resize(2048);

        auto addTag = section->query("Add")->asByteArray();
        std::vector<uint8_t> add;
        if (addTag) {
            add = addTag->value();
        }
        add.resize(2048);

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

        std::vector<std::shared_ptr<Block const>> palette;
        std::vector<uint16_t> paletteIndices(16 * 16 * 16);
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    auto index = BlockIndex(x, y, z);
                    uint8_t const idLo = blocks[index];
                    uint8_t const idHi = Flatten::Nibble4(add, index);
                    uint16_t const id = (uint16_t)idLo + ((uint16_t)idHi << 8);
                    uint8_t const blockData = Flatten::Nibble4(data, index);
                    std::shared_ptr<Block const> block = Flatten::DoFlatten(id, blockData);
                    int paletteIndex = -1;
                    for (int i = 0; i < (int)palette.size(); i++) {
                        if (palette[i]->equals(*block)) {
                            paletteIndex = i;
                            break;
                        }
                    }
                    if (paletteIndex < 0) {
                        paletteIndex = (int)palette.size();
                        palette.push_back(block);
                    }
                    paletteIndices[index] = (uint16_t)paletteIndex;
                }
            }
        }

        return std::shared_ptr<ChunkSection112>(new ChunkSection112(yTag->fValue, palette, paletteIndices, blockLight, skyLight));
    }

    static void Migrate(std::vector<std::shared_ptr<ChunkSection112>> const &raw,
                        int chunkX, int chunkZ,
                        std::vector<std::shared_ptr<nbt::CompoundTag>> const &inTileEntities,
                        std::vector<std::shared_ptr<ChunkSection>> &out) {
        using namespace std;
        using mcfile::String;

        map<tuple<int, int, int>, std::shared_ptr<nbt::CompoundTag>> tileEntities;
        for (auto const &it : inTileEntities) {
            auto x = it->int32("x");
            auto y = it->int32("y");
            auto z = it->int32("z");
            if (!x || !y || !z) {
                continue;
            }
            tileEntities[make_tuple(*x - 16 * chunkX, *y, *z - 16 * chunkZ)] = it;
        }

        auto tileAt = [&tileEntities](int offsetX, int blockY, int offsetZ) -> shared_ptr<nbt::CompoundTag> {
            auto pos = make_tuple(offsetX, blockY, offsetZ);
            auto found = tileEntities.find(pos);
            if (found == tileEntities.end()) {
                return nullptr;
            } else {
                return found->second;
            }
        };

        out.reserve(raw.size());
        for (auto const &it : raw) {
            vector<shared_ptr<Block const>> palette;
            vector<uint16_t> paletteIndices;
            paletteIndices.reserve(16 * 16 * 16);
            if (it->fY < 0 || 16 <= it->fY) {
                continue;
            }

            for (int y = 0; y < 16; y++) {
                int const by = it->fY * 16 + y;
                for (int z = 0; z < 16; z++) {
                    for (int x = 0; x < 16; x++) {
                        auto const &block = it->blockAt(x, y, z);
                        assert(block);
                        shared_ptr<Block const> converted;
                        string name = block->fName;
                        if (name.size() > 10 && name.substr(0, 10) == "minecraft:") {
                            name = name.substr(10);
                        }
                        if (String::EndsWith(name, "_door")) {
                            string half = block->property("half", "lower");
                            string hinge = "left";
                            string powered = "false";
                            string facing = "east";
                            string open = "false";
                            if (half == "lower") {
                                facing = block->property("facing", "east");
                                open = block->property("open", "false");

                                auto upper = GetBlockAt(x, by + 1, z, raw);
                                if (upper) {
                                    hinge = upper->property("hinge", "left");
                                    powered = upper->property("powered", "false");
                                }
                            } else {
                                hinge = block->property("hinge", "left");
                                powered = block->property("powered", "false");

                                auto lower = GetBlockAt(x, by - 1, z, raw);
                                if (lower) {
                                    facing = lower->property("facing", "east");
                                    open = lower->property("open", "false");
                                }
                            }
                            converted = block->applying({
                                {"hinge", hinge},
                                {"powered", powered},
                                {"facing", facing},
                                {"open", open},
                            });
                        } else if (name == "red_bed") {
                            auto tile = tileAt(x, by, z);
                            if (tile && tile->string("id", "") == "minecraft:bed") {
                                auto colorCode = tile->int32("color", 0);
                                auto color = ColorNameFromCode(colorCode);
                                converted = block->renamed("minecraft:" + color + "_bed");
                            }
                        } else if (name == "note_block") {
                            auto tile = tileAt(x, by, z);
                            if (tile && tile->string("id", "") == "minecraft:noteblock") {
                                auto note = tile->byte("note", 0);
                                auto powered = tile->boolean("powered", false);
                                converted = block->applying({
                                    {"note", to_string(note)},
                                    {"powered", powered ? "true" : "false"},
                                });
                            }
                        } else if (name == "sunflower") {
                            auto lower = GetBlockAt(x, by - 1, z, raw);
                            if (lower) {
                                if (lower->fName == "minecraft:lilac" || lower->fName == "minecraft:tall_grass" || lower->fName == "minecraft:large_fern" || lower->fName == "minecraft:rose_bush" || lower->fName == "minecraft:peony") {
                                    converted = block->applying({{"half", "upper"}});
                                } else {
                                    converted = block->applying({{"half", lower->fName == "minecraft:sunflower" ? "upper" : "lower"}});
                                }
                            } else {
                                converted = block->applying({
                                    {"half", "lower"}, // sunflower on y = 0?
                                });
                            }
                        } else if (name == "skeleton_skull" || name == "skeleton_wall_skull") {
                            auto tile = tileAt(x, by, z);
                            if (tile) {
                                auto rot = tile->byte("Rot");
                                auto type = tile->byte("SkullType");
                                if (rot && type) {
                                    string n = "skeleton";
                                    string p = "skull";
                                    switch (*type) {
                                    case 0:
                                        n = "skeleton";
                                        p = "skull";
                                        break;
                                    case 1:
                                        n = "wither_skeleton";
                                        p = "skull";
                                        break;
                                    case 2:
                                        n = "zombie";
                                        p = "head";
                                        break;
                                    case 3:
                                        n = "player";
                                        p = "head";
                                        break;
                                    case 4:
                                        n = "creeper";
                                        p = "head";
                                        break;
                                    case 5:
                                        n = "dragon";
                                        p = "head";
                                        break;
                                    }
                                    if (name == "skeleton_skull") {
                                        converted = block->renamed("minecraft:" + n + "_" + p)->applying({{"rotation", std::to_string(*rot)}});
                                    } else {
                                        converted = block->renamed("minecraft:" + n + "_wall_" + p);
                                    }
                                }
                            }
                        }
                        if (!converted) {
                            converted = block;
                        }

                        auto found = find_if(palette.begin(), palette.end(), [&converted](shared_ptr<Block const> const &p) {
                            return p->equals(*converted);
                        });
                        if (found == palette.end()) {
                            uint16_t index = (uint16_t)palette.size();
                            palette.push_back(converted);
                            paletteIndices.push_back(index);
                        } else {
                            uint16_t index = (uint16_t)distance(palette.begin(), found);
                            paletteIndices.push_back(index);
                        }
                    }
                }
            }

            shared_ptr<ChunkSection112> section(new ChunkSection112(it->fY, palette, paletteIndices, it->fBlockLight, it->fSkyLight));
            out.push_back(section);
        }
    }

    static std::string ColorNameFromCode(int32_t code) {
        switch (code % 16) {
        case 0:
            return "white";
        case 1:
            return "orange";
        case 2:
            return "magenta";
        case 3:
            return "light_blue";
        case 4:
            return "yellow";
        case 5:
            return "lime";
        case 6:
            return "pink";
        case 7:
            return "gray";
        case 8:
            return "light_gray";
        case 9:
            return "cyan";
        case 10:
            return "purple";
        case 11:
            return "blue";
        case 12:
            return "brown";
        case 13:
            return "green";
        case 14:
            return "red";
        case 15:
            return "black";
        }
        assert(false);
        return "";
    }

    static std::shared_ptr<Block const> GetBlockAt(int offsetX, int blockY, int offsetZ, std::vector<std::shared_ptr<ChunkSection112>> const &raw) {
        if (offsetX < 0 || 16 <= offsetX || offsetZ < 0 || 16 <= offsetZ) {
            return nullptr;
        }
        int const chunkY = blockY / 16;
        int const offsetY = blockY - 16 * chunkY;
        auto found = find_if(raw.begin(), raw.end(), [chunkY](auto const &item) { return item->fY == chunkY; });
        if (found == raw.end()) {
            return nullptr;
        }
        return (*found)->blockAt(offsetX, offsetY, offsetZ);
    }

    static inline size_t BlockIndex(int offsetX, int offsetY, int offsetZ) {
        assert(0 <= offsetX && offsetX < 16 && 0 <= offsetY && offsetY < 16 && 0 <= offsetZ && offsetZ < 16);
        return (offsetY << 8) + (offsetZ << 4) + offsetX;
    }

private:
    int const fY;

    BlockPalette fBlocks;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
};

} // namespace mcfile::je::chunksection
