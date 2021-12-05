#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;
using namespace mcfile::be;

TEST_CASE("be/biome-section") {
    SUBCASE("encode") {
        auto section = make_shared<BiomeSection>();
        section->fill(biomes::minecraft::taiga);
        CHECK(section->encode(nullptr) == false);
    }
    SUBCASE("format0") {
        auto section = make_shared<BiomeSection>();
        section->fill(biomes::minecraft::taiga);
        string encoded;
        CHECK(section->encode(&encoded));
        string expected;
        expected.assign("\01\05\00\00\00", 5);
        CHECK(encoded == expected);

        string data = "abc" + encoded;
        size_t offset = 3;
        auto decoded = BiomeSection::Decode(data, &offset);
        CHECK(decoded);
        CHECK(offset == 8);
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
    SUBCASE("format3") {
        auto section = make_shared<BiomeSection>();
        section->fill(biomes::minecraft::taiga);
        section->set(0, 0, 0, biomes::minecraft::desert);
        section->set(1, 3, 5, biomes::minecraft::basalt_deltas);
        section->set(15, 15, 15, biomes::minecraft::badlands);
        CHECK(biomes::minecraft::desert == *section->get(0, 0, 0));
        CHECK(biomes::minecraft::basalt_deltas == *section->get(1, 3, 5));
        CHECK(biomes::minecraft::badlands == *section->get(15, 15, 15));
        string encoded;
        CHECK(section->encode(&encoded));

        string data = "abcd" + encoded;
        size_t offset = 4;
        auto decoded = BiomeSection::Decode(data, &offset);
        CHECK(decoded);
        CHECK(offset == 4 + encoded.size());
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    auto biome = decoded->get(x, y, z);
                    CHECK(biome);
                    if (x == 0 && y == 0 && z == 0) {
                        CHECK(*biome == biomes::minecraft::desert);
                    } else if (x == 1 && y == 3 && z == 5) {
                        CHECK(*biome == biomes::minecraft::basalt_deltas);
                    } else if (x == 15 && y == 15 && z == 15) {
                        CHECK(*biome == biomes::minecraft::badlands);
                    } else {
                        CHECK(*biome == biomes::minecraft::taiga);
                    }
                }
            }
        }
    }
}
