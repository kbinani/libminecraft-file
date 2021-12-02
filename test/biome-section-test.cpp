#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;
using namespace mcfile::be;

TEST_CASE("BiomeSection") {
    SUBCASE("format0") {
        BiomeSection section;
        section.fill(biomes::minecraft::taiga);
        string encoded;
        CHECK(section.encode(encoded));
        string expected;
        expected.assign("\01\05\00\00\00", 5);
        CHECK(encoded == expected);

        string data = "abc" + encoded;
        auto decoded = BiomeSection::Decode(data, 3);
        CHECK(decoded);
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    auto biome = decoded->get(x, y, z);
                    CHECK(biome);
                    CHECK(*biome == biomes::minecraft::taiga);
                }
            }
        }
    }
}
