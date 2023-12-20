#pragma once

namespace mcfile::je::chunksection {

class ChunkSection112 : public ChunkSection {
public:
    static void MakeChunkSections(std::shared_ptr<nbt::ListTag> const &tags,
                                  int chunkX, int chunkZ, int dataVersion,
                                  std::vector<std::shared_ptr<nbt::CompoundTag>> const &tileEntities,
                                  std::vector<std::shared_ptr<ChunkSection>> &out) {
        using namespace std;

        vector<shared_ptr<ChunkSection112>> rawSections;
        for (auto const &it : *tags) {
            auto section = it->asCompound();
            if (!section) {
                continue;
            }
            auto const &converted = MakeRawSection(section, dataVersion);
            if (converted) {
                rawSections.push_back(converted);
            }
        }

        Migrate(rawSections, chunkX, chunkZ, tileEntities, out);
    }

    static std::shared_ptr<ChunkSection> MakeEmpty(int sectionY, int dataVersion) {
        using namespace std;
        vector<shared_ptr<Block const>> palette;
        palette.push_back(Block::FromId(blocks::minecraft::air, dataVersion));
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

    void eachBlockPalette(std::function<bool(std::shared_ptr<Block const> const &, size_t)> visitor) const override {
        fBlocks.eachValue([visitor](std::shared_ptr<Block const> const &v, size_t i) {
            return visitor(v, i);
        });
    }

    ChunkSection112 *clone() const override {
        std::unique_ptr<ChunkSection112> s(new ChunkSection112(fY));
        std::copy(fBlockLight.begin(), fBlockLight.end(), std::back_inserter(s->fBlockLight));
        std::copy(fSkyLight.begin(), fSkyLight.end(), std::back_inserter(s->fSkyLight));
        s->fBlocks = fBlocks;
        return s.release();
    }

private:
    ChunkSection112(int y,
                    std::vector<std::shared_ptr<Block const>> const &palette, std::vector<uint16_t> const &paletteIndices,
                    std::vector<uint8_t> &blockLight,
                    std::vector<uint8_t> &skyLight)
        : fY(y) {
        fBlockLight.swap(blockLight);
        fSkyLight.swap(skyLight);
        fBlocks.reset(palette, paletteIndices);
    }

    explicit ChunkSection112(int y)
        : fY(y) {}

    static std::shared_ptr<ChunkSection112> MakeRawSection(nbt::CompoundTag const *section, int dataVersion) {
        if (!section) {
            return nullptr;
        }
        auto yTag = section->query(u8"Y")->asByte();
        if (!yTag) {
            return nullptr;
        }

        auto blocksTag = section->query(u8"Blocks")->asByteArray();
        if (!blocksTag) {
            return nullptr;
        }
        std::vector<uint8_t> blocks = blocksTag->value();
        blocks.resize(4096);

        auto dataTag = section->query(u8"Data")->asByteArray();
        if (!dataTag) {
            return nullptr;
        }
        std::vector<uint8_t> data = dataTag->value();
        data.resize(2048);

        auto addTag = section->query(u8"Add")->asByteArray();
        std::vector<uint8_t> add;
        if (addTag) {
            add = addTag->value();
        }
        add.resize(2048);

        std::vector<uint8_t> blockLight;
        auto blockLightTag = section->query(u8"BlockLight")->asByteArray();
        if (blockLightTag) {
            blockLight = blockLightTag->value();
        }

        std::vector<uint8_t> skyLight;
        auto skyLightTag = section->query(u8"SkyLight")->asByteArray();
        if (skyLightTag) {
            skyLight = skyLightTag->value();
        }

        std::vector<std::shared_ptr<Block const>> palette;
        std::vector<std::pair<uint16_t, uint8_t>> numericPalette;
        std::vector<uint16_t> paletteIndices(16 * 16 * 16);
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    auto index = BlockIndex(x, y, z);
                    uint8_t const idLo = blocks[index];
                    uint8_t const idHi = Flatten::Nibble4(add, index);
                    uint16_t const id = (uint16_t)idLo + ((uint16_t)idHi << 8);
                    uint8_t const blockData = Flatten::Nibble4(data, index);
                    std::pair<uint16_t, uint8_t> key = std::make_pair(id, blockData);
                    int paletteIndex = -1;
                    for (int i = 0; i < (int)numericPalette.size(); i++) {
                        if (numericPalette[i] == key) {
                            paletteIndex = i;
                            break;
                        }
                    }
                    if (paletteIndex < 0) {
                        std::shared_ptr<Block const> block = Flatten::Block(id, blockData, dataVersion);
                        paletteIndex = (int)palette.size();
                        palette.push_back(block);
                        numericPalette.push_back(key);
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
            auto x = it->int32(u8"x");
            auto y = it->int32(u8"y");
            auto z = it->int32(u8"z");
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
                        auto name = block->fName;
                        if (name.size() > 10 && name.substr(0, 10) == u8"minecraft:") {
                            name = name.substr(10);
                        }
                        if (name.ends_with(u8"_door")) {
                            auto half = block->property(u8"half", u8"lower");
                            u8string hinge = u8"left";
                            u8string powered = u8"false";
                            u8string facing = u8"east";
                            u8string open = u8"false";
                            if (half == u8"lower") {
                                facing = block->property(u8"facing", u8"east");
                                open = block->property(u8"open", u8"false");

                                auto upper = GetBlockAt(x, by + 1, z, raw);
                                if (upper) {
                                    hinge = upper->property(u8"hinge", u8"left");
                                    powered = upper->property(u8"powered", u8"false");
                                }
                            } else {
                                hinge = block->property(u8"hinge", u8"left");
                                powered = block->property(u8"powered", u8"false");

                                auto lower = GetBlockAt(x, by - 1, z, raw);
                                if (lower) {
                                    facing = lower->property(u8"facing", u8"east");
                                    open = lower->property(u8"open", u8"false");
                                }
                            }
                            converted = block->applying({
                                {u8"hinge", hinge},
                                {u8"powered", powered},
                                {u8"facing", facing},
                                {u8"open", open},
                            });
                        } else if (name == u8"red_bed") {
                            auto tile = tileAt(x, by, z);
                            if (tile && tile->string(u8"id", u8"") == u8"minecraft:bed") {
                                auto colorCode = tile->int32(u8"color", 0);
                                converted = block->withId(BedBlockIdFromCode(colorCode));
                            }
                        } else if (name == u8"note_block") {
                            auto tile = tileAt(x, by, z);
                            if (tile && tile->string(u8"id", u8"") == u8"minecraft:noteblock") {
                                auto note = tile->byte(u8"note", 0);
                                auto powered = tile->boolean(u8"powered", false);
                                converted = block->applying({
                                    {u8"note", String::ToString(note)},
                                    {u8"powered", powered ? u8"true" : u8"false"},
                                });
                            }
                        } else if (name == u8"sunflower") {
                            auto lower = GetBlockAt(x, by - 1, z, raw);
                            if (lower) {
                                if (lower->fName == u8"minecraft:lilac" || lower->fName == u8"minecraft:tall_grass" || lower->fName == u8"minecraft:large_fern" || lower->fName == u8"minecraft:rose_bush" || lower->fName == u8"minecraft:peony") {
                                    converted = block->applying({{u8"half", u8"upper"}});
                                } else {
                                    converted = block->applying({{u8"half", lower->fName == u8"minecraft:sunflower" ? u8"upper" : u8"lower"}});
                                }
                            } else {
                                converted = block->applying({
                                    {u8"half", u8"lower"}, // sunflower on y = 0?
                                });
                            }
                        } else if (name == u8"skeleton_skull" || name == u8"skeleton_wall_skull") {
                            auto tile = tileAt(x, by, z);
                            if (tile) {
                                auto rot = tile->byte(u8"Rot");
                                auto type = tile->byte(u8"SkullType");
                                if (rot && type) {
                                    using namespace blocks::minecraft;
                                    bool wall = !(name == u8"skeleton_skull");
                                    blocks::BlockId id = skeleton_skull;
                                    switch (*type) {
                                    case 0:
                                        id = wall ? skeleton_wall_skull : skeleton_skull;
                                        break;
                                    case 1:
                                        id = wall ? wither_skeleton_wall_skull : wither_skeleton_skull;
                                        break;
                                    case 2:
                                        id = wall ? zombie_wall_head : zombie_head;
                                        break;
                                    case 3:
                                        id = wall ? player_wall_head : player_head;
                                        break;
                                    case 4:
                                        id = wall ? creeper_wall_head : creeper_head;
                                        break;
                                    case 5:
                                        id = wall ? dragon_wall_head : dragon_head;
                                        break;
                                    }
                                    if (wall) {
                                        converted = block->withId(id);
                                    } else {
                                        converted = block->withId(id)->applying({{u8"rotation", String::ToString(*rot)}});
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

    static blocks::BlockId BedBlockIdFromCode(int32_t code) {
        using namespace blocks::minecraft;
        switch (code % 16) {
        case 0:
            return white_bed;
        case 1:
            return orange_bed;
        case 2:
            return magenta_bed;
        case 3:
            return light_blue_bed;
        case 4:
            return yellow_bed;
        case 5:
            return lime_bed;
        case 6:
            return pink_bed;
        case 7:
            return gray_bed;
        case 8:
            return light_gray_bed;
        case 9:
            return cyan_bed;
        case 10:
            return purple_bed;
        case 11:
            return blue_bed;
        case 12:
            return brown_bed;
        case 13:
            return green_bed;
        case 14:
            return red_bed;
        case 15:
            return black_bed;
        }
        assert(false);
        return red_bed;
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
};

} // namespace mcfile::je::chunksection
