#pragma once

namespace mcfile::biomes::minecraft {

enum : BiomeId {
    ocean = 1,
    plains,
    desert,
    windswept_hills, // was "mountains" before 1.18
    forest,
    taiga,
    swamp,
    river,
    nether_wastes,
    the_end, // 10
    frozen_ocean,
    frozen_river,
    snowy_plains, // was "snowy_tundra" before 1.18
    snowy_mountains,
    mushroom_fields,
    mushroom_field_shore, // removed at 21w40a (1.18)
    beach,
    desert_hills,
    wooded_hills,
    taiga_hills, // 20
    mountain_edge,
    jungle,
    jungle_hills,
    sparse_jungle, // was "jungle_edge" before 1.18
    deep_ocean,
    stony_shore, // was "stone_shore" before 1.18
    snowy_beach,
    birch_forest,
    birch_forest_hills,
    dark_forest, // 30
    snowy_taiga,
    snowy_taiga_hills,
    old_growth_pine_taiga, // was "giant_tree_taiga" before 1.18
    giant_tree_taiga_hills,
    windswept_forest, // was "wooded_mountains" before 1.18
    savanna,
    savanna_plateau,
    badlands,
    wooded_badlands,  // was "wooded_badlands_plateau" before 1.18
    badlands_plateau, // 40
    small_end_islands,
    end_midlands,
    end_highlands,
    end_barrens,
    warm_ocean,
    lukewarm_ocean,
    cold_ocean,
    deep_warm_ocean,
    deep_lukewarm_ocean,
    deep_cold_ocean, // 50
    deep_frozen_ocean,
    the_void,
    sunflower_plains,
    desert_lakes,
    windswept_gravelly_hills, // was "gravelly_mountains" before 1.18
    flower_forest,
    taiga_mountains,
    swamp_hills,
    ice_spikes,
    modified_jungle, // 60
    modified_jungle_edge,
    old_growth_birch_forest, // was "tall_birch_forest" before 1.18
    tall_birch_hills,
    dark_forest_hills,
    snowy_taiga_mountains,
    old_growth_spruce_taiga, // was "giant_spruce_taiga" before 1.18
    giant_spruce_taiga_hills,
    modified_gravelly_mountains,
    windswept_savanna,         // was "shattered_savanna" before 1.18
    shattered_savanna_plateau, // 70
    eroded_badlands,
    modified_wooded_badlands_plateau,
    modified_badlands_plateau,
    bamboo_jungle,
    bamboo_jungle_hills,

    // 20w06a
    soul_sand_valley,
    crimson_forest,
    warped_forest,

    // 20w15a
    basalt_deltas,

    // 1.18
    dripstone_caves, // 80
    lush_caves,

    // not numbered in >= 1.18
    snowy_slopes,
    grove,
    jagged_peaks,
    meadow,
    frozen_peaks,
    stony_peaks,

    // 1.19
    deep_dark,
    mangrove_swamp,

    // 1.20
    cherry_grove,

    minecraft_max_biome_id

    // Functions need to update when new biomes will be released:
    // - mcfile::biomes::FromName
    // - mcfile::biomes::Name
    // - mcfile::be::Biome::FromUint32
    // - mcfile::be::Biome::ToUint32
};

}
