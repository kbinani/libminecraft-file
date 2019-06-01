#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "minecraft-file.hpp"
#include "Path.hpp"
#include "Point3D.hpp"
#include "BlockData.hpp"

using namespace mcfile;

TEST_CASE("Chunk") {
    const auto dir = Path::Dirname(__FILE__);
    std::map<Point3D, BlockData> expected;
    BlockData::ReadAll(dir + std::string("/data/1.13.2/mitomitoyapparikawaiiyo/region/r.0.0-c.0.0.csv"), expected);

    World world(dir + std::string("/data/1.13.2/mitomitoyapparikawaiiyo"));

    SUBCASE("{blockAt,blockLightAt,skyLightAt}") {
        auto region = world.region(0, 0);
        CHECK(region != nullptr);
        const bool ret = region->loadChunk(0, 0, [&expected](Chunk const& chunk) {
            CHECK(chunk.minBlockX() == 0);
            CHECK(chunk.maxBlockX() == 15);
            CHECK(chunk.minBlockZ() == 0);
            CHECK(chunk.maxBlockZ() == 15);
            for (int y = 0; y < 256; y++) {
                for (int z = chunk.minBlockZ(); z <= chunk.maxBlockZ(); z++) {
                    for (int x = chunk.minBlockX(); x <= chunk.maxBlockX(); x++) {
                        auto pos = MakePoint3D(x, y, z);
                        auto expectedIt = expected.find(pos);
                        assert(expectedIt != expected.end());
                        auto const& e = expectedIt->second;

                        auto block = chunk.blockAt(x, y, z);
                        if (block) {
                            CHECK(block->fName == e.fName);
                        } else {
                            CHECK(e.fName == "NULL");
                        }

                        auto blockLight = chunk.blockLightAt(x, y, z);
                        CHECK(blockLight == e.fBlockLight);

                        auto skyLight = chunk.skyLightAt(x, y, z);
                        CHECK(skyLight == e.fSkyLight);
                    }
                }
            }
            return true;
        });
        CHECK(ret);
    }
    
    SUBCASE("blockIdAt") {
        auto region = world.region(0, 0);
        CHECK(region != nullptr);
        auto const ret = region->loadChunk(0, 0, [&expected](Chunk const& chunk) {
            for (int y = 0; y < 256; y++) {
                for (int z = chunk.minBlockZ(); z <= chunk.maxBlockZ(); z++) {
                    for (int x = chunk.minBlockX(); x <= chunk.maxBlockX(); x++) {
                        auto pos = MakePoint3D(x, y, z);
                        auto expectedIt = expected.find(pos);
                        assert(expectedIt != expected.end());
                        auto const& e = expectedIt->second;
                        
                        auto blockId = chunk.blockIdAt(x, y, z);
                        if (e.fName == "NULL") {
                            CHECK(blockId == blocks::unknown);
                        } else {
                            auto ex = blocks::FromName(e.fName);
                            CHECK(blockId == ex);
                        }
                    }
                }
            }
            return true;
        });
        CHECK(ret);
    }
}
