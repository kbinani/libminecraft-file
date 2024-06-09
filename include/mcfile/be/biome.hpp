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
        case 18: return wooded_hills; // forest_hills not used in BE. replacing to wooded_hills
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
        case 190: return deep_dark;
        case 191: return mangrove_swamp;
        case 192: return cherry_grove;
        default:
            return ocean;
        }
    }

    static uint32_t ToUint32(mcfile::biomes::BiomeId biome) {
        using namespace mcfile::biomes::minecraft;
        switch (biome) {
        case ocean: return 0;
        // case legacy_frozen_ocean: return 47;
        case deep_ocean: return 24;
        case frozen_ocean: return 10;
        case deep_frozen_ocean: return 46;
        case cold_ocean: return 42;
        case deep_cold_ocean: return 45;
        case lukewarm_ocean: return 41;
        case deep_lukewarm_ocean: return 44;
        case warm_ocean: return 40;
        case deep_warm_ocean: return 40; // deep_warm_ocean=43 is not used in BE. replacing to warm_ocean=40
        case river: return 7;
        case frozen_river: return 11;
        case beach: return 16;
        case stony_shore: return 25; // stone_beach
        case snowy_beach: return 26; // cold_beach
        case forest: return 4;
        case wooded_hills: return 18; // forest_hills
        case flower_forest: return 132;
        case birch_forest: return 27;
        case birch_forest_hills: return 27;       // birch_forest_hills=28 is not used in BE. replacing to birch_forest=27
        case old_growth_birch_forest: return 155; // birch_forest_mutated=155
        case tall_birch_hills: return 155;        // birch_forest_hills_mutated=156 is not used in BE. replacing to birch_forest_mutated=155
        case dark_forest: return 29;              // roofed_forest=29
        case dark_forest_hills: return 29;        // roofed_forest_mutated=157 is not used in BE. replacing to roofed_forest=29
        case jungle: return 21;
        case jungle_hills: return 21;         // jungle_hills=22 is not used in BE. replacing to jungle=21
        case modified_jungle: return 21;      // jungle_mutated=149 is not used in BE. replacing to jungle=21
        case sparse_jungle: return 23;        // jungle_edge=23
        case modified_jungle_edge: return 23; // jungle_edge_mutated=151 is not used in BE. replacing to jungle_edge=23
        case bamboo_jungle: return 168;
        case bamboo_jungle_hills: return 168; // bamboo_jungle_hills=169 is not used in BE. replacing to bamboo_jungle=168
        case taiga: return 5;
        case taiga_hills: return 5;                // taig_hills=19 is not used in BE. replacing to taiga=5
        case taiga_mountains: return 5;            //         taiga_mutated=133 is not used in BE. replacing to taiga=5
        case snowy_taiga: return 30;               // cold_taiga
        case snowy_taiga_hills: return 30;         // cold_taiga_hills=31 is not used in BE. replacing to cold_taig=30
        case snowy_taiga_mountains: return 30;     // cold_taiga_mutated=158 is not used in BE. replacing to cold_taig=30
        case old_growth_pine_taiga: return 32;     // mega_taiga
        case giant_tree_taiga_hills: return 32;    // mega_taiga_hills=33 is not used in BE. replacing to mega_taiga=32
        case old_growth_spruce_taiga: return 160;  // redwood_taiga_mutated
        case giant_spruce_taiga_hills: return 160; // redwood_taiga_hills_mutated=161 is not used in BE. replacing to redwood_taiga_mutated=160
        case mushroom_fields: return 14;           // mushroom_island
        case mushroom_field_shore: return 14;      // mushroom_island_shore=15 is not used in BE. replacing to mushroom_island=14
        case swamp: return 6;                      // swampland
        case swamp_hills: return 6;                // swampland_mutated=134 is not used in BE. replacing to swampland=6
        case savanna: return 35;
        case savanna_plateau: return 36;
        case windswept_savanna: return 163;         // savanna_mutated
        case shattered_savanna_plateau: return 163; // savanna_plateau_mutated=164 is not used in BE. replacing to savanna_mutated=163
        case plains: return 1;
        case sunflower_plains: return 129;
        case desert: return 2;
        case desert_hills: return 2;     // desert_hills=17 is not used in BE. replacing to desert=2
        case desert_lakes: return 2;     // desert_mutated=130 is not used in BE. replacing to desert=2
        case snowy_plains: return 12;    // ice_plains
        case snowy_mountains: return 12; // ice_mountains=13 is not used in BE. replacing to ice_plains=12
        case ice_spikes: return 140;     // ice_plains_spikes
        case windswept_hills:            // extreme_hills (listed as windswept_hills on wiki)
        case mountain_edge:
            return 3;                     // extreme_hills
        case windswept_forest: return 34; // extreme_hills_plus_trees
        case windswept_gravelly_hills:
        case modified_gravelly_mountains:
            return 131; // extreme_hills_mutated
        // case extreme_hills_plus_trees_mutated: return 162;
        // case extreme_hills_edge: return 20;
        case badlands: return 37;                         // mesa
        case badlands_plateau: return 37;                 // mesa_plateau=38 is not used in BE. replacing mesa=37
        case modified_badlands_plateau: return 37;        // mesa_plateau_mutated=166 is not used in BE. replacing mesa=37
        case wooded_badlands: return 38;                  // mesa_plateau_stone=38 (wiki says 39, actually 38)
        case modified_wooded_badlands_plateau: return 38; // mesa_plateau_stone_mutated=167 is not used in BE. replacing to mesa_plateau_stone=38
        case eroded_badlands: return 165;                 // mesa_bryce
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
        case deep_dark: return 190;
        case mangrove_swamp: return 191;
        case cherry_grove: return 192;
        default:
            return 0;
        }
    }

private:
    Biome() = delete;
};

} // namespace mcfile::be
