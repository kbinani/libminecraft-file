#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace mcfile;
using namespace std;

TEST_CASE("heightmap") {
    SUBCASE("V2") {
        random_device rd;
        mt19937 engine(rd());
        uniform_int_distribution<uint16_t> dist(0, 0x1ff);

        HeightmapV2 m;
        vector<pair<Pos2i, uint16_t>> expected;
        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++) {
                uint16_t v = dist(engine);
                expected.push_back(make_pair(Pos2i{x, z}, v));
            }
        }
        shuffle(expected.begin(), expected.end(), engine);
        for (auto const &it : expected) {
            m.setUnchecked(it.first.fX, it.first.fZ, it.second);
        }
        shuffle(expected.begin(), expected.end(), engine);
        for (auto const &it : expected) {
            CHECK(it.second == m.getUnchecked(it.first.fX, it.first.fZ));
        }
    }

    SUBCASE("V1") {
        random_device rd;
        mt19937 engine(rd());
        uniform_int_distribution<uint16_t> dist(0, 0x1ff);

        HeightmapV1 m;
        vector<pair<Pos2i, uint16_t>> expected;
        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++) {
                uint16_t v = dist(engine);
                expected.push_back(make_pair(Pos2i{x, z}, v));
            }
        }
        shuffle(expected.begin(), expected.end(), engine);
        for (auto const &it : expected) {
            m.setUnchecked(it.first.fX, it.first.fZ, it.second);
        }
        shuffle(expected.begin(), expected.end(), engine);
        for (auto const &it : expected) {
            CHECK(it.second == m.getUnchecked(it.first.fX, it.first.fZ));
        }
    }
}
