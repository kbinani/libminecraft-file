#pragma once

namespace mcfile {
namespace biomes {
namespace minecraft {

enum : BiomeId {
    ocean = 0,
    plains = 1,
    desert = 2,
    windswept_hills = 3, // was "mountains" before 1.18
    forest = 4,
    taiga = 5,
    swamp = 6,
    river = 7,
    nether_wastes = 8,
    the_end = 9,
    frozen_ocean = 10,
    frozen_river = 11,
    snowy_plains = 12, // was "snowy_tundra" before 1.18
    snowy_mountains = 13,
    mushroom_fields = 14,
    mushroom_field_shore = 15,
    beach = 16,
    desert_hills = 17,
    wooded_hills = 18,
    taiga_hills = 19,
    mountain_edge = 20,
    jungle = 21,
    jungle_hills = 22,
    sparse_jungle = 23, // was "jungle_edge" before 1.18
    deep_ocean = 24,
    stony_shore = 25, // was "stone_shore" before 1.18
    snowy_beach = 26,
    birch_forest = 27,
    birch_forest_hills = 28,
    dark_forest = 29,
    snowy_taiga = 30,
    snowy_taiga_hills = 31,
    old_growth_pine_taiga = 32, // was "giant_tree_taiga" before 1.18
    giant_tree_taiga_hills = 33,
    windswept_forest = 34, // was "wooded_mountains" before 1.18
    savanna = 35,
    savanna_plateau = 36,
    badlands = 37,
    wooded_badlands = 38, // was "wooded_badlands_plateau" before 1.18
    badlands_plateau = 39,
    small_end_islands = 40,
    end_midlands = 41,
    end_highlands = 42,
    end_barrens = 43,
    warm_ocean = 44,
    lukewarm_ocean = 45,
    cold_ocean = 46,
    deep_warm_ocean = 47,
    deep_lukewarm_ocean = 48,
    deep_cold_ocean = 49,
    deep_frozen_ocean = 50,
    the_void = 127,
    sunflower_plains = 129,
    desert_lakes = 130,
    windswept_gravelly_hills = 131, // was "gravelly_mountains" before 1.18
    flower_forest = 132,
    taiga_mountains = 133,
    swamp_hills = 134,
    ice_spikes = 140,
    modified_jungle = 149,
    modified_jungle_edge = 151,
    old_growth_birch_forest = 155, // was "tall_birch_forest" before 1.18
    tall_birch_hills = 156,
    dark_forest_hills = 157,
    snowy_taiga_mountains = 158,
    old_growth_spruce_taiga = 160, // was "giant_spruce_taiga" before 1.18
    giant_spruce_taiga_hills = 161,
    modified_gravelly_mountains = 162,
    windswept_savanna = 163, // was "shattered_savanna" before 1.18
    shattered_savanna_plateau = 164,
    eroded_badlands = 165,
    modified_wooded_badlands_plateau = 166,
    modified_badlands_plateau = 167,
    bamboo_jungle = 168,
    bamboo_jungle_hills = 169,

    // 20w06a
    soul_sand_valley = 170,
    crimson_forest = 171,
    warped_forest = 172,

    // 20w15a
    basalt_deltas = 173,

    // 1.18
    dripstone_caves = 174,
    lush_caves = 175,

    // not numbered in >= 1.18
    snowy_slopes,
    grove,
    jagged_peaks,
    meadow,
    frozen_peaks,
    stony_peaks,

    minecraft_max_biome_id
};

} // namespace minecraft
} // namespace biomes
} // namespace mcfile
