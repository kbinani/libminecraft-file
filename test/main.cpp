#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "minecraft-file.hpp"

#include "BlockData.hpp"
#include "Path.hpp"
#include "Point3D.hpp"

using namespace mcfile;

TEST_CASE("1.13.2") {
    const auto dir = Path::Dirname(__FILE__);
    std::map<Point3D, BlockData> expected;
    BlockData::ReadAll(dir + std::string("/data/1.13.2/mitomitoyapparikawaiiyo/region/r.0.0-c.0.0.csv"), expected);

    je::World world(std::filesystem::path(dir) / "data" / "1.13.2" / "mitomitoyapparikawaiiyo");

    SUBCASE("Chunk::{blockAt,blockLightAt,skyLightAt}") {
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
                        CHECK(e.fName == "NULL");
                    }

                    auto blockLight = chunk->blockLightAt(x, y, z);
                    CHECK(blockLight == e.fBlockLight);

                    auto skyLight = chunk->skyLightAt(x, y, z);
                    CHECK(skyLight == e.fSkyLight);
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
                    if (e.fName == "NULL") {
                        CHECK(blockId == blocks::unknown);
                    } else {
                        auto ex = blocks::FromName(e.fName);
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
                CHECK(e.fName == "NULL");
            }
            return true;
        });
        CHECK(ret);
    }
}

TEST_CASE("20w06a") {
    std::filesystem::path dir(Path::Dirname(__FILE__));
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
    for (mcfile::blocks::BlockId id = 1; id < blocks::minecraft::minecraft_max_block_id; id++) {
        auto name = blocks::Name(id);
        CHECK(!name.empty());
        auto reverse = blocks::FromName(name);
        CHECK(reverse == id);
        if (reverse != id) {
            std::cout << name << std::endl;
        }
    }
}
