#pragma once

namespace mcfile::be {

class Biome {
public:
    static biomes::BiomeId FromUint8(uint8_t raw) {
        switch (raw) {
        case 0: return biomes::minecraft::ocean;
        case 47: return biomes::minecraft::frozen_ocean; // legacy frozen ocean
        case 24: return biomes::minecraft::deep_ocean;
        case 10: return biomes::minecraft::frozen_ocean;
        case 46: return biomes::minecraft::deep_frozen_ocean;
        case 42: return biomes::minecraft::cold_ocean;
        case 45: return biomes::minecraft::deep_cold_ocean;
        case 41: return biomes::minecraft::lukewarm_ocean;
        case 44: return biomes::minecraft::deep_lukewarm_ocean;
        case 40: return biomes::minecraft::warm_ocean;
        case 43: return biomes::minecraft::deep_warm_ocean;
        case 7: return biomes::minecraft::river;
        case 11: return biomes::minecraft::frozen_river;
        case 16: return biomes::minecraft::beach;
        case 25: return biomes::minecraft::stone_shore; // stone_beach
        case 26: return biomes::minecraft::snowy_beach; // cold_beach
        case 4: return biomes::minecraft::forest;
        case 18: return biomes::minecraft::wooded_hills; // forest_hills
        case 132: return biomes::minecraft::flower_forest;
        case 27: return biomes::minecraft::birch_forest;
        case 28: return biomes::minecraft::birch_forest_hills;
        case 155: return biomes::minecraft::tall_birch_forest; // birch_forest_mutated
        case 156: return biomes::minecraft::tall_birch_hills; // birch_forest_hills_mutated
        case 29: return biomes::minecraft::dark_forest; // roofed_forest
        case 157: return biomes::minecraft::dark_forest_hills; // roofed_forest_mutated
        case 21: return biomes::minecraft::jungle;
        case 22: return biomes::minecraft::jungle_hills;
        case 149: return biomes::minecraft::modified_jungle; // jungle_mutated
        case 23: return biomes::minecraft::jungle_edge;
        case 151: return biomes::minecraft::modified_jungle_edge; // jungle_edge_mutated
        case 168: return biomes::minecraft::bamboo_jungle;
        case 169: return biomes::minecraft::bamboo_jungle_hills;
        case 5: return biomes::minecraft::taiga;
        case 19: return biomes::minecraft::taiga_hills;
        case 133: return biomes::minecraft::taiga_mountains; // taiga_mutated
        case 30: return biomes::minecraft::snowy_taiga; // cold_taiga
        case 31: return biomes::minecraft::snowy_taiga_hills; // cold_taiga_hills
        case 158: return biomes::minecraft::snowy_taiga_mountains; // cold_taiga_mutated
        case 32: return biomes::minecraft::giant_tree_taiga; // mega_taiga
        case 33: return biomes::minecraft::giant_tree_taiga_hills; // mega_taiga_hills
        case 160: return biomes::minecraft::giant_spruce_taiga; // redwood_taiga_mutated
        case 161: return biomes::minecraft::giant_spruce_taiga_hills; // redwood_taiga_hills_mutated
        case 14: return biomes::minecraft::mushroom_fields; // mushroom_island
        case 15: return biomes::minecraft::mushroom_field_shore; // mushroom_island_shore
        case 6: return biomes::minecraft::swamp; // swampland
        case 134: return biomes::minecraft::swamp_hills; // swampland_mutated
        case 35: return biomes::minecraft::savanna;
        case 36: return biomes::minecraft::savanna_plateau;
        case 163: return biomes::minecraft::shattered_savanna; // savanna_mutated
        case 164: return biomes::minecraft::shattered_savanna_plateau; // savanna_plateau_mutated
        case 1: return biomes::minecraft::plains;
        case 129: return biomes::minecraft::sunflower_plains;
        case 2: return biomes::minecraft::desert;
        case 17: return biomes::minecraft::desert_hills;
        case 130: return biomes::minecraft::desert_lakes; // desert_mutated
        case 12: return biomes::minecraft::snowy_tundra; // ice_plains
        case 13: return biomes::minecraft::snowy_mountains; // ice_mountains
        case 140: return biomes::minecraft::ice_spikes; // ice_plains_spikes
        case 3: return biomes::minecraft::mountains; // extreme_hills
        case 34: return biomes::minecraft::wooded_mountains; // extreme_hills_plus_trees
        case 131: return biomes::minecraft::gravelly_mountains; // extreme_hills_mutated
        case 162: return biomes::minecraft::modified_gravelly_mountains; // extreme_hills_plus_trees_mutated
        case 20: return biomes::minecraft::mountain_edge; // extreme_hills_edge
        case 37: return biomes::minecraft::badlands; // mesa;
        case 38: return biomes::minecraft::badlands_plateau; // mesa_plateau;
        case 166: return biomes::minecraft::modified_badlands_plateau; // mesa_plateau_mutated
        case 39: return biomes::minecraft::wooded_badlands_plateau; // mesa_plateau_stone
        case 167: return biomes::minecraft::modified_wooded_badlands_plateau; // mesa_plateau_stone_mutated
        case 165: return biomes::minecraft::eroded_badlands; // mesa_bryce
        case 8: return biomes::minecraft::nether_wastes; // hell
        case 179: return biomes::minecraft::crimson_forest;
        case 180: return biomes::minecraft::warped_forest;
        case 178: return biomes::minecraft::soul_sand_valley; // soulsand_valley
        case 181: return biomes::minecraft::basalt_deltas;
        case 9: return biomes::minecraft::the_end;
        default:
            return biomes::minecraft::ocean;
        }
    }

    static uint8_t ToUint8(mcfile::biomes::BiomeId biome) {
        using namespace mcfile::biomes::minecraft;
        switch (biome) {
        case ocean:
            return 42;
        case deep_ocean:
            return 24;
        case frozen_ocean:
            return 10;
        case deep_frozen_ocean:
            return 50;
        case cold_ocean:
            return 46;
        case deep_cold_ocean:
            return 49;
        case lukewarm_ocean:
            return 45;
        case deep_lukewarm_ocean:
            return 48;
        case warm_ocean:
            return 44;
        case deep_warm_ocean:
            return 47;
        case river:
            return 7;
        case frozen_river:
            return 11;
        case beach:
            return 16;
        case stone_shore:
            return 25;
        case snowy_beach:
            return 26;
        case forest:
            return 4;
        case wooded_hills:
            return 18;
        case flower_forest:
            return 132;
        case birch_forest:
            return 27;
        case birch_forest_hills:
            return 28;
        case tall_birch_forest:
            return 155;
        case tall_birch_hills:
            return 156;
        case dark_forest:
            return 29;
        case dark_forest_hills:
            return 157;
        case jungle:
            return 21;
        case jungle_hills:
            return 22;
        case modified_jungle:
            return 149;
        case jungle_edge:
            return 23;
        case modified_jungle_edge:
            return 151;
        case bamboo_jungle:
            return 168;
        case bamboo_jungle_hills:
            return 169;
        case taiga:
            return 5;
        case taiga_hills:
            return 19;
        case taiga_mountains:
            return 133;
        case snowy_taiga:
            return 30;
        case snowy_taiga_hills:
            return 31;
        case snowy_taiga_mountains:
            return 158;
        case giant_tree_taiga:
            return 32;
        case giant_tree_taiga_hills:
            return 33;
        case giant_spruce_taiga:
            return 160;
        case giant_spruce_taiga_hills:
            return 161;
        case mushroom_fields:
            return 14;
        case mushroom_field_shore:
            return 15;
        case swamp:
            return 6;
        case swamp_hills:
            return 134;
        case savanna:
            return 35;
        case savanna_plateau:
            return 36;
        case shattered_savanna:
            return 163;
        case shattered_savanna_plateau:
            return 164;
        case plains:
            return 1;
        case sunflower_plains:
            return 129;
        case desert:
            return 2;
        case desert_hills:
            return 17;
        case desert_lakes:
            return 130;
        case snowy_tundra:
            return 12;
        case snowy_mountains:
            return 13;
        case ice_spikes:
            return 140;
        case mountains:
            return 3;
        case wooded_mountains:
            return 34;
        case gravelly_mountains:
            return 131;
        case modified_gravelly_mountains:
            return 162;
        case mountain_edge:
            return 20;
        case badlands:
            return 37;
        case badlands_plateau:
            return 39;
        case modified_badlands_plateau:
            return 167;
        case wooded_badlands_plateau:
            return 38;
        case modified_wooded_badlands_plateau:
            return 166;
        case eroded_badlands:
            return 165;
        case nether_wastes:
            return 8;
        case crimson_forest:
            return 179;
        case warped_forest:
            return 180;
        case soul_sand_valley:
            return 178;
        case basalt_deltas:
            return 181;
        case the_end:
        case small_end_islands:
        case end_midlands:
        case end_highlands:
        case end_barrens:
            return 9;
        case the_void:
        default:
            assert(false);
            return 42;
        }
    }

private:
    Biome() = delete;
};

}
