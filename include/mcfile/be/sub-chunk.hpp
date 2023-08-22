#pragma once

namespace mcfile::be {

class SubChunk {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const {
        auto const index = paletteIndexAt(offsetX, offsetY, offsetZ);
        if (!index) {
            return nullptr;
        }
        return fPalette[*index];
    }

    std::shared_ptr<Block const> blockAtUnchecked(int offsetX, int offsetY, int offsetZ) const {
        if (fPalette.empty()) {
            return nullptr;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        auto paletteIndex = fPaletteIndices[index];
        return fPalette[paletteIndex];
    }

    static std::shared_ptr<SubChunk> Parse(std::string const &data, int8_t chunkY, Endian endian) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        auto bs = make_shared<ByteInputStream>(data);
        InputStreamReader sr(bs, endian);

        uint8_t version;
        if (!sr.read(&version)) {
            return nullptr;
        }

        // version values:
        // 1.0.0.16: 0
        // 1.1.5.0: 0
        // 1.2.10.2: 0
        // 1.4.2.0: 8
        // 1.10.1.1: 9

        if (version == 0) {
            return ParseVersion0(sr, chunkY);
        } else if (version == 8) {
            return ParseVersion8(sr, chunkY);
        } else if (version == 9) {
            return ParseVersion9(sr, chunkY);
        } else {
            return nullptr;
        }
    }

    static int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return (offsetX * 16 + offsetZ) * 16 + offsetY;
    }

private:
    static std::shared_ptr<SubChunk> ParseVersion0(mcfile::stream::InputStreamReader &sr, int8_t chunkY) {
        using namespace std;

        vector<uint8_t> id(4096);
        vector<uint8_t> data(4096);
        for (int i = 0; i < 4096; i++) {
            if (!sr.read(id.data() + i)) {
                return nullptr;
            }
        }
        for (int i = 0; i < 4096; i += 2) {
            uint8_t v;
            if (!sr.read(&v)) {
                return nullptr;
            }
            data[i] = 0xf & v;
            data[i + 1] = 0xf & (v >> 4);
        }
        static array<u8string, 256> const sTable = {
            u8"minecraft:air",
            u8"minecraft:stone",
            u8"minecraft:grass",
            u8"minecraft:dirt",
            u8"minecraft:cobblestone",
            u8"minecraft:planks",
            u8"minecraft:sapling",
            u8"minecraft:bedrock",
            u8"minecraft:flowing_water",
            u8"minecraft:water",
            u8"minecraft:flowing_lava",
            u8"minecraft:lava",
            u8"minecraft:sand",
            u8"minecraft:gravel",
            u8"minecraft:gold_ore",
            u8"minecraft:iron_ore",
            u8"minecraft:coal_ore",
            u8"minecraft:log",
            u8"minecraft:leaves",
            u8"minecraft:sponge",
            u8"minecraft:glass",
            u8"minecraft:lapis_ore",
            u8"minecraft:lapis_block",
            u8"minecraft:dispenser",
            u8"minecraft:sandstone",
            u8"minecraft:noteblock",
            u8"minecraft:bed",
            u8"minecraft:golden_rail",
            u8"minecraft:detector_rail",
            u8"minecraft:sticky_piston",
            u8"minecraft:web",
            u8"minecraft:tallgrass",
            u8"minecraft:deadbush",
            u8"minecraft:piston",
            u8"minecraft:pistonArmCollision",
            u8"minecraft:wool",
            u8"minecraft:element_0",
            u8"minecraft:yellow_flower",
            u8"minecraft:red_flower",
            u8"minecraft:brown_mushroom",
            u8"minecraft:red_mushroom",
            u8"minecraft:gold_block",
            u8"minecraft:iron_block",
            u8"minecraft:double_stone_slab",
            u8"minecraft:stone_slab",
            u8"minecraft:brick_block",
            u8"minecraft:tnt",
            u8"minecraft:bookshelf",
            u8"minecraft:mossy_cobblestone",
            u8"minecraft:obsidian",
            u8"minecraft:torch",
            u8"minecraft:fire",
            u8"minecraft:mob_spawner",
            u8"minecraft:oak_stairs",
            u8"minecraft:chest",
            u8"minecraft:redstone_wire",
            u8"minecraft:diamond_ore",
            u8"minecraft:diamond_block",
            u8"minecraft:crafting_table",
            u8"minecraft:wheat",
            u8"minecraft:farmland",
            u8"minecraft:furnace",
            u8"minecraft:lit_furnace",
            u8"minecraft:standing_sign",
            u8"minecraft:wooden_door",
            u8"minecraft:ladder",
            u8"minecraft:rail",
            u8"minecraft:stone_stairs",
            u8"minecraft:wall_sign",
            u8"minecraft:lever",
            u8"minecraft:stone_pressure_plate",
            u8"minecraft:iron_door",
            u8"minecraft:wooden_pressure_plate",
            u8"minecraft:redstone_ore",
            u8"minecraft:lit_redstone_ore",
            u8"minecraft:redstone_torch",
            u8"minecraft:redstone_torch",
            u8"minecraft:stone_button",
            u8"minecraft:snow_layer",
            u8"minecraft:ice",
            u8"minecraft:snow",
            u8"minecraft:cactus",
            u8"minecraft:clay",
            u8"minecraft:reeds",
            u8"minecraft:jukebox",
            u8"minecraft:fence",
            u8"minecraft:pumpkin",
            u8"minecraft:netherrack",
            u8"minecraft:soul_sand",
            u8"minecraft:glowstone",
            u8"minecraft:portal",
            u8"minecraft:lit_pumpkin",
            u8"minecraft:cake",
            u8"minecraft:unpowered_repeater",
            u8"minecraft:unpowered_repeater",
            u8"minecraft:invisibleBedrock",
            u8"minecraft:trapdoor",
            u8"minecraft:monster_egg",
            u8"minecraft:stonebrick",
            u8"minecraft:brown_mushroom_block",
            u8"minecraft:red_mushroom_block",
            u8"minecraft:iron_bars",
            u8"minecraft:glass_pane",
            u8"minecraft:melon_block",
            u8"minecraft:pumpkin_stem",
            u8"minecraft:melon_stem",
            u8"minecraft:vine",
            u8"minecraft:fence_gate",
            u8"minecraft:brick_stairs",
            u8"minecraft:stone_brick_stairs",
            u8"minecraft:mycelium",
            u8"minecraft:waterlily",
            u8"minecraft:nether_brick",
            u8"minecraft:nether_brick_fence",
            u8"minecraft:nether_brick_stairs",
            u8"minecraft:nether_wart",
            u8"minecraft:enchanting_table",
            u8"minecraft:brewing_stand",
            u8"minecraft:cauldron",
            u8"minecraft:end_portal",
            u8"minecraft:end_portal_frame",
            u8"minecraft:end_stone",
            u8"minecraft:dragon_egg",
            u8"minecraft:redstone_lamp",
            u8"minecraft:redstone_lamp",
            u8"minecraft:dropper",
            u8"minecraft:activator_rail",
            u8"minecraft:cocoa",
            u8"minecraft:sandstone_stairs",
            u8"minecraft:emerald_ore",
            u8"minecraft:ender_chest",
            u8"minecraft:tripwire_hook",
            u8"minecraft:tripWire",
            u8"minecraft:emerald_block",
            u8"minecraft:spruce_stairs",
            u8"minecraft:birch_stairs",
            u8"minecraft:jungle_stairs",
            u8"minecraft:command_block",
            u8"minecraft:beacon",
            u8"minecraft:cobblestone_wall",
            u8"minecraft:flower_pot",
            u8"minecraft:carrots",
            u8"minecraft:potatoes",
            u8"minecraft:wooden_button",
            u8"minecraft:skull",
            u8"minecraft:anvil",
            u8"minecraft:trapped_chest",
            u8"minecraft:light_weighted_pressure_plate",
            u8"minecraft:heavy_weighted_pressure_plate",
            u8"minecraft:unpowered_comparator",
            u8"minecraft:powered_comparator",
            u8"minecraft:daylight_detector",
            u8"minecraft:redstone_block",
            u8"minecraft:quartz_ore",
            u8"minecraft:hopper",
            u8"minecraft:quartz_block",
            u8"minecraft:quartz_stairs",
            u8"minecraft:double_wooden_slab",
            u8"minecraft:wooden_slab",
            u8"minecraft:stained_hardened_clay",
            u8"minecraft:stained_glass_pane",
            u8"minecraft:leaves2",
            u8"minecraft:log2",
            u8"minecraft:acacia_stairs",
            u8"minecraft:dark_oak_stairs",
            u8"minecraft:slime",
            u8"minecraft:glow_stick",
            u8"minecraft:iron_trapdoor",
            u8"minecraft:prismarine",
            u8"minecraft:seaLantern",
            u8"minecraft:hay_block",
            u8"minecraft:carpet",
            u8"minecraft:hardened_clay",
            u8"minecraft:coal_block",
            u8"minecraft:packed_ice",
            u8"minecraft:double_plant",
            u8"minecraft:standing_banner",
            u8"minecraft:wall_banner",
            u8"minecraft:daylight_detector_inverted",
            u8"minecraft:red_sandstone",
            u8"minecraft:red_sandstone_stairs",
            u8"minecraft:double_stone_slab2",
            u8"minecraft:stone_slab2",
            u8"minecraft:spruce_fence_gate",
            u8"minecraft:birch_fence_gate",
            u8"minecraft:jungle_fence_gate",
            u8"minecraft:dark_oak_fence_gate",
            u8"minecraft:acacia_fence_gate",
            u8"minecraft:repeating_command_block",
            u8"minecraft:chain_command_block",
            u8"minecraft:hard_glass_pane",
            u8"minecraft:hard_stained_glass_pane",
            u8"minecraft:chemical_heat",
            u8"minecraft:spruce_door",
            u8"minecraft:birch_door",
            u8"minecraft:jungle_door",
            u8"minecraft:acacia_door",
            u8"minecraft:dark_oak_door",
            u8"minecraft:grass_path",
            u8"minecraft:frame",
            u8"minecraft:chorus_flower",
            u8"minecraft:purpur_block",
            u8"minecraft:colored_torch_rg",
            u8"minecraft:purpur_stairs",
            u8"minecraft:colored_torch_bp",
            u8"minecraft:undyed_shulker_box",
            u8"minecraft:end_bricks",
            u8"minecraft:frosted_ice",
            u8"minecraft:end_rod",
            u8"minecraft:end_gateway",
            u8"minecraft:210",
            u8"minecraft:211",
            u8"minecraft:212",
            u8"minecraft:magma",
            u8"minecraft:nether_wart_block",
            u8"minecraft:red_nether_brick",
            u8"minecraft:bone_block",
            u8"minecraft:217",
            u8"minecraft:shulker_box",
            u8"minecraft:purple_glazed_terracotta",
            u8"minecraft:white_glazed_terracotta",
            u8"minecraft:orange_glazed_terracotta",
            u8"minecraft:magenta_glazed_terracotta",
            u8"minecraft:light_blue_glazed_terracotta",
            u8"minecraft:yellow_glazed_terracotta",
            u8"minecraft:lime_glazed_terracotta",
            u8"minecraft:pink_glazed_terracotta",
            u8"minecraft:gray_glazed_terracotta",
            u8"minecraft:silver_glazed_terracotta",
            u8"minecraft:cyan_glazed_terracotta",
            u8"minecraft:230",
            u8"minecraft:blue_glazed_terracotta",
            u8"minecraft:brown_glazed_terracotta",
            u8"minecraft:green_glazed_terracotta",
            u8"minecraft:red_glazed_terracotta",
            u8"minecraft:black_glazed_terracotta",
            u8"minecraft:concrete",
            u8"minecraft:concretePowder",
            u8"minecraft:chemistry_table",
            u8"minecraft:underwater_torch",
            u8"minecraft:chorus_plant",
            u8"minecraft:stained_glass",
            u8"minecraft:242",
            u8"minecraft:podzol",
            u8"minecraft:beetroot",
            u8"minecraft:stonecutter",
            u8"minecraft:glowingobsidian",
            u8"minecraft:netherreactor",
            u8"minecraft:info_update",
            u8"minecraft:info_update2",
            u8"minecraft:movingBlock",
            u8"minecraft:observer",
            u8"minecraft:structure_block",
            u8"minecraft:hard_glass",
            u8"minecraft:hard_stained_glass",
            u8"minecraft:reserved6",
        };
        vector<uint16_t> indices;
        vector<shared_ptr<Block const>> palette;
        vector<uint16_t> waterIndices;
        vector<shared_ptr<Block const>> waterPalette;
        map<pair<uint8_t, uint8_t>, uint16_t> paletteLut;
        for (size_t i = 0; i < 4096; i++) {
            uint8_t blockId = id[i];
            uint8_t val = data[i];
            auto key = make_pair(blockId, val);
            auto found = paletteLut.find(key);
            if (found == paletteLut.end()) {
                uint16_t index = (uint16_t)palette.size();
                auto block = make_shared<Block>(sTable[blockId], make_shared<nbt::CompoundTag>(), std::nullopt, val);
                palette.push_back(block);
                paletteLut[key] = index;
                indices.push_back(index);
            } else {
                indices.push_back(found->second);
            }
        }
        return shared_ptr<SubChunk>(new SubChunk(palette, indices, waterPalette, waterIndices, chunkY, 0));
    }

    static std::shared_ptr<SubChunk> ParseVersion9(mcfile::stream::InputStreamReader &sr, int8_t chunkY) {
        using namespace std;

        uint8_t numLayers;
        if (!sr.read(&numLayers)) {
            return nullptr;
        }
        int8_t cy;
        if (!sr.read(&cy)) {
            return nullptr;
        }
        if (cy != chunkY) {
            return nullptr;
        }
        if (numLayers == 0) {
            return nullptr;
        }
        vector<uint16_t> indices;
        vector<shared_ptr<Block const>> palette;
        if (!ParsePalette(sr, indices, palette)) {
            return nullptr;
        }
        vector<uint16_t> waterIndices;
        vector<shared_ptr<Block const>> waterPalette;
        if (numLayers > 1) {
            if (!ParsePalette(sr, waterIndices, waterPalette)) {
                return nullptr;
            }
        }
        return shared_ptr<SubChunk>(new SubChunk(palette, indices, waterPalette, waterIndices, chunkY, 9));
    }

    static std::shared_ptr<SubChunk> ParseVersion8(mcfile::stream::InputStreamReader &sr, int8_t chunkY) {
        using namespace std;

        uint8_t numLayers;
        if (!sr.read(&numLayers)) {
            return nullptr;
        }
        if (numLayers == 0) {
            return nullptr;
        }
        vector<uint16_t> indices;
        vector<shared_ptr<Block const>> palette;
        if (!ParsePalette(sr, indices, palette)) {
            return nullptr;
        }
        if (indices.size() != 4096 || palette.empty()) {
            return nullptr;
        }
        vector<uint16_t> waterIndices;
        vector<shared_ptr<Block const>> waterPalette;
        if (numLayers > 1) {
            if (!ParsePalette(sr, waterIndices, waterPalette)) {
                return nullptr;
            }
        }
        return shared_ptr<SubChunk>(new SubChunk(palette, indices, waterPalette, waterIndices, chunkY, 8));
    }

    static bool ParsePalette(mcfile::stream::InputStreamReader &sr, std::vector<uint16_t> &index, std::vector<std::shared_ptr<Block const>> &palette) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        uint8_t bitsPerBlock;
        if (!sr.read(&bitsPerBlock)) {
            return false;
        }
        if (bitsPerBlock == 0) {
            // layer exists, but no data.
            return true;
        }
        bitsPerBlock = bitsPerBlock / 2;

        int blocksPerWord = 32 / bitsPerBlock;
        int numWords;
        if (4096 % blocksPerWord == 0) {
            numWords = 4096 / blocksPerWord;
        } else {
            numWords = (int)ceilf(4096.0 / blocksPerWord);
        }
        vector<uint32_t> indexBuffer(numWords);
        for (int i = 0; i < numWords; i++) {
            uint32_t v;
            if (!sr.read(&v)) {
                return false;
            }
            indexBuffer[i] = v;
        }

        uint32_t const mask = ~((~((uint32_t)0)) << bitsPerBlock);
        index.resize(4096);
        int k = 0;
        for (int i = 0; i < numWords; i++) {
            uint32_t word = indexBuffer[i];
            for (int j = 0; j < blocksPerWord && k < 4096; j++) {
                uint16_t v = (word >> (bitsPerBlock * j)) & mask;
                index[k++] = v;
            }
        }

        uint32_t numPaletteEntries;
        if (!sr.read(&numPaletteEntries)) {
            return false;
        }
        if (numPaletteEntries > 4096) {
            return false;
        }

        palette.reserve(numPaletteEntries);

        for (uint32_t i = 0; i < numPaletteEntries; i++) {
            auto tag = CompoundTag::Read(sr);
            if (!tag) {
                return false;
            }
            auto block = Block::FromCompound(*tag);
            if (!block) {
                return false;
            }
            palette.push_back(block);
        }

        return true;
    }

    SubChunk(
        std::vector<std::shared_ptr<Block const>> &palette, std::vector<uint16_t> &paletteIndices,
        std::vector<std::shared_ptr<Block const>> &waterPalette, std::vector<uint16_t> &waterPaletteIndices,
        int8_t chunkY,
        uint8_t chunkVersion)
        : fChunkY(chunkY)
        , fChunkVersion(chunkVersion) {
        fPalette.swap(palette);
        fPaletteIndices.swap(paletteIndices);
        fWaterPalette.swap(waterPalette);
        fWaterPaletteIndices.swap(waterPaletteIndices);
    }

    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0 || fPaletteIndices.size() <= index) {
            return std::nullopt;
        }

        int paletteIndex = fPaletteIndices[index];
        if (paletteIndex < 0 || fPalette.size() <= paletteIndex) {
            return std::nullopt;
        }

        return (size_t)paletteIndex;
    }

public:
    std::vector<std::shared_ptr<Block const>> fPalette;
    std::vector<uint16_t> fPaletteIndices;
    std::vector<std::shared_ptr<Block const>> fWaterPalette;
    std::vector<uint16_t> fWaterPaletteIndices;
    int8_t const fChunkY;
    uint8_t const fChunkVersion;
};

} // namespace mcfile::be
