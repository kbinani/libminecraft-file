#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "minecraft-file.hpp"

#include "BlockData.hpp"
#include "Point3D.hpp"

#include "string.test.hpp"

using namespace mcfile;
using namespace mcfile::u8stream;
namespace fs = std::filesystem;

TEST_CASE("1.13.2") {
    auto const file = fs::path(__FILE__);
    auto const dir = file.parent_path();
    std::map<Point3D, BlockData> expected;
    BlockData::ReadAll((dir / "data" / "1.13.2" / "mitomitoyapparikawaiiyo" / "region" / "r.0.0-c.0.0.csv").string(), expected);

    je::World world(dir / "data" / "1.13.2" / "mitomitoyapparikawaiiyo");

    SUBCASE("Chunk::{blockAt}") {
        auto region = world.region(0, 0);
        CHECK(region != nullptr);
        auto const &chunk = region->chunkAt(0, 0);
        CHECK(chunk != nullptr);
        CHECK(chunk->minBlockX() == 0);
        CHECK(chunk->maxBlockX() == 15);
        CHECK(chunk->minBlockY() == 0);
        CHECK(chunk->maxBlockY() == 255);
        CHECK(chunk->minBlockZ() == 0);
        CHECK(chunk->maxBlockZ() == 15);
        for (int y = 0; y < 256; y++) {
            for (int z = chunk->minBlockZ(); z <= chunk->maxBlockZ(); z++) {
                for (int x = chunk->minBlockX(); x <= chunk->maxBlockX(); x++) {
                    auto pos = MakePoint3D(x, y, z);
                    auto expectedIt = expected.find(pos);
                    assert(expectedIt != expected.end());
                    auto const &e = expectedIt->second;

                    auto block = chunk->blockAt(x, y, z);
                    if (block) {
                        CHECK(block->fName == e.fName);
                    } else {
                        CHECK(e.fName == u8"NULL");
                    }
                }
            }
        }
    }

    SUBCASE("Chunk::blockIdAt") {
        auto region = world.region(0, 0);
        CHECK(region != nullptr);
        auto const &chunk = region->chunkAt(0, 0);
        CHECK(chunk != nullptr);
        for (int y = 0; y < 256; y++) {
            for (int z = chunk->minBlockZ(); z <= chunk->maxBlockZ(); z++) {
                for (int x = chunk->minBlockX(); x <= chunk->maxBlockX(); x++) {
                    auto pos = MakePoint3D(x, y, z);
                    auto expectedIt = expected.find(pos);
                    assert(expectedIt != expected.end());
                    auto const &e = expectedIt->second;

                    auto blockId = chunk->blockIdAt(x, y, z);
                    if (e.fName == u8"NULL") {
                        CHECK(blockId == blocks::unknown);
                    } else {
                        auto ex = blocks::FromName(e.fName, chunk->getDataVersion());
                        CHECK(blockId == ex);
                    }
                }
            }
        }
    }

    SUBCASE("World::eachBlock") {
        auto const ret = world.eachBlock(0, 0, 15, 15, [&expected](int x, int y, int z, std::shared_ptr<je::Block const> const &block) {
            auto pos = MakePoint3D(x, y, z);
            auto expectedIt = expected.find(pos);
            assert(expectedIt != expected.end());
            auto const &e = expectedIt->second;
            if (block) {
                CHECK(block->fName == e.fName);
            } else {
                CHECK(e.fName == u8"NULL");
            }
            return true;
        });
        CHECK(ret);
    }
}

TEST_CASE("20w06a") {
    auto const file = fs::path(__FILE__);
    auto dir = file.parent_path();
    je::World world(dir / "data" / "20w06a" / "world" / "DIM-1");

    SUBCASE("biomeAt") {
        auto const &region = world.region(-1, -1);
        CHECK(region);
        auto const &chunk = region->chunkAt(-12, -5);
        CHECK(chunk);
        {
            auto const biome = chunk->biomeAt(-190, 54, -79);
            CHECK((int)biome == (int)biomes::minecraft::crimson_forest);
        }
        {
            auto const biome = chunk->biomeAt(-180, 75, -69);
            CHECK((int)biome == (int)biomes::minecraft::soul_sand_valley);
        }
    }
}

TEST_CASE("block-id") {
    int const dataVersion = mcfile::je::Chunk::kDataVersion;
    for (mcfile::blocks::BlockId id = 1; id < blocks::minecraft::minecraft_max_block_id; id++) {
        auto name = blocks::Name(id, dataVersion);
        CHECK(!name.empty());
        auto reverse = blocks::FromName(name, dataVersion);
        CHECK(reverse == id);
        if (reverse != id) {
            std::cout << name << std::endl;
        }
    }
}

TEST_CASE("biome_id") {
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

namespace {
std::shared_ptr<mcfile::nbt::CompoundTag> ReadCompoundFromFile(fs::path p) {
    auto s = std::make_shared<mcfile::stream::FileInputStream>(p);
    mcfile::stream::InputStreamReader reader(s, Endian::Big);
    return mcfile::nbt::CompoundTag::Read(reader);
}

std::shared_ptr<mcfile::nbt::CompoundTag> ReadCompoundFromCompressedFile(fs::path p) {
    return mcfile::nbt::CompoundTag::ReadCompressedFromFile(p, Endian::Big);
}

} // namespace

TEST_CASE("1.18") {
    auto const file = fs::path(__FILE__);
    auto dir = file.parent_path();
    je::World world(dir / "data" / "1.18rc3" / "12345678");
    SUBCASE("blockAt") {
        auto const &chunk = world.chunkAt(0, 0);
        CHECK(chunk);
        auto bricks = chunk->blockAt(14, -52, 9);
        CHECK(bricks);
        CHECK(bricks->fName == u8"minecraft:bricks");
        auto stoneBricks = chunk->blockAt(11, 316, 7);
        CHECK(stoneBricks);
        CHECK(stoneBricks->fName == u8"minecraft:stone_bricks");
    }
    SUBCASE("biomeAt") {
        {
            auto const &chunk = world.chunkAt(10, 17);
            CHECK(chunk->biomeAt(161, 2, 278) == biomes::minecraft::lush_caves);
            CHECK(chunk->biomeAt(161, 9, 278) == biomes::minecraft::snowy_beach);
        }
        {
            auto const &chunk = world.chunkAt(0, 12);
            CHECK(chunk->biomeAt(14, -52, 206) == biomes::minecraft::frozen_river);
            CHECK(chunk->biomeAt(15, -52, 202) == biomes::minecraft::snowy_taiga);
        }
    }
    SUBCASE("toCompoundTag") {
        auto tempDir = File::CreateTempDir(fs::temp_directory_path());
        auto region = world.region(0, 0);
        auto original = *tempDir / "original_c.0.0.nbt";
        CHECK(region->exportToNbt(0, 0, original));
        auto const &writableChunk = world.writableChunkAt(0, 0);
        auto written = *tempDir / "written_c.0.0.nbt";
        auto stream = std::make_shared<mcfile::stream::FileOutputStream>(written);
        CHECK(writableChunk->write(*stream, Dimension::Overworld));
        stream.reset();

        auto expectedNbt = ReadCompoundFromFile(original);
        CHECK(expectedNbt);
        auto expected = je::Chunk::MakeChunk(0, 0, expectedNbt);
        CHECK(expected);
        auto actualNbt = ReadCompoundFromCompressedFile(written);
        CHECK(actualNbt);
        auto actual = mcfile::je::Chunk::MakeChunk(0, 0, actualNbt);
        CHECK(actual);

        CHECK(expected->minBlockY() == actual->minBlockY());
        CHECK(expected->maxBlockY() == actual->maxBlockY());

        for (int y = expected->minBlockY(); y <= expected->maxBlockY(); y++) {
            for (int x = expected->minBlockX(); x <= expected->maxBlockX(); x++) {
                for (int z = expected->minBlockZ(); z <= expected->maxBlockZ(); z++) {
                    auto e = expected->blockAt(x, y, z);
                    auto a = actual->blockAt(x, y, z);
                    if (e) {
                        CHECK(e->toString() == a->toString());
                    } else {
                        CHECK(!a);
                    }

                    CHECK(expected->biomeAt(x, y, z) == actual->biomeAt(x, y, z));
                }
            }
        }

        std::error_code ec;
        fs::remove_all(*tempDir, ec);
    }

    SUBCASE("5biomes") {
        using namespace std;
        using namespace mcfile;
        using namespace mcfile::je;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        fs::path in = dir / "data" / "5biomes" / "c.-1.-54.nbt";
        shared_ptr<WritableChunk> expected;
        {
            auto root = CompoundTag::ReadFromFile(in, Endian::Big);
            expected = WritableChunk::MakeChunk(-1, -54, root);
        }

        auto tmp = File::CreateTempDir(fs::temp_directory_path());
        auto out = *tmp / "out.nbt";
        {
            auto s = std::make_shared<FileOutputStream>(out);
            expected->write(*s, Dimension::Overworld);
        }

        shared_ptr<Chunk> actual;
        {
            auto root = CompoundTag::ReadCompressedFromFile(out, Endian::Big);
            actual = Chunk::MakeChunk(-1, -54, root);
        }

        for (int y = expected->minBlockY(); y <= expected->maxBlockY(); y++) {
            for (int z = expected->minBlockZ(); z <= expected->maxBlockZ(); z++) {
                for (int x = expected->minBlockX(); x <= expected->maxBlockX(); x++) {
                    CHECK(expected->biomeAt(x, y, z) == actual->biomeAt(x, y, z));
                }
            }
        }

        error_code ec;
        fs::remove_all(*tmp, ec);
    }
}
