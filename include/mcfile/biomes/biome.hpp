#pragma once

#include <iostream>

namespace mcfile::biomes {

class Biome {
    Biome() = delete;

private:
    static std::unordered_map<int, BiomeId> *CreateTable() {
        using namespace biomes::minecraft;
        return new std::unordered_map<int, BiomeId>({
            {0, ocean},
            {1, plains},
            {2, desert},
            {3, windswept_hills},
            {4, forest},
            {5, taiga},
            {6, swamp},
            {7, river},
            {8, nether_wastes},
            {9, the_end},
            {10, frozen_ocean},
            {11, frozen_river},
            {12, snowy_plains},
            {13, snowy_mountains},
            {14, mushroom_fields},
            {15, mushroom_field_shore},
            {16, beach},
            {17, desert_hills},
            {18, wooded_hills},
            {19, taiga_hills},
            {20, mountain_edge},
            {21, jungle},
            {22, jungle_hills},
            {23, sparse_jungle},
            {24, deep_ocean},
            {25, stony_shore},
            {26, snowy_beach},
            {27, birch_forest},
            {28, birch_forest_hills},
            {29, dark_forest},
            {30, snowy_taiga},
            {31, snowy_taiga_hills},
            {32, old_growth_pine_taiga},
            {33, giant_tree_taiga_hills},
            {34, windswept_forest},
            {35, savanna},
            {36, savanna_plateau},
            {37, badlands},
            {38, wooded_badlands},
            {39, badlands_plateau},
            {40, small_end_islands},
            {41, end_midlands},
            {42, end_highlands},
            {43, end_barrens},
            {44, warm_ocean},
            {45, lukewarm_ocean},
            {46, cold_ocean},
            {47, deep_warm_ocean},
            {48, deep_lukewarm_ocean},
            {49, deep_cold_ocean},
            {50, deep_frozen_ocean},
            {127, the_void},
            {129, sunflower_plains},
            {130, desert_lakes},
            {131, windswept_gravelly_hills},
            {132, flower_forest},
            {133, taiga_mountains},
            {134, swamp_hills},
            {140, ice_spikes},
            {149, modified_jungle},
            {151, modified_jungle_edge},
            {155, old_growth_birch_forest},
            {156, tall_birch_hills},
            {157, dark_forest_hills},
            {158, snowy_taiga_mountains},
            {160, old_growth_spruce_taiga},
            {161, giant_spruce_taiga_hills},
            {162, modified_gravelly_mountains},
            {163, windswept_savanna},
            {164, shattered_savanna_plateau},
            {165, eroded_badlands},
            {166, modified_wooded_badlands_plateau},
            {167, modified_badlands_plateau},
            {168, bamboo_jungle},
            {169, bamboo_jungle_hills},

            // 20w06a
            {170, soul_sand_valley},
            {171, crimson_forest},
            {172, warped_forest},

            // 20w15a
            {173, basalt_deltas},

            // 1.18
            {174, dripstone_caves},
            {175, lush_caves},

            // Need not to update here for 1.18 or later releases, as FromInt is used for numeric biome ids used in 1.17 or older.
        });
    }

    static std::unordered_map<int, BiomeId> const &Table() {
        static std::unique_ptr<std::unordered_map<int, BiomeId> const> const sTable(CreateTable());
        return *sTable;
    }

    static std::unordered_map<BiomeId, int> *CreateRTable() {
        auto ret = new std::unordered_map<BiomeId, int>();
        for (auto const &it : Table()) {
            (*ret)[it.second] = it.first;
        }
        return ret;
    }

    static std::unordered_map<BiomeId, int> const &RTable() {
        static std::unique_ptr<std::unordered_map<BiomeId, int> const> const sTable(CreateRTable());
        return *sTable;
    }

public:
    static std::optional<BiomeId> FromInt(int v) {
        auto const &table = Table();
        if (auto found = table.find(v); found != table.end()) {
            return found->second;
        } else {
            return std::nullopt;
        }
    }

    static std::optional<int> ToInt(BiomeId id) {
        auto const &table = RTable();
        if (auto found = table.find(id); found != table.end()) {
            return found->second;
        } else {
            return std::nullopt;
        }
    }

    static std::u8string Name(BiomeId id, int chunkDataVersion, std::u8string const &fallback) {
        if (auto name = Name(id, chunkDataVersion); name) {
            return *name;
        } else {
            return fallback;
        }
    }

    static std::optional<std::u8string> Name(BiomeId id, int dataVersion) {
        using namespace mcfile::biomes::minecraft;

        static std::unordered_map<BiomeId, std::u8string> const mapping = {
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
            {minecraft::mushroom_fields, u8"minecraft:mushroom_fields"},
            {minecraft::beach, u8"minecraft:beach"},
            {minecraft::jungle, u8"minecraft:jungle"},
            {minecraft::deep_ocean, u8"minecraft:deep_ocean"},
            {minecraft::snowy_beach, u8"minecraft:snowy_beach"},
            {minecraft::birch_forest, u8"minecraft:birch_forest"},
            {minecraft::dark_forest, u8"minecraft:dark_forest"},
            {minecraft::snowy_taiga, u8"minecraft:snowy_taiga"},
            {minecraft::savanna, u8"minecraft:savanna"},
            {minecraft::savanna_plateau, u8"minecraft:savanna_plateau"},
            {minecraft::badlands, u8"minecraft:badlands"},
            {minecraft::small_end_islands, u8"minecraft:small_end_islands"},
            {minecraft::end_midlands, u8"minecraft:end_midlands"},
            {minecraft::end_highlands, u8"minecraft:end_highlands"},
            {minecraft::end_barrens, u8"minecraft:end_barrens"},
            {minecraft::warm_ocean, u8"minecraft:warm_ocean"},
            {minecraft::lukewarm_ocean, u8"minecraft:lukewarm_ocean"},
            {minecraft::cold_ocean, u8"minecraft:cold_ocean"},
            {minecraft::deep_lukewarm_ocean, u8"minecraft:deep_lukewarm_ocean"},
            {minecraft::deep_cold_ocean, u8"minecraft:deep_cold_ocean"},
            {minecraft::deep_frozen_ocean, u8"minecraft:deep_frozen_ocean"},
            {minecraft::the_void, u8"minecraft:the_void"},
            {minecraft::sunflower_plains, u8"minecraft:sunflower_plains"},
            {minecraft::flower_forest, u8"minecraft:flower_forest"},
            {minecraft::ice_spikes, u8"minecraft:ice_spikes"},
            {minecraft::eroded_badlands, u8"minecraft:eroded_badlands"},
        };

        static auto const ns = [](std::u8string const &v) -> std::u8string { return u8"minecraft:" + v; };

        switch (id) {
        case bamboo_jungle:
            if (dataVersion < 1901) {
                return ns(u8"jungle");
            } else {
                return ns(u8"bamboo_jungle");
            }
        case nether_wastes:
            if (dataVersion < 2504) {
                return ns(u8"nether");
            } else {
                return ns(u8"nether_wastes");
            }
        case soul_sand_valley:
            if (dataVersion < 2504) {
                return ns(u8"nether");
            } else {
                return ns(u8"soul_sand_valley");
            }
        case crimson_forest:
            if (dataVersion < 2504) {
                return ns(u8"nether");
            } else {
                return ns(u8"crimson_forest");
            }
        case warped_forest:
            if (dataVersion < 2504) {
                return ns(u8"nether");
            } else {
                return ns(u8"warped_forest");
            }
        case basalt_deltas:
            if (dataVersion < 2525) {
                if (dataVersion < 2504) {
                    return ns(u8"nether");
                } else {
                    return ns(u8"nether_wastes");
                }
            } else {
                return ns(u8"basalt_deltas");
            }
        case snowy_plains:
            if (dataVersion < 2838) {
                return ns(u8"snowy_tundra");
            } else {
                return ns(u8"snowy_plains");
            }
        case windswept_hills:
            if (dataVersion < 2838) {
                return ns(u8"mountains");
            } else {
                return ns(u8"windswept_hills");
            }
        case windswept_gravelly_hills:
            if (dataVersion < 2838) {
                return ns(u8"gravelly_mountains");
            } else {
                return ns(u8"windswept_gravelly_hills");
            }
        case windswept_forest:
            if (dataVersion < 2838) {
                return ns(u8"wooded_mountains");
            } else {
                return ns(u8"windswept_forest");
            }
        case old_growth_pine_taiga:
            if (dataVersion < 2838) {
                return ns(u8"giant_tree_taiga");
            } else {
                return ns(u8"old_growth_pine_taiga");
            }
        case old_growth_spruce_taiga:
            if (dataVersion < 2838) {
                return ns(u8"giant_spruce_taiga");
            } else {
                return ns(u8"old_growth_spruce_taiga");
            }
        case stony_shore:
            if (dataVersion < 2838) {
                return ns(u8"stone_shore");
            } else {
                return ns(u8"stony_shore");
            }
        case windswept_savanna:
            if (dataVersion < 2838) {
                return ns(u8"shattered_savanna");
            } else {
                return ns(u8"windswept_savanna");
            }
        case sparse_jungle:
            if (dataVersion < 2838) {
                return ns(u8"jungle_edge");
            } else {
                return ns(u8"sparse_jungle");
            }
        case wooded_badlands:
            if (dataVersion < 2838) {
                return ns(u8"wooded_badlands_plateau");
            } else {
                return ns(u8"wooded_badlands");
            }
        case old_growth_birch_forest:
            if (dataVersion < 2838) {
                return ns(u8"tall_birch_forest");
            } else {
                return ns(u8"old_growth_birch_forest");
            }
        case mushroom_field_shore:
            if (dataVersion < 2838) {
                return ns(u8"mushroom_field_shore");
            } else {
                return ns(u8"mushroom_fields");
            }
        case snowy_mountains:
            if (dataVersion < 2838) {
                return ns(u8"snowy_mountains");
            } else {
                return ns(u8"snowy_plains");
            }
        case desert_hills:
            if (dataVersion < 2838) {
                return ns(u8"desert_hills");
            } else {
                return ns(u8"desert");
            }
        case wooded_hills:
            if (dataVersion < 2838) {
                return ns(u8"wooded_hills");
            } else {
                return ns(u8"forest");
            }
        case taiga_hills:
            if (dataVersion < 2838) {
                return ns(u8"taiga_hills");
            } else {
                return ns(u8"taiga");
            }
        case jungle_hills:
            if (dataVersion < 2838) {
                return ns(u8"jungle_hills");
            } else {
                return ns(u8"jungle");
            }
        case birch_forest_hills:
            if (dataVersion < 2838) {
                return ns(u8"birch_forest_hills");
            } else {
                return ns(u8"birch_forest");
            }
        case snowy_taiga_hills:
            if (dataVersion < 2838) {
                return ns(u8"snowy_taiga_hills");
            } else {
                return ns(u8"snowy_taiga");
            }
        case giant_tree_taiga_hills:
            if (dataVersion < 2838) {
                return ns(u8"giant_tree_taiga_hills");
            } else {
                return ns(u8"old_growth_pine_taiga");
            }
        case badlands_plateau:
            if (dataVersion < 2838) {
                return ns(u8"badlands_plateau");
            } else {
                return ns(u8"badlands");
            }
        case desert_lakes:
            if (dataVersion < 2838) {
                return ns(u8"desert_lakes");
            } else {
                return ns(u8"desert");
            }
        case taiga_mountains:
            if (dataVersion < 2838) {
                return ns(u8"taiga_mountains");
            } else {
                return ns(u8"taiga");
            }
        case swamp_hills:
            if (dataVersion < 2838) {
                return ns(u8"swamp_hills");
            } else {
                return ns(u8"swamp");
            }
        case modified_jungle:
            if (dataVersion < 2838) {
                return ns(u8"modified_jungle");
            } else {
                return ns(u8"jungle");
            }
        case modified_jungle_edge:
            if (dataVersion < 2838) {
                return ns(u8"modified_jungle_edge");
            } else {
                return ns(u8"sparse_jungle");
            }
        case tall_birch_hills:
            if (dataVersion < 2838) {
                return ns(u8"tall_birch_hills");
            } else {
                return ns(u8"old_growth_birch_forest");
            }
        case dark_forest_hills:
            if (dataVersion < 2838) {
                return ns(u8"dark_forest_hills");
            } else {
                return ns(u8"dark_forest");
            }
        case snowy_taiga_mountains:
            if (dataVersion < 2838) {
                return ns(u8"snowy_taiga_mountains");
            } else {
                return ns(u8"snowy_taiga");
            }
        case giant_spruce_taiga_hills:
            if (dataVersion < 2838) {
                return ns(u8"giant_spruce_taiga_hills");
            } else {
                return ns(u8"old_growth_spruce_taiga");
            }
        case modified_gravelly_mountains:
            if (dataVersion < 2838) {
                return ns(u8"modified_gravelly_mountains");
            } else {
                return ns(u8"windswept_gravelly_hills");
            }
        case shattered_savanna_plateau:
            if (dataVersion < 2838) {
                return ns(u8"shattered_savanna_plateau");
            } else {
                return ns(u8"windswept_savanna");
            }
        case modified_badlands_plateau:
            if (dataVersion < 2838) {
                return ns(u8"modified_badlands_plateau");
            } else {
                return ns(u8"badlands");
            }
        case bamboo_jungle_hills:
            if (dataVersion < 2838) {
                if (dataVersion < 1901) {
                    return ns(u8"jungle_hills");
                } else {
                    return ns(u8"bamboo_jungle_hills");
                }
            } else {
                return ns(u8"bamboo_jungle");
            }
        case modified_wooded_badlands_plateau:
            if (dataVersion < 2838) {
                return ns(u8"modified_wooded_badlands_plateau");
            } else {
                return ns(u8"wooded_badlands");
            }
        case frozen_peaks:
            if (dataVersion < 2838) {
                return ns(u8"snowy_mountains");
            } else {
                return ns(u8"frozen_peaks");
            }
        case jagged_peaks:
            if (dataVersion < 2838) {
                return ns(u8"mountains");
            } else {
                return ns(u8"jagged_peaks");
            }
        case deep_warm_ocean:
            if (dataVersion < 2844) {
                return ns(u8"deep_warm_ocean");
            } else {
                return ns(u8"deep_ocean");
            }
        case meadow:
            if (dataVersion > 2730) {
                return ns(u8"meadow");
            } else {
                return ns(u8"plains");
            }
        case grove:
            if (dataVersion > 2730) {
                return ns(u8"grove");
            } else {
                return ns(u8"snowy_mountains");
            }
        case snowy_slopes:
            if (dataVersion > 2730) {
                return ns(u8"snowy_slopes");
            } else {
                return ns(u8"snowy_mountains");
            }
        case stony_peaks:
            if (dataVersion > 2730) {
                return ns(u8"stony_peaks");
            } else {
                return ns(u8"mountains");
            }
        case mountain_edge:
            if (dataVersion < 2838) {
                return ns(u8"mountain_edge");
            } else {
                return ns(u8"windswept_hills");
            }
        case dripstone_caves:
            if (dataVersion < 2685) {
                return ns(u8"mountains");
            } else {
                return ns(u8"dripstone_caves");
            }
        case lush_caves:
            if (dataVersion < 2699) {
                return ns(u8"mountains");
            } else {
                return ns(u8"lush_caves");
            }
        case mangrove_swamp:
            if (dataVersion < 3088) {
                return ns(u8"swamp");
            } else {
                return ns(u8"mangrove_swamp");
            }
        case deep_dark:
            if (dataVersion < 3066) {
                if (dataVersion < 2838) {
                    return ns(u8"mountains");
                } else {
                    return ns(u8"windswept_hills");
                }
            } else {
                return ns(u8"deep_dark");
            }
        case cherry_grove:
            if (dataVersion < 3442) {
                return ns(u8"flower_forest");
            } else {
                return ns(u8"cherry_grove");
            }
        }

        // For newly added biomes, see
        // https://github.com/ViaVersion/ViaBackwards/blob/master/common/src/main/resources/assets/viabackwards/data/biome-mappings.json
        //
        // For removed biomes, create a world to visit them with an old version, then open it with a new version to see how they are migrated.

        auto mappingIt = mapping.find(id);
        if (mappingIt == mapping.end()) {
            assert(false);
            return std::nullopt;
        }
        return mappingIt->second;
    }

    static std::optional<mcfile::biomes::BiomeId> FromName(std::u8string const &name) {
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

    static std::u8string Default(Dimension d) {
        switch (d) {
        case Dimension::End:
            return u8"minecraft:the_end";
        case Dimension::Nether:
            return u8"minecraft:nether_wastes";
        default:
            return u8"minecraft:ocean";
        }
    }
};

} // namespace mcfile::biomes
