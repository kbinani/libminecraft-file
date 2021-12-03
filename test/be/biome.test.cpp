#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;
using namespace mcfile::be;
using namespace mcfile::biomes;

TEST_CASE("be/biome") {
    using namespace mcfile::biomes::minecraft;
    for (BiomeId b = 1; b <= minecraft::minecraft_max_biome_id; b++) {
        auto name = Name(b, 4000);
        if (name.empty()) {
            continue;
        }
        uint32_t iv = Biome::ToUint32(b);
        switch (b) {
        case ocean:
            CHECK(iv == 0);
            break;
        case the_void:
            break;
        default:
            CHECK(iv > 0);
            break;
        }
    }
}
