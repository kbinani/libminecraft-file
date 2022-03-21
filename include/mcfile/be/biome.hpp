#pragma once

namespace mcfile::be {

class Biome {
public:
    static biomes::BiomeId FromUint32(uint32_t raw) {
        using namespace biomes::minecraft;
        switch (raw) {
        case 0: return ocean;
        case 47: return frozen_ocean; // legacy_frozen_ocean
        case 24: return deep_ocean;
        case 10: return frozen_ocean;
        case 46: return deep_frozen_ocean;
        case 42: return cold_ocean;
        case 45: return deep_cold_ocean;
        case 41: return lukewarm_ocean;
        case 44: return deep_lukewarm_ocean;
        case 40: return warm_ocean;
        case 43: return deep_warm_ocean;
        case 7: return river;
        case 11: return frozen_river;
        case 16: return beach;
        case 25: return stony_shore;
        case 26: return snowy_beach;
        case 4: return forest;
        case 18: return wooded_hills;
        case 132: return flower_forest;
        case 27: return birch_forest;
        case 28: return birch_forest_hills;
        case 155: return old_growth_birch_forest;
        case 156: return tall_birch_hills;
        case 29: return dark_forest;
        case 157: return dark_forest_hills;
        case 21: return jungle;
        case 22: return jungle_hills;
        case 149: return jungle_hills; // jungle_mutated;
        case 23: return sparse_jungle;
        case 151: return sparse_jungle; // jungle_edge_mutated;
        case 168: return bamboo_jungle;
        case 169: return bamboo_jungle_hills;
        case 5: return taiga;
        case 19: return taiga_hills;
        case 133: return taiga_mountains;
        case 30: return snowy_taiga;
        case 31: return snowy_taiga_hills;
        case 158: return snowy_taiga_mountains;
        case 32: return old_growth_pine_taiga;
        case 33: return giant_tree_taiga_hills;
        case 160: return old_growth_spruce_taiga;
        case 161: return giant_spruce_taiga_hills;
        case 14: return mushroom_fields;
        case 15: return mushroom_fields; // was mushroom_field_shore before 21w40a
        case 6: return swamp;
        case 134: return swamp_hills;
        case 35: return savanna;
        case 36: return savanna_plateau;
        case 163: return windswept_savanna;
        case 164: return shattered_savanna_plateau;
        case 1: return plains;
        case 129: return sunflower_plains;
        case 2: return desert;
        case 17: return desert_hills;
        case 130: return desert_lakes;
        case 12: return snowy_plains;
        case 13: return snowy_mountains;
        case 140: return ice_spikes;
        case 3: return windswept_hills;
        case 34: return windswept_forest;
        case 131: return windswept_gravelly_hills;
        case 162: return windswept_gravelly_hills; // extreme_hills_plus_trees_mutated;
        case 20: return windswept_hills;           // extreme_hills_edge;
        case 37: return badlands;
        case 38: return badlands_plateau;
        case 166: return modified_badlands_plateau;
        case 39: return wooded_badlands;
        case 167: return modified_wooded_badlands_plateau;
        case 165: return eroded_badlands;
        case 186: return meadow;
        case 185: return grove;
        case 184: return snowy_slopes;
        case 182: return jagged_peaks;
        case 183: return frozen_peaks;
        case 189: return stony_peaks;
        case 187: return lush_caves;
        case 188: return dripstone_caves;
        case 8: return nether_wastes;
        case 179: return crimson_forest;
        case 180: return warped_forest;
        case 178: return soul_sand_valley;
        case 181: return basalt_deltas;
        case 9: return the_end;
        default:
            return ocean;
        }
    }

    static uint32_t ToUint32(mcfile::biomes::BiomeId biome) {
        using namespace mcfile::biomes::minecraft;
        switch (biome) {
        case ocean: return 0;
        //case legacy_frozen_ocean: return 47;
        case deep_ocean: return 24;
        case frozen_ocean: return 10;
        case deep_frozen_ocean: return 46;
        case cold_ocean: return 42;
        case deep_cold_ocean: return 45;
        case lukewarm_ocean: return 41;
        case deep_lukewarm_ocean: return 44;
        case warm_ocean: return 40;
        case deep_warm_ocean: return 43;
        case river: return 7;
        case frozen_river: return 11;
        case beach: return 16;
        case stony_shore: return 25; // stone_beach
        case snowy_beach: return 26; // cold_beach
        case forest: return 4;
        case wooded_hills: return 18; // forest_hills
        case flower_forest: return 132;
        case birch_forest: return 27;
        case birch_forest_hills: return 28;
        case old_growth_birch_forest: return 155; // birch_forest_mutated
        case tall_birch_hills: return 156;        // birch_forest_hills_mutated
        case dark_forest: return 29;              // roofed_forest
        case dark_forest_hills: return 157;       // roofed_forest_mutated
        case jungle: return 21;
        case jungle_hills: return 22;
        case modified_jungle: return 149; // jungle_mutated
        case sparse_jungle:
            return 23;                         // jungle_edge
        case modified_jungle_edge: return 151; // jungle_edge_mutated
        case bamboo_jungle: return 21;         // 168;
        case bamboo_jungle_hills: return 22;   // 169;
        case taiga: return 5;
        case taiga_hills: return 19;
        case taiga_mountains: return 133;          // taiga_mutated
        case snowy_taiga: return 30;               // cold_taiga
        case snowy_taiga_hills: return 31;         // cold_taiga_hills
        case snowy_taiga_mountains: return 158;    // cold_taiga_mutated
        case old_growth_pine_taiga: return 32;     // mega_taiga
        case giant_tree_taiga_hills: return 33;    // mega_taiga_hills
        case old_growth_spruce_taiga: return 160;  // redwood_taiga_mutated
        case giant_spruce_taiga_hills: return 161; // redwood_taiga_hills_mutated
        case mushroom_fields: return 14;           // mushroom_island
        case mushroom_field_shore: return 15;      // mushroom_island_shore
        case swamp: return 6;                      // swampland
        case swamp_hills: return 134;              // swampland_mutated
        case savanna: return 35;
        case savanna_plateau: return 36;
        case windswept_savanna: return 163;         // savanna_mutated
        case shattered_savanna_plateau: return 164; // savanna_plateau_mutated
        case plains: return 1;
        case sunflower_plains: return 129;
        case desert: return 2;
        case desert_hills: return 17;
        case desert_lakes: return 130;   // desert_mutated
        case snowy_plains: return 12;    // ice_plains
        case snowy_mountains: return 13; // ice_mountains
        case ice_spikes: return 140;     // ice_plains_spikes
        case windswept_hills:
        case mountain_edge:
            return 3;                     // extreme_hills
        case windswept_forest: return 34; // extreme_hills_plus_trees
        case windswept_gravelly_hills:
        case modified_gravelly_mountains:
            return 131; // extreme_hills_mutated
        //case extreme_hills_plus_trees_mutated: return 162;
        //case extreme_hills_edge: return 20;
        case badlands: return 37;                          // mesa
        case badlands_plateau: return 38;                  // mesa_plateau
        case modified_badlands_plateau: return 166;        // mesa_plateau_mutated
        case wooded_badlands: return 39;                   // mesa_plateau_stone
        case modified_wooded_badlands_plateau: return 167; // mesa_plateau_stone_mutated
        case eroded_badlands: return 165;                  // mesa_bryce
        case meadow: return 186;
        case grove: return 185;
        case snowy_slopes: return 184;
        case jagged_peaks: return 182;
        case frozen_peaks: return 183;
        case stony_peaks: return 189;
        case lush_caves: return 187;
        case dripstone_caves: return 188;
        case nether_wastes: return 8; // hell
        case crimson_forest: return 179;
        case warped_forest: return 180;
        case soul_sand_valley: return 178; // soulsand_valley
        case basalt_deltas: return 181;
        case the_end:
        case end_barrens:
        case end_highlands:
        case end_midlands:
        case small_end_islands:
            return 9;
        default:
            return 0;
        }
    }

private:
    Biome() = delete;
};

} // namespace mcfile::be
