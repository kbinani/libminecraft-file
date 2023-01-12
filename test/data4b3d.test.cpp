#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace mcfile;
using namespace std;

TEST_CASE("data4b3d") {
    random_device rd;
    mt19937 engine(rd());
    uniform_int_distribution<uint32_t> dist(0, 0xf);
    uniform_int_distribution<uint32_t> sizeDist(1, 16);

    Pos3i origin{(uint8_t)dist(engine), (uint8_t)dist(engine), (uint8_t)dist(engine)};
    int widthX = sizeDist(engine);
    int height = sizeDist(engine);
    int widthZ = sizeDist(engine);

    Data4b3d data(origin, widthX, height, widthZ);
    for (int y = 0; y < height; y++) {
        for (int z = 0; z < widthZ; z++) {
            for (int x = 0; x < widthX; x++) {
                Pos3i v{x, y, z};
                CHECK(data.getUnchecked(origin + v) == 0);
            }
        }
    }

    unordered_map<Pos3i, uint8_t, Pos3iHasher> values;
    for (int y = 0; y < height; y++) {
        for (int z = 0; z < widthZ; z++) {
            for (int x = 0; x < widthX; x++) {
                Pos3i v{x, y, z};
                uint8_t next = dist(engine);
                data.setUnchecked(origin + v, next);
                values[origin + v] = next;
            }
        }
    }

    for (auto const &it : values) {
        Pos3i p = it.first;
        uint8_t v = it.second;
        CHECK(data.getUnchecked(p) == v);
    }
}
