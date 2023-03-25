#pragma once

namespace mcfile {
namespace biomes {

static inline std::u8string Name(BiomeId id, int chunkDataVersion) {
    if (id == minecraft::nether_wastes) {
        if (chunkDataVersion >= 2504) {
            // 20w06a
            return u8"minecraft:nether_wastes";
        } else {
            return u8"minecraft:nether";
        }
    } else if (id == minecraft::snowy_plains) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:snowy_tundra";
        } else {
            return u8"minecraft:snowy_plains";
        }
    } else if (id == minecraft::windswept_hills) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:mountains";
        } else {
            return u8"minecraft:windswept_hills";
        }
    } else if (id == minecraft::windswept_gravelly_hills) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:gravelly_mountains";
        } else {
            return u8"minecraft:windswept_gravelly_hills";
        }
    } else if (id == minecraft::windswept_forest) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:wooded_mountains";
        } else {
            return u8"minecraft:windswept_forest";
        }
    } else if (id == minecraft::old_growth_pine_taiga) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:giant_tree_taiga";
        } else {
            return u8"minecraft:old_growth_pine_taiga";
        }
    } else if (id == minecraft::old_growth_spruce_taiga) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:giant_spruce_taiga";
        } else {
            return u8"minecraft:old_growth_spruce_taiga";
        }
    } else if (id == minecraft::stony_shore) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:stone_shore";
        } else {
            return u8"minecraft:stony_shore";
        }
    } else if (id == minecraft::windswept_savanna) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:shattered_savanna";
        } else {
            return u8"minecraft:windswept_savanna";
        }
    } else if (id == minecraft::sparse_jungle) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:jungle_edge";
        } else {
            return u8"minecraft:sparse_jungle";
        }
    } else if (id == minecraft::wooded_badlands) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:wooded_badlands_plateau";
        } else {
            return u8"minecraft:wooded_badlands";
        }
    } else if (id == minecraft::old_growth_birch_forest) {
        if (chunkDataVersion <= 2840) {
            return u8"minecraft:tall_birch_forest";
        } else {
            return u8"minecraft:old_growth_birch_forest";
        }
    } else if (id == minecraft::mushroom_field_shore) {
        if (chunkDataVersion < 2838) {
            return u8"minecraft:mushroom_field_shore";
        } else {
            return u8"minecraft:mushroom_fields";
        }
    }

    static std::map<BiomeId, std::u8string> const mapping = {
        {minecraft::ocean, u8"minecraft:ocean"},
        {minecraft::plains, u8"minecraft:plains"},
        {minecraft::desert, u8"minecraft:desert"},
        {minecraft::forest, u8"minecraft:forest"},
        {minecraft::taiga, u8"minecraft:taiga"},
        {minecraft::swamp, u8"minecraft:swamp"},
        {minecraft::river, u8"minecraft:river"},
        {minecraft::the_end, u8"minecraft:the_end"},
        {minecraft::frozen_ocean, u8"minecraft:frozen_ocean"},
        {minecraft::frozen_river, u8"minecraft:frozen_river"},
        {minecraft::snowy_mountains, u8"minecraft:snowy_mountains"},
        {minecraft::mushroom_fields, u8"minecraft:mushroom_fields"},
        {minecraft::mushroom_field_shore, u8"minecraft:mushroom_field_shore"},
        {minecraft::beach, u8"minecraft:beach"},
        {minecraft::desert_hills, u8"minecraft:desert_hills"},
        {minecraft::wooded_hills, u8"minecraft:wooded_hills"},
        {minecraft::taiga_hills, u8"minecraft:taiga_hills"},
        {minecraft::mountain_edge, u8"minecraft:mountain_edge"},
        {minecraft::jungle, u8"minecraft:jungle"},
        {minecraft::jungle_hills, u8"minecraft:jungle_hills"},
        {minecraft::deep_ocean, u8"minecraft:deep_ocean"},
        {minecraft::snowy_beach, u8"minecraft:snowy_beach"},
        {minecraft::birch_forest, u8"minecraft:birch_forest"},
        {minecraft::birch_forest_hills, u8"minecraft:birch_forest_hills"},
        {minecraft::dark_forest, u8"minecraft:dark_forest"},
        {minecraft::snowy_taiga, u8"minecraft:snowy_taiga"},
        {minecraft::snowy_taiga_hills, u8"minecraft:snowy_taiga_hills"},
        {minecraft::giant_tree_taiga_hills, u8"minecraft:giant_tree_taiga_hills"},
        {minecraft::savanna, u8"minecraft:savanna"},
        {minecraft::savanna_plateau, u8"minecraft:savanna_plateau"},
        {minecraft::badlands, u8"minecraft:badlands"},
        {minecraft::badlands_plateau, u8"minecraft:badlands_plateau"},
        {minecraft::small_end_islands, u8"minecraft:small_end_islands"},
        {minecraft::end_midlands, u8"minecraft:end_midlands"},
        {minecraft::end_highlands, u8"minecraft:end_highlands"},
        {minecraft::end_barrens, u8"minecraft:end_barrens"},
        {minecraft::warm_ocean, u8"minecraft:warm_ocean"},
        {minecraft::lukewarm_ocean, u8"minecraft:lukewarm_ocean"},
        {minecraft::cold_ocean, u8"minecraft:cold_ocean"},
        {minecraft::deep_warm_ocean, u8"minecraft:deep_warm_ocean"},
        {minecraft::deep_lukewarm_ocean, u8"minecraft:deep_lukewarm_ocean"},
        {minecraft::deep_cold_ocean, u8"minecraft:deep_cold_ocean"},
        {minecraft::deep_frozen_ocean, u8"minecraft:deep_frozen_ocean"},
        {minecraft::the_void, u8"minecraft:the_void"},
        {minecraft::sunflower_plains, u8"minecraft:sunflower_plains"},
        {minecraft::desert_lakes, u8"minecraft:desert_lakes"},
        {minecraft::flower_forest, u8"minecraft:flower_forest"},
        {minecraft::taiga_mountains, u8"minecraft:taiga_mountains"},
        {minecraft::swamp_hills, u8"minecraft:swamp_hills"},
        {minecraft::ice_spikes, u8"minecraft:ice_spikes"},
        {minecraft::modified_jungle, u8"minecraft:modified_jungle"},
        {minecraft::modified_jungle_edge, u8"minecraft:modified_jungle_edge"},
        {minecraft::tall_birch_hills, u8"minecraft:tall_birch_hills"},
        {minecraft::dark_forest_hills, u8"minecraft:dark_forest_hills"},
        {minecraft::snowy_taiga_mountains, u8"minecraft:snowy_taiga_mountains"},
        {minecraft::giant_spruce_taiga_hills, u8"minecraft:giant_spruce_taiga_hills"},
        {minecraft::modified_gravelly_mountains, u8"minecraft:modified_gravelly_mountains"},
        {minecraft::shattered_savanna_plateau, u8"minecraft:shattered_savanna_plateau"},
        {minecraft::eroded_badlands, u8"minecraft:eroded_badlands"},
        {minecraft::modified_wooded_badlands_plateau, u8"minecraft:modified_wooded_badlands_plateau"},
        {minecraft::modified_badlands_plateau, u8"minecraft:modified_badlands_plateau"},
        {minecraft::bamboo_jungle, u8"minecraft:bamboo_jungle"},
        {minecraft::bamboo_jungle_hills, u8"minecraft:bamboo_jungle_hills"},

        // 20w06a
        {minecraft::soul_sand_valley, u8"minecraft:soul_sand_valley"},
        {minecraft::crimson_forest, u8"minecraft:crimson_forest"},
        {minecraft::warped_forest, u8"minecraft:warped_forest"},

        // 20w15a
        {minecraft::basalt_deltas, u8"minecraft:basalt_deltas"},

        // 1.18
        {minecraft::dripstone_caves, u8"minecraft:dripstone_caves"},
        {minecraft::lush_caves, u8"minecraft:lush_caves"},
        {minecraft::snowy_slopes, u8"minecraft:snowy_slopes"},
        {minecraft::grove, u8"minecraft:grove"},
        {minecraft::jagged_peaks, u8"minecraft:jagged_peaks"},
        {minecraft::meadow, u8"minecraft:meadow"},
        {minecraft::frozen_peaks, u8"minecraft:frozen_peaks"},
        {minecraft::stony_peaks, u8"minecraft:stony_peaks"},

        // 1.19
        {minecraft::deep_dark, u8"minecraft:deep_dark"},
        {minecraft::mangrove_swamp, u8"minecraft:mangrove_swamp"},

        // 1.20
        {minecraft::cherry_grove, u8"minecraft:cherry_grove"},
    };
    auto mappingIt = mapping.find(id);
    if (mappingIt == mapping.end()) {
        assert(false);
        return u8"";
    }
    return mappingIt->second;
}

} // namespace biomes
} // namespace mcfile
