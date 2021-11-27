#pragma once
namespace mcfile::biomes {

static inline std::optional<mcfile::biomes::BiomeId> FromName(std::string const &name) {
    using namespace std;

    static unordered_map<string, BiomeId> const sMapping = {
        {"minecraft:nether_wastes", minecraft::nether_wastes},
        {"minecraft:nether", minecraft::nether_wastes},

        {"minecraft:ocean", minecraft::ocean},
        {"minecraft:plains", minecraft::plains},
        {"minecraft:desert", minecraft::desert},
        {"minecraft:windswept_hills", minecraft::windswept_hills}, // >= 1.18
        {"minecraft:mountains", minecraft::windswept_hills},       // < 1.18
        {"minecraft:forest", minecraft::forest},
        {"minecraft:taiga", minecraft::taiga},
        {"minecraft:swamp", minecraft::swamp},
        {"minecraft:river", minecraft::river},
        {"minecraft:the_end", minecraft::the_end},
        {"minecraft:frozen_ocean", minecraft::frozen_ocean},
        {"minecraft:frozen_river", minecraft::frozen_river},
        {"minecraft:snowy_plains", minecraft::snowy_plains},
        {"minecraft:snowy_tundra", minecraft::snowy_plains},
        {"minecraft:snowy_mountains", minecraft::snowy_mountains},
        {"minecraft:mushroom_fields", minecraft::mushroom_fields},
        {"minecraft:mushroom_field_shore", minecraft::mushroom_field_shore},
        {"minecraft:beach", minecraft::beach},
        {"minecraft:desert_hills", minecraft::desert_hills},
        {"minecraft:wooded_hills", minecraft::wooded_hills},
        {"minecraft:taiga_hills", minecraft::taiga_hills},
        {"minecraft:mountain_edge", minecraft::mountain_edge},
        {"minecraft:jungle", minecraft::jungle},
        {"minecraft:jungle_hills", minecraft::jungle_hills},
        {"minecraft:jungle_edge", minecraft::sparse_jungle},   // < 1.18
        {"minecraft:sparse_jungle", minecraft::sparse_jungle}, // >= 1/18
        {"minecraft:deep_ocean", minecraft::deep_ocean},
        {"minecraft:stone_shore", minecraft::stony_shore}, // < 1.18
        {"minecraft:stony_shore", minecraft::stony_shore}, // >= 1.18
        {"minecraft:snowy_beach", minecraft::snowy_beach},
        {"minecraft:birch_forest", minecraft::birch_forest},
        {"minecraft:birch_forest_hills", minecraft::birch_forest_hills},
        {"minecraft:dark_forest", minecraft::dark_forest},
        {"minecraft:snowy_taiga", minecraft::snowy_taiga},
        {"minecraft:snowy_taiga_hills", minecraft::snowy_taiga_hills},
        {"minecraft:giant_tree_taiga", minecraft::old_growth_pine_taiga},      // < 1.18
        {"minecraft:old_growth_pine_taiga", minecraft::old_growth_pine_taiga}, // >= 1.18
        {"minecraft:giant_tree_taiga_hills", minecraft::giant_tree_taiga_hills},
        {"minecraft:wooded_mountains", minecraft::windswept_forest}, // < 1.18
        {"minecraft:windswept_forest", minecraft::windswept_forest}, // >= 1.18
        {"minecraft:savanna", minecraft::savanna},
        {"minecraft:savanna_plateau", minecraft::savanna_plateau},
        {"minecraft:badlands", minecraft::badlands},
        {"minecraft:wooded_badlands_plateau", minecraft::wooded_badlands}, // < 1.18
        {"minecraft:wooded_badlands", minecraft::wooded_badlands},         // >= 1/18
        {"minecraft:badlands_plateau", minecraft::badlands_plateau},
        {"minecraft:small_end_islands", minecraft::small_end_islands},
        {"minecraft:end_midlands", minecraft::end_midlands},
        {"minecraft:end_highlands", minecraft::end_highlands},
        {"minecraft:end_barrens", minecraft::end_barrens},
        {"minecraft:warm_ocean", minecraft::warm_ocean},
        {"minecraft:lukewarm_ocean", minecraft::lukewarm_ocean},
        {"minecraft:cold_ocean", minecraft::cold_ocean},
        {"minecraft:deep_warm_ocean", minecraft::deep_warm_ocean},
        {"minecraft:deep_lukewarm_ocean", minecraft::deep_lukewarm_ocean},
        {"minecraft:deep_cold_ocean", minecraft::deep_cold_ocean},
        {"minecraft:deep_frozen_ocean", minecraft::deep_frozen_ocean},
        {"minecraft:the_void", minecraft::the_void},
        {"minecraft:sunflower_plains", minecraft::sunflower_plains},
        {"minecraft:desert_lakes", minecraft::desert_lakes},
        {"minecraft:gravelly_mountains", minecraft::windswept_gravelly_hills},
        {"minecraft:windswept_gravelly_hills", minecraft::windswept_gravelly_hills},
        {"minecraft:flower_forest", minecraft::flower_forest},
        {"minecraft:taiga_mountains", minecraft::taiga_mountains},
        {"minecraft:swamp_hills", minecraft::swamp_hills},
        {"minecraft:ice_spikes", minecraft::ice_spikes},
        {"minecraft:modified_jungle", minecraft::modified_jungle},
        {"minecraft:modified_jungle_edge", minecraft::modified_jungle_edge},
        {"minecraft:tall_birch_forest", minecraft::old_growth_birch_forest},       // < 1.18
        {"minecraft:old_growth_birch_forest", minecraft::old_growth_birch_forest}, // >= 1.18
        {"minecraft:tall_birch_hills", minecraft::tall_birch_hills},
        {"minecraft:dark_forest_hills", minecraft::dark_forest_hills},
        {"minecraft:snowy_taiga_mountains", minecraft::snowy_taiga_mountains},
        {"minecraft:giant_spruce_taiga", minecraft::old_growth_spruce_taiga},      // < 1.18
        {"minecraft:old_growth_spruce_taiga", minecraft::old_growth_spruce_taiga}, // >= 1.18
        {"minecraft:giant_spruce_taiga_hills", minecraft::giant_spruce_taiga_hills},
        {"minecraft:modified_gravelly_mountains", minecraft::modified_gravelly_mountains},
        {"minecraft:shattered_savanna", minecraft::windswept_savanna}, // < 1.18
        {"minecraft:windswept_savanna", minecraft::windswept_savanna}, // >= 1.18
        {"minecraft:shattered_savanna_plateau", minecraft::shattered_savanna_plateau},
        {"minecraft:eroded_badlands", minecraft::eroded_badlands},
        {"minecraft:modified_wooded_badlands_plateau", minecraft::modified_wooded_badlands_plateau},
        {"minecraft:modified_badlands_plateau", minecraft::modified_badlands_plateau},
        {"minecraft:bamboo_jungle", minecraft::bamboo_jungle},
        {"minecraft:bamboo_jungle_hills", minecraft::bamboo_jungle_hills},

        // 20w06a
        {"minecraft:soul_sand_valley", minecraft::soul_sand_valley},
        {"minecraft:crimson_forest", minecraft::crimson_forest},
        {"minecraft:warped_forest", minecraft::warped_forest},

        // 20w15a
        {"minecraft:basalt_deltas", minecraft::basalt_deltas},

        // 1.18
        {"minecraft:dripstone_caves", minecraft::dripstone_caves},
        {"minecraft:lush_caves", minecraft::lush_caves},
        {"minecraft:snowy_slopes", minecraft::snowy_slopes},
        {"minecraft:grove", minecraft::grove},
        {"minecraft:jagged_peaks", minecraft::jagged_peaks},
        {"minecraft:meadow", minecraft::meadow},
        {"minecraft:frozen_peaks", minecraft::frozen_peaks},
        {"minecraft:stony_peaks", minecraft::stony_peaks},
    };
    auto found = sMapping.find(name);
    if (found == sMapping.end()) {
        return std::nullopt;
    } else {
        return found->second;
    }
}
} // namespace mcfile::biomes
