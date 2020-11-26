#pragma once

namespace mcfile {
namespace detail {

class ChunkSection_1_12 : public ChunkSection {
public:
    static void MakeChunkSections(std::shared_ptr<nbt::ListTag> const& tags,
                                  int chunkX, int chunkZ,
                                  std::vector<std::shared_ptr<nbt::CompoundTag>> const& tileEntities,
                                  std::vector<std::shared_ptr<ChunkSection>> &out) {
        using namespace std;

        vector<shared_ptr<ChunkSection_1_12>> rawSections;
        for (auto const& it : *tags) {
            auto section = it->asCompound();
            if (!section) {
                continue;
            }
            auto const& converted = MakeRawSection(section);
            if (converted) {
                rawSections.push_back(converted);
            }
        }

        ChunkSection_1_12::Migrate(rawSections, chunkX, chunkZ, tileEntities, out);
    }

    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return nullptr;
        }
        int const i = fPaletteIndices[index];
        return fPalette[i];
    }

    uint8_t blockLightAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return 0;
        }
        return fBlockLight[index];
    }

    uint8_t skyLightAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return 0;
        }
        return fSkyLight[index];
    }

    blocks::BlockId blockIdAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return blocks::unknown;
        }
        int const i = fPaletteIndices[index];
        auto const& block = fPalette[i];
        return blocks::FromName(block->fName);
    }

    bool blockIdWithYOffset(int offsetY, std::function<bool(int offsetX, int offsetZ, blocks::BlockId blockId)> callback) const override {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                auto id = blockIdAt(x, offsetY, z);
                if (callback(x, z, id)) {
                    return false;
                }
            }
        }
        return true;
    }

    int y() const override {
        return fY;
    }

    std::vector<std::shared_ptr<Block const>> const& palette() const override {
        return fPalette;
    }

    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return std::nullopt;
        }
        return fPaletteIndices[index];
    }

private:
    ChunkSection_1_12(int y,
                      std::vector<std::shared_ptr<Block const>> const& palette, std::vector<uint16_t> const& paletteIndices,
                      std::vector<uint8_t> const& blockLight,
                      std::vector<uint8_t> const& skyLight)
        : fY(y)
        , fPalette(palette)
        , fPaletteIndices(paletteIndices)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight)
    {
    }

    static std::shared_ptr<ChunkSection_1_12> MakeRawSection(nbt::CompoundTag const* section) {
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
                    int const index = BlockIndex(x, y, z);
                    uint8_t const idLo = blocks[index];
                    uint8_t const idHi = Nibble4(add, index);
                    uint16_t const id = (uint16_t)idLo + ((uint16_t)idHi << 8);
                    uint8_t const blockData = Nibble4(data, index);
                    std::shared_ptr<Block const> block = Flatten(id, blockData);
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

        return std::shared_ptr<ChunkSection_1_12>(new ChunkSection_1_12(yTag->fValue, palette, paletteIndices, blockLight, skyLight));
    }

    static void Migrate(std::vector<std::shared_ptr<ChunkSection_1_12>> const& raw,
                        int chunkX, int chunkZ,
                        std::vector<std::shared_ptr<nbt::CompoundTag>> const& inTileEntities,
                        std::vector<std::shared_ptr<ChunkSection>> &out) {
        using namespace std;

        map<tuple<int, int, int>, std::shared_ptr<nbt::CompoundTag>> tileEntities;
        for (auto const& it : inTileEntities) {
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
        for (auto const& it : raw) {
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
                        auto const& block = it->blockAt(x, y, z);
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
                            map<string, string> props(block->fProperties);
                            props["hinge"] = hinge;
                            props["powered"] = powered;
                            props["facing"] = facing;
                            props["open"] = open;
                            converted.reset(new Block(block->fName, props));
                        } else if (name == "red_bed") {
                            auto tile = tileAt(x, by, z);
                            if (tile && tile->string("id", "") == "minecraft:bed") {
                                auto colorCode = tile->int32("color", 0);
                                auto color = ColorNameFromCode(colorCode);
                                converted.reset(new Block("minecraft:" + color + "_bed", block->fProperties));
                            } else {
                                converted = block;
                            }
                        } else if (name == "note_block") {
                            auto tile = tileAt(x, by, z);
                            if (tile && tile->string("id", "") == "minecraft:noteblock") {
                                auto note = tile->byte("note", 0);
                                auto powered = tile->boolean("powered", false);
                                map<string, string> props(block->fProperties);
                                props["note"] = to_string(note);
                                props["powered"] = powered ? "true" : "false";
                                converted.reset(new Block(block->fName, props));
                            } else {
                                converted = block;
                            }
                        } else {
                            converted = block;
                        }

                        auto found = find_if(palette.begin(), palette.end(), [&converted](shared_ptr<Block const> const& p) {
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

            shared_ptr<ChunkSection_1_12> section(new ChunkSection_1_12(it->fY, palette, paletteIndices, it->fBlockLight, it->fSkyLight));
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

    static std::shared_ptr<Block const> GetBlockAt(int offsetX, int blockY, int offsetZ, std::vector<std::shared_ptr<ChunkSection_1_12>> const& raw) {
        if (offsetX < 0 || 16 <= offsetX || offsetZ < 0 || 16 <= offsetZ) {
            return nullptr;
        }
        int const chunkY = blockY / 16;
        int const offsetY = blockY - 16 * chunkY;
        auto found = find_if(raw.begin(), raw.end(), [chunkY](auto const& item) { return item->fY == chunkY; });
        if (found == raw.end()) {
            return nullptr;
        }
        return (*found)->blockAt(offsetX, offsetY, offsetZ);
    } 

    static inline int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return offsetY * 16 * 16 + offsetZ * 16 + offsetX;
    }

    static inline uint8_t Nibble4(std::vector<uint8_t> const& arr, int index) {
        return index % 2 == 0 ? arr[index / 2] & 0x0F : (arr[index / 2] >> 4) & 0x0F;
    }

    static void Stairs(uint8_t data, std::map<std::string, std::string> &props) {
        static std::string const facing[4] = {"east", "west", "south", "north"};
        props["facing"] = facing[std::clamp(data & 0x3, 0, 3)];
        static std::string const half[2] = {"bottm", "top"};
        props["half"] = half[std::clamp(data & 0x4, 0, 1)];
    }

    static void Door(uint8_t data, std::map<std::string, std::string>& props) {
        static std::string const facing[4] = {"east", "south", "west", "north"};
        static std::string const hinge[2] = {"left", "right"};
        static std::string const open[2] = {"false", "true"};
        static std::string const powered[2] = {"false", "true"};
        if (data < 8) {
            props["half"] = "lower";
            props["facing"] = facing[std::clamp(data & 0x3, 0, 3)];
            props["open"] = open[std::clamp((data >> 2) & 0x1, 0, 1)];
        } else {
            props["half"] = "upper";
            props["hinge"] = hinge[std::clamp(data & 0x1, 0, 1)];
            props["powered"] = powered[std::clamp((data >> 1) & 0x1, 0, 1)];
        }
    }

    static void Log(uint8_t data, std::map<std::string, std::string>& props) {
        switch ((data >> 2) & 0x3) {
        case 1:
            props["axis"] = "x";
            break;
        case 2:
            props["axis"] = "z";
            break;
        case 0:
        default:
            props["axis"] = "y";
            break;
        }
    }

    static void Leaves(uint8_t data, std::map<std::string, std::string> & props) {
        if ((data >> 2) & 0x1 == 0x1) {
            props["persistent"] = "true";
        } else {
            props["persistent"] = "false";
        }
    }

    static void Bed(uint8_t data, std::map<std::string, std::string> & props) {
        switch (data & 0x3) {
        case 2:
            props["facing"] = "north";
            break;
        case 3:
            props["facing"] = "east";
        case 1:
            props["facing"] = "west";
            break;
        case 0:
        default:
            props["facing"] = "south";
            break;
        }
        props["occupied"] = (data >> 2) & 0x1 == 0x1 ? "true" : "false";
        props["part"] = (data >> 3) & 0x1 == 0x1 ? "head" : "foot";
    }

    static void Rail(uint8_t data, std::map<std::string, std::string>& props) {
        static std::string const shape[10] = {
            "north_south", // 0
            "east_west", // 1
            "ascending_east", // 2
            "ascending_west", // 3
            "ascending_north", // 4
            "ascending_south", // 5
            "south_east", // 6
            "south_west", // 7
            "north_west", // 8
            "north_east", // 9
        };
        props["shape"] = shape[std::clamp<uint8_t>(data, 0, 9)];
    }

    static void PoweredRail(uint8_t data, std::map<std::string, std::string>& props) {
        static std::string const shape[6] = {
            "north_south", // 0
            "east_west", // 1
            "ascending_east", // 2
            "ascending_west", // 3
            "ascending_north", // 4
            "ascending_south" // 5
        };
        props["shape"] = shape[std::clamp(data & 0x7, 0, 5)];
        props["powered"] = ((data >> 3) & 0x1) == 0x1 ? "true" : "false";
    }

    static inline std::shared_ptr<Block const> Flatten(uint16_t blockId, uint8_t data) {
        auto id = blocks::minecraft::air;
        std::map<std::string, std::string> props;
        switch (blockId) {
            case 0: id = blocks::minecraft::air; break;
            case 1:
                switch (data) {
                    case 1: id = blocks::minecraft::granite; break;
                    case 2: id = blocks::minecraft::polished_granite; break;
                    case 3: id = blocks::minecraft::diorite; break;
                    case 4: id = blocks::minecraft::polished_diorite; break;
                    case 5: id = blocks::minecraft::andesite; break;
                    case 6: id = blocks::minecraft::polished_andesite; break;
                    case 0:
                    default:
                        id = blocks::minecraft::stone;
                        break;
                }
                break;
            case 2: id = blocks::minecraft::grass_block; break;
            case 3:
                switch (data) {
                    case 1: id = blocks::minecraft::coarse_dirt; break;
                    case 2: id = blocks::minecraft::podzol; break;
                    case 0:
                    default:
                        id = blocks::minecraft::dirt;
                        break;
                }
                break;
            case 4: id = blocks::minecraft::cobblestone; break;
            case 5:
                switch (data) {
                    case 1: id = blocks::minecraft::spruce_planks; break;
                    case 2: id = blocks::minecraft::birch_planks; break;
                    case 3: id = blocks::minecraft::jungle_planks; break;
                    case 4: id = blocks::minecraft::acacia_planks; break;
                    case 5: id = blocks::minecraft::dark_oak_planks; break;
                    case 0:
                    default:
                        id = blocks::minecraft::oak_planks;
                        break;
                }
                break;
            case 6:
                switch (data & 0x7) {
                    case 1: id = blocks::minecraft::spruce_sapling; break;
                    case 2: id = blocks::minecraft::birch_sapling; break;
                    case 3: id = blocks::minecraft::jungle_sapling; break;
                    case 4: id = blocks::minecraft::acacia_sapling; break;
                    case 5: id = blocks::minecraft::dark_oak_sapling; break;
                    case 0:
                    default:
                        id = blocks::minecraft::oak_sapling;
                        break;
                }
                break;
            case 7: id = blocks::minecraft::bedrock; break;
            case 8: id = blocks::minecraft::water; break; //TODO: flowing_water
            case 9:
                id = blocks::minecraft::water;
                props["level"] = std::to_string(data);
                break;
            case 10: id = blocks::minecraft::lava; break; //TODO: flowing_lava
            case 11:
                id = blocks::minecraft::lava;
                props["level"] = std::to_string(data);
                break;
            case 12:
                switch (data) {
                    case 1: id = blocks::minecraft::red_sand; break;
                    case 0:
                    default:
                        id = blocks::minecraft::sand;
                }
                break;
            case 13: id = blocks::minecraft::gravel; break;
            case 14: id = blocks::minecraft::gold_ore; break;
            case 15: id = blocks::minecraft::iron_ore; break;
            case 16: id = blocks::minecraft::coal_ore; break;
            case 17:
                switch (data & 0x3) {
                    case 1: id = blocks::minecraft::spruce_log; break;
                    case 2: id = blocks::minecraft::birch_log; break;
                    case 3: id = blocks::minecraft::jungle_log; break;
                    case 0:
                    default:
                        id = blocks::minecraft::oak_log;
                        break;
                }
                Log(data, props);
                break;
            case 18:
                switch (data & 0x3) {
                case 1:
                    id = blocks::minecraft::spruce_leaves;
                    break;
                case 2:
                    id = blocks::minecraft::birch_leaves;
                    break;
                case 3:
                    id = blocks::minecraft::jungle_leaves;
                    break;
                case 0:
                default:
                    id = blocks::minecraft::oak_leaves;
                    break;
                }
                Leaves(data, props);
                break;
            case 19:
                switch (data) {
                    case 1: id = blocks::minecraft::wet_sponge; break;
                    case 0:
                    default:
                        id = blocks::minecraft::sponge;
                        break;
                }
                break;
            case 20: id = blocks::minecraft::glass; break;
            case 21: id = blocks::minecraft::lapis_ore; break;
            case 22: id = blocks::minecraft::lapis_block; break;
            case 23: id = blocks::minecraft::dispenser; break;
            case 24:
                switch (data) {
                    case 1: id = blocks::minecraft::chiseled_sandstone; break;
                    case 2: id = blocks::minecraft::smooth_sandstone; break;
                    case 0:
                    default:
                        id = blocks::minecraft::sandstone;
                        break;
                }
                break;
            case 25: id = blocks::minecraft::note_block; break;
            case 26:
                id = blocks::minecraft::red_bed;
                Bed(data, props);
                break;
            case 27:
                id = blocks::minecraft::powered_rail;
                PoweredRail(data, props);
                break;
            case 28:
                id = blocks::minecraft::detector_rail;
                PoweredRail(data, props);
                break;
            case 29: id = blocks::minecraft::sticky_piston; break;
            case 30: id = blocks::minecraft::cobweb; break;
            case 31: id = blocks::minecraft::grass; break;
            case 32: id = blocks::minecraft::dead_bush; break;
            case 33: id = blocks::minecraft::piston; break;
            case 34: id = blocks::minecraft::piston_head; break;
            case 35:
                switch (data) {
                    case 1: id = blocks::minecraft::orange_wool; break;
                    case 2: id = blocks::minecraft::magenta_wool; break;
                    case 3: id = blocks::minecraft::light_blue_wool; break;
                    case 4: id = blocks::minecraft::yellow_wool; break;
                    case 5: id = blocks::minecraft::lime_wool; break;
                    case 6: id = blocks::minecraft::pink_wool; break;
                    case 7: id = blocks::minecraft::gray_wool; break;
                    case 8: id = blocks::minecraft::light_gray_wool; break;
                    case 9: id = blocks::minecraft::cyan_wool; break;
                    case 10: id = blocks::minecraft::purple_wool; break;
                    case 11: id = blocks::minecraft::blue_wool; break;
                    case 12: id = blocks::minecraft::brown_wool; break;
                    case 13: id = blocks::minecraft::green_wool; break;
                    case 14: id = blocks::minecraft::red_wool; break;
                    case 15: id = blocks::minecraft::black_wool; break;
                    case 0:
                    default:
                        id = blocks::minecraft::white_wool;
                        break;
                }
                break;
            case 37: id = blocks::minecraft::dandelion; break;
            case 38: id = blocks::minecraft::poppy; break;
            case 39: id = blocks::minecraft::brown_mushroom; break;
            case 40: id = blocks::minecraft::red_mushroom; break;
            case 41: id = blocks::minecraft::gold_block; break;
            case 42: id = blocks::minecraft::iron_block; break;
            case 43:
                switch (data) {
                    case 1: id = blocks::minecraft::sandstone_slab; break;
                    case 2: id = blocks::minecraft::oak_slab; break;
                    case 3: id = blocks::minecraft::cobblestone_slab; break;
                    case 4: id = blocks::minecraft::brick_slab; break;
                    case 5: id = blocks::minecraft::stone_brick_slab; break;
                    case 6: id = blocks::minecraft::quartz_slab; break;
                    case 7: id = blocks::minecraft::nether_brick_slab; break;
                    case 0:
                    default:
                        id = blocks::minecraft::stone_slab;
                        break;
                }
                props["type"] = "double";
                break;
            case 44:
                switch (data & 0x7) {
                case 1:
                    id = blocks::minecraft::sandstone_slab;
                    break;
                case 2:
                    id = blocks::minecraft::oak_slab;
                    break;
                case 3:
                    id = blocks::minecraft::cobblestone_slab;
                    break;
                case 4:
                    id = blocks::minecraft::brick_slab;
                    break;
                case 5:
                    id = blocks::minecraft::stone_brick_slab;
                    break;
                case 6:
                    id = blocks::minecraft::nether_brick_slab;
                    break;
                case 7:
                    id = blocks::minecraft::quartz_slab;
                    break;
                case 0:
                default:
                    id = id = blocks::minecraft::stone_slab;
                    break;
                }
                props["type"] = ((data >> 3) & 0x1) == 0x1 ? "top" : "bottom";
                break;
            case 45: id = blocks::minecraft::bricks; break;
            case 46: id = blocks::minecraft::tnt; break;
            case 47: id = blocks::minecraft::bookshelf; break;
            case 48: id = blocks::minecraft::mossy_cobblestone; break;
            case 49: id = blocks::minecraft::obsidian; break;
            case 50:
                switch (data) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        id = blocks::minecraft::wall_torch;
                        break;
                    case 0:
                    case 5:
                    default:
                        id = blocks::minecraft::torch;
                        break;
                }
                break;
            case 51: id = blocks::minecraft::fire; break;
            case 52: id = blocks::minecraft::spawner; break;
            case 53: id = blocks::minecraft::oak_stairs;
                Stairs(data, props);
                break;
            case 54: id = blocks::minecraft::chest; break;
            case 55: id = blocks::minecraft::redstone_wire; break;
            case 56: id = blocks::minecraft::diamond_ore; break;
            case 57: id = blocks::minecraft::diamond_block; break;
            case 58: id = blocks::minecraft::crafting_table; break;
            case 59: id = blocks::minecraft::wheat; break;
            case 60: id = blocks::minecraft::farmland; break;
            case 61:
            case 62: // lit
                id = blocks::minecraft::furnace;
                break;
            case 63: id = blocks::minecraft::oak_sign; break;
            case 64:
                id = blocks::minecraft::oak_door;
                Door(data, props);
                break;
            case 65: id = blocks::minecraft::ladder; break;
            case 66:
                id = blocks::minecraft::rail;
                Rail(data, props);
                break;
            case 67: id = blocks::minecraft::cobblestone_stairs;
                Stairs(data, props);
                break;
            case 68: id = blocks::minecraft::oak_wall_sign; break;
            case 69: id = blocks::minecraft::lever; break;
            case 70: id = blocks::minecraft::stone_pressure_plate; break;
            case 71:
                id = blocks::minecraft::iron_door;
                Door(data, props);
                break;
            case 72: id = blocks::minecraft::oak_pressure_plate; break;
            case 73: id = blocks::minecraft::redstone_ore; break;
            case 74: id = blocks::minecraft::redstone_ore; break; // glowing_redstone_ore
            case 75: //unlit_redstone_torch
                if (data == 5) {
                    id = blocks::minecraft::redstone_torch;
                } else {
                    id = blocks::minecraft::redstone_wall_torch;
                }
                break;
            case 76: //redstone_torch
                if (data == 5) {
                    id = blocks::minecraft::redstone_torch;
                } else {
                    id = blocks::minecraft::redstone_wall_torch;
                }
                break;
            case 77: id = blocks::minecraft::stone_button; break;
            case 78: id = blocks::minecraft::snow; break;
            case 79: id = blocks::minecraft::ice; break;
            case 80: id = blocks::minecraft::snow_block; break;
            case 81: id = blocks::minecraft::cactus; break;
            case 82: id = blocks::minecraft::clay; break;
            case 83: id = blocks::minecraft::sugar_cane; break;
            case 84: id = blocks::minecraft::jukebox; break;
            case 85: id = blocks::minecraft::oak_fence; break;
            case 86: id = blocks::minecraft::pumpkin; break;
            case 87: id = blocks::minecraft::netherrack; break;
            case 88: id = blocks::minecraft::soul_sand; break;
            case 89: id = blocks::minecraft::glowstone; break;
            case 90: id = blocks::minecraft::nether_portal; break;
            case 91: id = blocks::minecraft::jack_o_lantern; break;
            case 92: id = blocks::minecraft::cake; break;
            case 93: id = blocks::minecraft::repeater; break;
            case 94: id = blocks::minecraft::repeater; break; // powered_repeater
            case 95:
                switch (data) {
                    case 1: id = blocks::minecraft::orange_stained_glass; break;
                    case 2: id = blocks::minecraft::magenta_stained_glass; break;
                    case 3: id = blocks::minecraft::light_blue_stained_glass; break;
                    case 4: id = blocks::minecraft::yellow_stained_glass; break;
                    case 5: id = blocks::minecraft::lime_stained_glass; break;
                    case 6: id = blocks::minecraft::pink_stained_glass; break;
                    case 7: id = blocks::minecraft::gray_stained_glass; break;
                    case 8: id = blocks::minecraft::light_gray_stained_glass; break;
                    case 9: id = blocks::minecraft::cyan_stained_glass; break;
                    case 10: id = blocks::minecraft::purple_stained_glass; break;
                    case 11: id = blocks::minecraft::blue_stained_glass; break;
                    case 12: id = blocks::minecraft::brown_stained_glass; break;
                    case 13: id = blocks::minecraft::green_stained_glass; break;
                    case 14: id = blocks::minecraft::red_stained_glass; break;
                    case 15: id = blocks::minecraft::black_stained_glass; break;
                    case 0:
                    default:
                        id = blocks::minecraft::white_stained_glass;
                        break;
                }
                break;
            case 96: id = blocks::minecraft::oak_trapdoor; break;
            case 97:
                switch (data) {
                    case 1: id = blocks::minecraft::infested_cobblestone; break;
                    case 2: id = blocks::minecraft::infested_stone_bricks; break;
                    case 3: id = blocks::minecraft::infested_mossy_stone_bricks; break;
                    case 4: id = blocks::minecraft::infested_cracked_stone_bricks; break;
                    case 5: id = blocks::minecraft::infested_chiseled_stone_bricks; break;
                    case 0:
                    default:
                        id = blocks::minecraft::infested_stone;
                        break;
                }
                break;
            case 98:
                switch (data) {
                    case 1: id = blocks::minecraft::mossy_stone_bricks; break;
                    case 2: id = blocks::minecraft::cracked_stone_bricks; break;
                    case 3: id = blocks::minecraft::chiseled_stone_bricks; break;
                    case 0:
                    default:
                        id = blocks::minecraft::stone_bricks;
                        break;
                }
                break;
            case 99: id = blocks::minecraft::brown_mushroom_block; break;
            case 100: id = blocks::minecraft::red_mushroom_block; break;
            case 101: id = blocks::minecraft::iron_bars; break;
            case 102: id = blocks::minecraft::glass_pane; break;
            case 103: id = blocks::minecraft::melon; break;
            case 104: id = blocks::minecraft::pumpkin_stem; break;
            case 105: id = blocks::minecraft::melon_stem; break;
            case 106: id = blocks::minecraft::vine; break;
            case 107: id = blocks::minecraft::oak_fence_gate; break;
            case 108: id = blocks::minecraft::brick_stairs;
                Stairs(data, props);
                break;
            case 109: id = blocks::minecraft::stone_brick_stairs;
                Stairs(data, props);
                break;
            case 110: id = blocks::minecraft::mycelium; break;
            case 111: id = blocks::minecraft::lily_pad; break;
            case 112: id = blocks::minecraft::nether_bricks; break;
            case 113: id = blocks::minecraft::nether_brick_fence; break;
            case 114: id = blocks::minecraft::nether_brick_stairs;
                Stairs(data, props);
                break;
            case 115: id = blocks::minecraft::nether_wart; break;
            case 116: id = blocks::minecraft::enchanting_table; break;
            case 117: id = blocks::minecraft::brewing_stand; break;
            case 118: id = blocks::minecraft::cauldron; break;
            case 119: id = blocks::minecraft::end_portal; break;
            case 120: id = blocks::minecraft::end_portal_frame; break;
            case 121: id = blocks::minecraft::end_stone; break;
            case 122: id = blocks::minecraft::dragon_egg; break;
            case 123: id = blocks::minecraft::redstone_lamp; break;
            case 124: id = blocks::minecraft::redstone_lamp; break; // lit_redstone_lamp
            case 125:
                id = blocks::minecraft::oak_slab;
                props["type"] = "double";
                break;
            case 126:
                id = blocks::minecraft::oak_slab;
                props["type"] = "bottom";
                break;
            case 127: id = blocks::minecraft::cocoa; break;
            case 128: id = blocks::minecraft::sandstone_stairs;
                Stairs(data, props);
                break;
            case 129: id = blocks::minecraft::emerald_ore; break;
            case 130: id = blocks::minecraft::ender_chest; break;
            case 131: id = blocks::minecraft::tripwire_hook; break;
            case 132: id = blocks::minecraft::tripwire; break;
            case 133: id = blocks::minecraft::emerald_block; break;
            case 134: id = blocks::minecraft::spruce_stairs;
                Stairs(data, props);
                break;
            case 135: id = blocks::minecraft::birch_stairs;
                Stairs(data, props);
                break;
            case 136: id = blocks::minecraft::jungle_stairs;
                Stairs(data, props);
                break;
            case 137: id = blocks::minecraft::command_block; break;
            case 138: id = blocks::minecraft::beacon; break;
            case 139: id = blocks::minecraft::cobblestone_wall; break;
            case 140: id = blocks::minecraft::flower_pot; break;
            case 141: id = blocks::minecraft::carrots; break;
            case 142: id = blocks::minecraft::potatoes; break;
            case 143: id = blocks::minecraft::oak_button; break;
            case 144: id = blocks::minecraft::skeleton_skull; break;
            case 145: id = blocks::minecraft::anvil; break;
            case 146: id = blocks::minecraft::trapped_chest; break;
            case 147: id = blocks::minecraft::light_weighted_pressure_plate; break;
            case 148: id = blocks::minecraft::heavy_weighted_pressure_plate; break;
            case 149: id = blocks::minecraft::comparator; break; //unpowered_comparator
            case 150: id = blocks::minecraft::comparator; break;
            case 151: id = blocks::minecraft::daylight_detector; break;
            case 152: id = blocks::minecraft::redstone_block; break;
            case 153: id = blocks::minecraft::nether_quartz_ore; break;
            case 154: id = blocks::minecraft::hopper; break;
            case 155: id = blocks::minecraft::quartz_block; break;
            case 156: id = blocks::minecraft::quartz_stairs;
                Stairs(data, props);
                break;
            case 157:
                id = blocks::minecraft::activator_rail;
                PoweredRail(data, props);
                break;
            case 158: id = blocks::minecraft::dropper; break;
            case 159: id = blocks::minecraft::white_glazed_terracotta; break; //??
            case 160:
                switch (data) {
                    case 1: id = blocks::minecraft::orange_stained_glass_pane; break;
                    case 2: id = blocks::minecraft::magenta_stained_glass_pane; break;
                    case 3: id = blocks::minecraft::light_blue_stained_glass_pane; break;
                    case 4: id = blocks::minecraft::yellow_stained_glass_pane; break;
                    case 5: id = blocks::minecraft::lime_stained_glass_pane; break;
                    case 6: id = blocks::minecraft::pink_stained_glass_pane; break;
                    case 7: id = blocks::minecraft::gray_stained_glass_pane; break;
                    case 8: id = blocks::minecraft::light_gray_stained_glass_pane; break;
                    case 9: id = blocks::minecraft::cyan_stained_glass_pane; break;
                    case 10: id = blocks::minecraft::purple_stained_glass_pane; break;
                    case 11: id = blocks::minecraft::blue_stained_glass_pane; break;
                    case 12: id = blocks::minecraft::brown_stained_glass_pane; break;
                    case 13: id = blocks::minecraft::green_stained_glass_pane; break;
                    case 14: id = blocks::minecraft::red_stained_glass_pane; break;
                    case 15: id = blocks::minecraft::black_stained_glass_pane; break;
                    case 0:
                    default:
                        id = blocks::minecraft::white_stained_glass_pane;
                        break;
                }
                break;
            case 161:
                switch (data & 0x3) {
                    case 1:
                        id = blocks::minecraft::dark_oak_leaves;
                        break;
                    case 0:
                    default:
                        id = blocks::minecraft::acacia_leaves;
                        break;
                }
                Leaves(data, props);
                break;
            case 162:
                switch (data & 0x3) {
                    case 1: id = blocks::minecraft::dark_oak_log; break;
                    case 0:
                    default:
                        id = blocks::minecraft::acacia_log;
                        break;
                }
                Log(data, props);
                break;
            case 163: id = blocks::minecraft::acacia_stairs;
                Stairs(data, props);
                break;
            case 164: id = blocks::minecraft::dark_oak_stairs;
                Stairs(data, props);
                break;
            case 165: id = blocks::minecraft::slime_block; break;
            case 166: id = blocks::minecraft::barrier; break;
            case 167: id = blocks::minecraft::iron_trapdoor; break;
            case 168: id = blocks::minecraft::prismarine; break;
            case 169: id = blocks::minecraft::sea_lantern; break;
            case 170: id = blocks::minecraft::hay_block; break;
            case 171:
                switch (data) {
                    case 1: id = blocks::minecraft::orange_carpet; break;
                    case 2: id = blocks::minecraft::magenta_carpet; break;
                    case 3: id = blocks::minecraft::light_blue_carpet; break;
                    case 4: id = blocks::minecraft::yellow_carpet; break;
                    case 5: id = blocks::minecraft::lime_carpet; break;
                    case 6: id = blocks::minecraft::pink_carpet; break;
                    case 7: id = blocks::minecraft::gray_carpet; break;
                    case 8: id = blocks::minecraft::light_gray_carpet; break;
                    case 9: id = blocks::minecraft::cyan_carpet; break;
                    case 10: id = blocks::minecraft::purple_carpet; break;
                    case 11: id = blocks::minecraft::blue_carpet; break;
                    case 12: id = blocks::minecraft::brown_carpet; break;
                    case 13: id = blocks::minecraft::green_carpet; break;
                    case 14: id = blocks::minecraft::red_carpet; break;
                    case 15: id = blocks::minecraft::black_carpet; break;
                    case 0:
                    default:
                        id = blocks::minecraft::white_carpet;
                        break;
                }
                break;
            case 172:
                switch (data) {
                    case 1: id = blocks::minecraft::orange_terracotta; break;
                    case 2: id = blocks::minecraft::magenta_terracotta; break;
                    case 3: id = blocks::minecraft::light_blue_terracotta; break;
                    case 4: id = blocks::minecraft::yellow_terracotta; break;
                    case 5: id = blocks::minecraft::lime_terracotta; break;
                    case 6: id = blocks::minecraft::pink_terracotta; break;
                    case 7: id = blocks::minecraft::gray_terracotta; break;
                    case 8: id = blocks::minecraft::light_gray_terracotta; break;
                    case 9: id = blocks::minecraft::cyan_terracotta; break;
                    case 10: id = blocks::minecraft::purple_terracotta; break;
                    case 11: id = blocks::minecraft::blue_terracotta; break;
                    case 12: id = blocks::minecraft::brown_terracotta; break;
                    case 13: id = blocks::minecraft::green_terracotta; break;
                    case 14: id = blocks::minecraft::red_terracotta; break;
                    case 15: id = blocks::minecraft::black_terracotta; break;
                    case 0:
                    default:
                        id = blocks::minecraft::white_terracotta;
                        break;
                }
                break;
            case 173: id = blocks::minecraft::coal_block; break;
            case 174: id = blocks::minecraft::packed_ice; break;
            case 175:
                switch (data) {
                    case 1: id = blocks::minecraft::lilac; break;
                    case 2: id = blocks::minecraft::tall_grass; break;
                    case 3: id = blocks::minecraft::large_fern; break;
                    case 4: id = blocks::minecraft::rose_bush; break;
                    case 5: id = blocks::minecraft::peony; break;
                    case 0:
                    default:
                        id = blocks::minecraft::sunflower;
                        break;
                }
                break;
            case 176: id = blocks::minecraft::white_banner; break;
            case 177: id = blocks::minecraft::white_wall_banner; break;
            case 178: id = blocks::minecraft::daylight_detector; break; // inverted_daylight_detector
            case 179: id = blocks::minecraft::red_sandstone; break;
            case 180: id = blocks::minecraft::red_sandstone_stairs;
                Stairs(data, props);
                break;
            case 181:
                id = blocks::minecraft::red_sandstone_slab;
                props["type"] = "double";
                break;
            case 182:
                switch (data) {
                    case 8:
                        id = blocks::minecraft::red_sandstone_slab;
                        props["type"] = "top";
                        break;
                    case 0:
                    default:
                        id = blocks::minecraft::red_sandstone_slab;
                        props["type"] = "bottom";
                        break;
                }
                break;
            case 183: id = blocks::minecraft::spruce_fence_gate; break;
            case 184: id = blocks::minecraft::birch_fence_gate; break;
            case 185: id = blocks::minecraft::jungle_fence_gate; break;
            case 186: id = blocks::minecraft::dark_oak_fence_gate; break;
            case 187: id = blocks::minecraft::acacia_fence_gate; break;
            case 188: id = blocks::minecraft::spruce_fence; break;
            case 189: id = blocks::minecraft::birch_fence; break;
            case 190: id = blocks::minecraft::jungle_fence; break;
            case 191: id = blocks::minecraft::dark_oak_fence; break;
            case 192: id = blocks::minecraft::acacia_fence; break;
            case 193:
                id = blocks::minecraft::spruce_door;
                Door(data, props);
                break;
            case 194:
                id = blocks::minecraft::birch_door;
                Door(data, props);
                break;
            case 195:
                id = blocks::minecraft::jungle_door;
                Door(data, props);
                break;
            case 196:
                id = blocks::minecraft::acacia_door;
                Door(data, props);
                break;
            case 197:
                id = blocks::minecraft::dark_oak_door;
                Door(data, props);
                break;
            case 198: id = blocks::minecraft::end_rod; break;
            case 199: id = blocks::minecraft::chorus_plant; break;
            case 200: id = blocks::minecraft::chorus_flower; break;
            case 201: id = blocks::minecraft::purpur_block; break;
            case 202: id = blocks::minecraft::purpur_pillar; break;
            case 203: id = blocks::minecraft::purpur_stairs;
                Stairs(data, props);
                break;
            case 204:
                id = blocks::minecraft::purpur_slab;
                props["type"] = "double";
                break;
            case 205:
                id = blocks::minecraft::purpur_slab;
                if (data == 8) {
                    props["type"] = "top";
                } else {
                    props["type"] = "bottom";
                }
                break;
            case 206: id = blocks::minecraft::end_stone_bricks; break;
            case 207: id = blocks::minecraft::beetroots; break;
            case 208: id = blocks::minecraft::grass_path; break;
            case 209: id = blocks::minecraft::end_gateway; break;
            case 210: id = blocks::minecraft::repeating_command_block; break;
            case 211: id = blocks::minecraft::chain_command_block; break;
            case 212: id = blocks::minecraft::frosted_ice; break;
            case 213: id = blocks::minecraft::magma_block; break;
            case 214: id = blocks::minecraft::nether_wart_block; break;
            case 215: id = blocks::minecraft::red_nether_bricks; break;
            case 216: id = blocks::minecraft::bone_block; break;
            case 217: id = blocks::minecraft::structure_void; break;
            case 218: id = blocks::minecraft::observer; break;
            case 219: id = blocks::minecraft::white_shulker_box; break;
            case 220: id = blocks::minecraft::orange_shulker_box; break;
            case 221: id = blocks::minecraft::magenta_shulker_box; break;
            case 222: id = blocks::minecraft::light_blue_shulker_box; break;
            case 223: id = blocks::minecraft::yellow_shulker_box; break;
            case 224: id = blocks::minecraft::lime_shulker_box; break;
            case 225: id = blocks::minecraft::pink_shulker_box; break;
            case 226: id = blocks::minecraft::gray_shulker_box; break;
            case 227: id = blocks::minecraft::light_gray_shulker_box; break;
            case 228: id = blocks::minecraft::cyan_shulker_box; break;
            case 229: id = blocks::minecraft::purple_shulker_box; break;
            case 230: id = blocks::minecraft::blue_shulker_box; break;
            case 231: id = blocks::minecraft::brown_shulker_box; break;
            case 232: id = blocks::minecraft::green_shulker_box; break;
            case 233: id = blocks::minecraft::red_shulker_box; break;
            case 234: id = blocks::minecraft::black_shulker_box; break;
            case 235: id = blocks::minecraft::white_glazed_terracotta; break;
            case 236: id = blocks::minecraft::orange_glazed_terracotta; break;
            case 237: id = blocks::minecraft::magenta_glazed_terracotta; break;
            case 238: id = blocks::minecraft::light_blue_glazed_terracotta; break;
            case 239: id = blocks::minecraft::yellow_glazed_terracotta; break;
            case 240: id = blocks::minecraft::lime_glazed_terracotta; break;
            case 241: id = blocks::minecraft::pink_glazed_terracotta; break;
            case 242: id = blocks::minecraft::gray_glazed_terracotta; break;
            case 243: id = blocks::minecraft::light_gray_glazed_terracotta; break;
            case 244: id = blocks::minecraft::cyan_glazed_terracotta; break;
            case 245: id = blocks::minecraft::purple_glazed_terracotta; break;
            case 246: id = blocks::minecraft::blue_glazed_terracotta; break;
            case 247: id = blocks::minecraft::brown_glazed_terracotta; break;
            case 248: id = blocks::minecraft::green_glazed_terracotta; break;
            case 249: id = blocks::minecraft::red_glazed_terracotta; break;
            case 250: id = blocks::minecraft::black_glazed_terracotta; break;
            case 251: id = blocks::minecraft::white_concrete; break;
            case 252: id = blocks::minecraft::white_concrete_powder; break;
            case 255: id = blocks::minecraft::structure_block; break;
            default: id = blocks::minecraft::air; break;
        }
        return std::make_shared<Block>(id, props);
    }

private:
    int const fY;
    std::vector<std::shared_ptr<Block const>> fPalette;
    std::vector<uint16_t> fPaletteIndices;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
};

} // namespace detail
} // namespace mcfile
