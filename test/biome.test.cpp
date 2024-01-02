#include <doctest/doctest.h>

#include <minecraft-file.hpp>

#include <iostream>

using namespace mcfile;
using namespace std;
using namespace mcfile::u8stream;

TEST_CASE("biome") {
    CHECK(biomes::Biome::Name(biomes::minecraft::mushroom_field_shore, mcfile::je::Chunk::kDataVersion) == u8"minecraft:mushroom_fields");
    CHECK(biomes::Biome::Name(biomes::minecraft::mushroom_field_shore, 2838 - 1) == u8"minecraft:mushroom_field_shore");
    CHECK(biomes::Biome::FromName(u8"minecraft:mushroom_field_shore") == biomes::minecraft::mushroom_field_shore);

    static auto const runTestCase = [](std::unordered_set<std::u8string> const &expected, int dataVersion) {
        std::unordered_set<std::u8string> actual;
        for (mcfile::biomes::BiomeId id = 1; id < biomes::minecraft::minecraft_max_biome_id; id++) {
            auto name = biomes::Biome::Name(id, dataVersion);
            if (!name) {
                continue;
            }
            CHECK(name->starts_with(u8"minecraft:"));
            actual.insert(name->substr(10));
        }
        if (actual.size() != expected.size()) {
            for (auto const &a : actual) {
                if (auto found = expected.find(a); found == expected.end()) {
                    std::cout << "excess biome: " << a << std::endl;
                }
            }
            for (auto const &e : expected) {
                if (auto found = actual.find(e); found == actual.end()) {
                    std::cout << "missing biome: " << e << std::endl;
                }
            }
        }
        CHECK(actual.size() == expected.size());
        for (auto const &e : expected) {
            CHECK(actual.find(e) != actual.end());
        }
    };

    SUBCASE("1.20") {
        // clang-format off
        std::unordered_set<std::u8string> const expected = {
            u8"badlands", u8"bamboo_jungle", u8"basalt_deltas", u8"beach", u8"birch_forest",
            u8"cherry_grove", u8"cold_ocean", u8"crimson_forest", u8"dark_forest", u8"deep_cold_ocean",
            u8"deep_dark", u8"deep_frozen_ocean", u8"deep_lukewarm_ocean", u8"deep_ocean", u8"desert",
            u8"dripstone_caves", u8"end_barrens", u8"end_highlands", u8"end_midlands", u8"eroded_badlands",
            u8"flower_forest", u8"forest", u8"frozen_ocean", u8"frozen_peaks", u8"frozen_river",
            u8"grove", u8"ice_spikes", u8"jagged_peaks", u8"jungle", u8"lukewarm_ocean",
            u8"lush_caves", u8"mangrove_swamp", u8"meadow", u8"mushroom_fields", u8"nether_wastes",
            u8"ocean", u8"old_growth_birch_forest", u8"old_growth_pine_taiga", u8"old_growth_spruce_taiga", u8"plains",
            u8"river", u8"savanna", u8"savanna_plateau", u8"small_end_islands", u8"snowy_beach",
            u8"snowy_plains", u8"snowy_slopes", u8"snowy_taiga", u8"soul_sand_valley", u8"sparse_jungle",
            u8"stony_peaks", u8"stony_shore", u8"sunflower_plains", u8"swamp", u8"taiga",
            u8"the_end", u8"the_void", u8"warm_ocean", u8"warped_forest", u8"windswept_forest",
            u8"windswept_gravelly_hills", u8"windswept_hills", u8"windswept_savanna", u8"wooded_badlands",
        };
        // clang-format on
        runTestCase(expected, 3463);
    }

    SUBCASE("1.19") {
        // clang-format off
        std::unordered_set<std::u8string> const expected = {
            u8"the_void", u8"plains", u8"sunflower_plains", u8"snowy_plains", u8"ice_spikes",
            u8"desert", u8"swamp", u8"mangrove_swamp", u8"forest", u8"flower_forest",
            u8"birch_forest", u8"dark_forest", u8"old_growth_birch_forest", u8"old_growth_pine_taiga", u8"old_growth_spruce_taiga",
            u8"taiga", u8"snowy_taiga", u8"savanna", u8"savanna_plateau", u8"windswept_hills",
            u8"windswept_gravelly_hills", u8"windswept_forest", u8"windswept_savanna", u8"jungle", u8"sparse_jungle",
            u8"bamboo_jungle", u8"badlands", u8"eroded_badlands", u8"wooded_badlands", u8"meadow",
            u8"grove", u8"snowy_slopes", u8"frozen_peaks", u8"jagged_peaks", u8"stony_peaks",
            u8"river", u8"frozen_river", u8"beach", u8"snowy_beach", u8"stony_shore",
            u8"warm_ocean", u8"lukewarm_ocean", u8"deep_lukewarm_ocean", u8"ocean", u8"deep_ocean",
            u8"cold_ocean", u8"deep_cold_ocean", u8"frozen_ocean", u8"deep_frozen_ocean", u8"mushroom_fields",
            u8"dripstone_caves", u8"lush_caves", u8"deep_dark", u8"nether_wastes", u8"warped_forest",
            u8"crimson_forest", u8"soul_sand_valley", u8"basalt_deltas", u8"the_end", u8"end_highlands",
            u8"end_midlands", u8"small_end_islands", u8"end_barrens",
        };
        // clang-format on
        runTestCase(expected, 3337);
    }

    SUBCASE("1.18") {
        // clang-format off
        std::unordered_set<std::u8string> const expected = {
            u8"the_void", u8"plains", u8"sunflower_plains", u8"snowy_plains", u8"ice_spikes",
            u8"desert", u8"swamp", u8"forest", u8"flower_forest", u8"birch_forest",
            u8"dark_forest", u8"old_growth_birch_forest", u8"old_growth_pine_taiga", u8"old_growth_spruce_taiga", u8"taiga",
            u8"snowy_taiga", u8"savanna", u8"savanna_plateau", u8"windswept_hills", u8"windswept_gravelly_hills",
            u8"windswept_forest", u8"windswept_savanna", u8"jungle", u8"sparse_jungle", u8"bamboo_jungle",
            u8"badlands", u8"eroded_badlands", u8"wooded_badlands", u8"meadow", u8"grove",
            u8"snowy_slopes", u8"frozen_peaks", u8"jagged_peaks", u8"stony_peaks", u8"river",
            u8"frozen_river", u8"beach", u8"snowy_beach", u8"stony_shore", u8"warm_ocean",
            u8"lukewarm_ocean", u8"deep_lukewarm_ocean", u8"ocean", u8"deep_ocean", u8"cold_ocean",
            u8"deep_cold_ocean", u8"frozen_ocean", u8"deep_frozen_ocean", u8"mushroom_fields", u8"dripstone_caves",
            u8"lush_caves", u8"nether_wastes", u8"warped_forest", u8"crimson_forest", u8"soul_sand_valley",
            u8"basalt_deltas", u8"the_end", u8"end_highlands", u8"end_midlands", u8"small_end_islands",
            u8"end_barrens",
        };
        // clang-format on
        runTestCase(expected, 2975);
    }

    SUBCASE("1.17") {
        // clang-format off
        std::unordered_set<std::u8string> const expected = {
            u8"ocean", u8"plains", u8"desert", u8"mountains", u8"forest",
            u8"taiga", u8"swamp", u8"river", u8"nether_wastes", u8"the_end",
            u8"frozen_ocean", u8"frozen_river", u8"snowy_tundra", u8"snowy_mountains", u8"mushroom_fields",
            u8"mushroom_field_shore", u8"beach", u8"desert_hills", u8"wooded_hills", u8"taiga_hills",
            u8"mountain_edge", u8"jungle", u8"jungle_hills", u8"jungle_edge", u8"deep_ocean",
            u8"stone_shore", u8"snowy_beach", u8"birch_forest", u8"birch_forest_hills", u8"dark_forest",
            u8"snowy_taiga", u8"snowy_taiga_hills", u8"giant_tree_taiga", u8"giant_tree_taiga_hills", u8"wooded_mountains",
            u8"savanna", u8"savanna_plateau", u8"badlands", u8"wooded_badlands_plateau", u8"badlands_plateau",
            u8"small_end_islands", u8"end_midlands", u8"end_highlands", u8"end_barrens", u8"warm_ocean",
            u8"lukewarm_ocean", u8"cold_ocean", u8"deep_warm_ocean", u8"deep_lukewarm_ocean", u8"deep_cold_ocean",
            u8"deep_frozen_ocean", u8"the_void", u8"sunflower_plains", u8"desert_lakes", u8"gravelly_mountains",
            u8"flower_forest", u8"taiga_mountains", u8"swamp_hills", u8"ice_spikes", u8"modified_jungle",
            u8"modified_jungle_edge", u8"tall_birch_forest", u8"tall_birch_hills", u8"dark_forest_hills", u8"snowy_taiga_mountains",
            u8"giant_spruce_taiga", u8"giant_spruce_taiga_hills", u8"modified_gravelly_mountains", u8"shattered_savanna", u8"shattered_savanna_plateau",
            u8"eroded_badlands", u8"modified_wooded_badlands_plateau", u8"modified_badlands_plateau", u8"bamboo_jungle", u8"bamboo_jungle_hills",
            u8"soul_sand_valley", u8"crimson_forest", u8"warped_forest", u8"basalt_deltas", u8"dripstone_caves",
            u8"lush_caves",
        };
        // clang-format on
        runTestCase(expected, 2730);
    }

    SUBCASE("1.16") {
        // clang-format off
        std::unordered_set<std::u8string> const expected = {
            u8"ocean", u8"plains", u8"desert", u8"mountains", u8"forest",
            u8"taiga", u8"swamp", u8"river", u8"nether_wastes", u8"the_end",
            u8"frozen_ocean", u8"frozen_river", u8"snowy_tundra", u8"snowy_mountains", u8"mushroom_fields",
            u8"mushroom_field_shore", u8"beach", u8"desert_hills", u8"wooded_hills", u8"taiga_hills",
            u8"mountain_edge", u8"jungle", u8"jungle_hills", u8"jungle_edge", u8"deep_ocean",
            u8"stone_shore", u8"snowy_beach", u8"birch_forest", u8"birch_forest_hills", u8"dark_forest",
            u8"snowy_taiga", u8"snowy_taiga_hills", u8"giant_tree_taiga", u8"giant_tree_taiga_hills", u8"wooded_mountains",
            u8"savanna", u8"savanna_plateau", u8"badlands", u8"wooded_badlands_plateau", u8"badlands_plateau",
            u8"small_end_islands", u8"end_midlands", u8"end_highlands", u8"end_barrens", u8"warm_ocean",
            u8"lukewarm_ocean", u8"cold_ocean", u8"deep_warm_ocean", u8"deep_lukewarm_ocean", u8"deep_cold_ocean",
            u8"deep_frozen_ocean", u8"the_void", u8"sunflower_plains", u8"desert_lakes", u8"gravelly_mountains",
            u8"flower_forest", u8"taiga_mountains", u8"swamp_hills", u8"ice_spikes", u8"modified_jungle",
            u8"modified_jungle_edge", u8"tall_birch_forest", u8"tall_birch_hills", u8"dark_forest_hills", u8"snowy_taiga_mountains",
            u8"giant_spruce_taiga", u8"giant_spruce_taiga_hills", u8"modified_gravelly_mountains", u8"shattered_savanna", u8"shattered_savanna_plateau",
            u8"eroded_badlands", u8"modified_wooded_badlands_plateau", u8"modified_badlands_plateau", u8"bamboo_jungle", u8"bamboo_jungle_hills",
            u8"soul_sand_valley", u8"crimson_forest", u8"warped_forest", u8"basalt_deltas",
        };
        // clang-format on
        runTestCase(expected, 2586);
    }
}
