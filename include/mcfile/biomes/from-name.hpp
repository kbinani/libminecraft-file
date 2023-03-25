#pragma once
namespace mcfile::biomes {

static inline std::optional<mcfile::biomes::BiomeId> FromName(std::u8string const &name) {
    using namespace std;

    static unordered_map<u8string, BiomeId> const sMapping = {
        {u8"minecraft:nether_wastes", minecraft::nether_wastes},
        {u8"minecraft:nether", minecraft::nether_wastes},

        {u8"minecraft:ocean", minecraft::ocean},
        {u8"minecraft:plains", minecraft::plains},
        {u8"minecraft:desert", minecraft::desert},
        {u8"minecraft:windswept_hills", minecraft::windswept_hills}, // >= 1.18
        {u8"minecraft:mountains", minecraft::windswept_hills},       // < 1.18
        {u8"minecraft:forest", minecraft::forest},
        {u8"minecraft:taiga", minecraft::taiga},
        {u8"minecraft:swamp", minecraft::swamp},
        {u8"minecraft:river", minecraft::river},
        {u8"minecraft:the_end", minecraft::the_end},
        {u8"minecraft:frozen_ocean", minecraft::frozen_ocean},
        {u8"minecraft:frozen_river", minecraft::frozen_river},
        {u8"minecraft:snowy_plains", minecraft::snowy_plains},
        {u8"minecraft:snowy_tundra", minecraft::snowy_plains},
        {u8"minecraft:snowy_mountains", minecraft::snowy_mountains},
        {u8"minecraft:mushroom_fields", minecraft::mushroom_fields},
        {u8"minecraft:mushroom_field_shore", minecraft::mushroom_field_shore},
        {u8"minecraft:beach", minecraft::beach},
        {u8"minecraft:desert_hills", minecraft::desert_hills},
        {u8"minecraft:wooded_hills", minecraft::wooded_hills},
        {u8"minecraft:taiga_hills", minecraft::taiga_hills},
        {u8"minecraft:mountain_edge", minecraft::mountain_edge},
        {u8"minecraft:jungle", minecraft::jungle},
        {u8"minecraft:jungle_hills", minecraft::jungle_hills},
        {u8"minecraft:jungle_edge", minecraft::sparse_jungle},   // < 1.18
        {u8"minecraft:sparse_jungle", minecraft::sparse_jungle}, // >= 1/18
        {u8"minecraft:deep_ocean", minecraft::deep_ocean},
        {u8"minecraft:stone_shore", minecraft::stony_shore}, // < 1.18
        {u8"minecraft:stony_shore", minecraft::stony_shore}, // >= 1.18
        {u8"minecraft:snowy_beach", minecraft::snowy_beach},
        {u8"minecraft:birch_forest", minecraft::birch_forest},
        {u8"minecraft:birch_forest_hills", minecraft::birch_forest_hills},
        {u8"minecraft:dark_forest", minecraft::dark_forest},
        {u8"minecraft:snowy_taiga", minecraft::snowy_taiga},
        {u8"minecraft:snowy_taiga_hills", minecraft::snowy_taiga_hills},
        {u8"minecraft:giant_tree_taiga", minecraft::old_growth_pine_taiga},      // < 1.18
        {u8"minecraft:old_growth_pine_taiga", minecraft::old_growth_pine_taiga}, // >= 1.18
        {u8"minecraft:giant_tree_taiga_hills", minecraft::giant_tree_taiga_hills},
        {u8"minecraft:wooded_mountains", minecraft::windswept_forest}, // < 1.18
        {u8"minecraft:windswept_forest", minecraft::windswept_forest}, // >= 1.18
        {u8"minecraft:savanna", minecraft::savanna},
        {u8"minecraft:savanna_plateau", minecraft::savanna_plateau},
        {u8"minecraft:badlands", minecraft::badlands},
        {u8"minecraft:wooded_badlands_plateau", minecraft::wooded_badlands}, // < 1.18
        {u8"minecraft:wooded_badlands", minecraft::wooded_badlands},         // >= 1/18
        {u8"minecraft:badlands_plateau", minecraft::badlands_plateau},
        {u8"minecraft:small_end_islands", minecraft::small_end_islands},
        {u8"minecraft:end_midlands", minecraft::end_midlands},
        {u8"minecraft:end_highlands", minecraft::end_highlands},
        {u8"minecraft:end_barrens", minecraft::end_barrens},
        {u8"minecraft:warm_ocean", minecraft::warm_ocean},
        {u8"minecraft:lukewarm_ocean", minecraft::lukewarm_ocean},
        {u8"minecraft:cold_ocean", minecraft::cold_ocean},
        {u8"minecraft:deep_warm_ocean", minecraft::deep_warm_ocean},
        {u8"minecraft:deep_lukewarm_ocean", minecraft::deep_lukewarm_ocean},
        {u8"minecraft:deep_cold_ocean", minecraft::deep_cold_ocean},
        {u8"minecraft:deep_frozen_ocean", minecraft::deep_frozen_ocean},
        {u8"minecraft:the_void", minecraft::the_void},
        {u8"minecraft:sunflower_plains", minecraft::sunflower_plains},
        {u8"minecraft:desert_lakes", minecraft::desert_lakes},
        {u8"minecraft:gravelly_mountains", minecraft::windswept_gravelly_hills},
        {u8"minecraft:windswept_gravelly_hills", minecraft::windswept_gravelly_hills},
        {u8"minecraft:flower_forest", minecraft::flower_forest},
        {u8"minecraft:taiga_mountains", minecraft::taiga_mountains},
        {u8"minecraft:swamp_hills", minecraft::swamp_hills},
        {u8"minecraft:ice_spikes", minecraft::ice_spikes},
        {u8"minecraft:modified_jungle", minecraft::modified_jungle},
        {u8"minecraft:modified_jungle_edge", minecraft::modified_jungle_edge},
        {u8"minecraft:tall_birch_forest", minecraft::old_growth_birch_forest},       // < 1.18
        {u8"minecraft:old_growth_birch_forest", minecraft::old_growth_birch_forest}, // >= 1.18
        {u8"minecraft:tall_birch_hills", minecraft::tall_birch_hills},
        {u8"minecraft:dark_forest_hills", minecraft::dark_forest_hills},
        {u8"minecraft:snowy_taiga_mountains", minecraft::snowy_taiga_mountains},
        {u8"minecraft:giant_spruce_taiga", minecraft::old_growth_spruce_taiga},      // < 1.18
        {u8"minecraft:old_growth_spruce_taiga", minecraft::old_growth_spruce_taiga}, // >= 1.18
        {u8"minecraft:giant_spruce_taiga_hills", minecraft::giant_spruce_taiga_hills},
        {u8"minecraft:modified_gravelly_mountains", minecraft::modified_gravelly_mountains},
        {u8"minecraft:shattered_savanna", minecraft::windswept_savanna}, // < 1.18
        {u8"minecraft:windswept_savanna", minecraft::windswept_savanna}, // >= 1.18
        {u8"minecraft:shattered_savanna_plateau", minecraft::shattered_savanna_plateau},
        {u8"minecraft:eroded_badlands", minecraft::eroded_badlands},
        {u8"minecraft:modified_wooded_badlands_plateau", minecraft::modified_wooded_badlands_plateau},
        {u8"minecraft:modified_badlands_plateau", minecraft::modified_badlands_plateau},
        {u8"minecraft:bamboo_jungle", minecraft::bamboo_jungle},
        {u8"minecraft:bamboo_jungle_hills", minecraft::bamboo_jungle_hills},

        // 20w06a
        {u8"minecraft:soul_sand_valley", minecraft::soul_sand_valley},
        {u8"minecraft:crimson_forest", minecraft::crimson_forest},
        {u8"minecraft:warped_forest", minecraft::warped_forest},

        // 20w15a
        {u8"minecraft:basalt_deltas", minecraft::basalt_deltas},

        // 1.18
        {u8"minecraft:dripstone_caves", minecraft::dripstone_caves},
        {u8"minecraft:lush_caves", minecraft::lush_caves},
        {u8"minecraft:snowy_slopes", minecraft::snowy_slopes},
        {u8"minecraft:grove", minecraft::grove},
        {u8"minecraft:jagged_peaks", minecraft::jagged_peaks},
        {u8"minecraft:meadow", minecraft::meadow},
        {u8"minecraft:frozen_peaks", minecraft::frozen_peaks},
        {u8"minecraft:stony_peaks", minecraft::stony_peaks},

        // 1.19
        {u8"minecraft:deep_dark", minecraft::deep_dark},
        {u8"minecraft:mangrove_swamp", minecraft::mangrove_swamp},

        // 1.20
        {u8"minecraft:cherry_grove", minecraft::cherry_grove},
    };
    auto found = sMapping.find(name);
    if (found == sMapping.end()) {
        assert(false);
        return std::nullopt;
    } else {
        return found->second;
    }
}
} // namespace mcfile::biomes
