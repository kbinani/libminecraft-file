#include <iostream>
#include "minecraft-file.hpp"
#include <set>
#include "svpng.inc"

using namespace std;
using namespace mcfile;

static constexpr uint32_t RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
}

static map<string, uint32_t> const blockToColor {
    {"minecraft:water", RGB(63, 63, 252)},
    {"minecraft:stone", RGB(111, 111, 111)},
    {"minecraft:granite", RGB(149, 108, 76)},
    {"minecraft:diorite", RGB(252, 249, 242)},
    {"minecraft:andesite", RGB(111, 111, 111)},
    {"minecraft:chest", RGB(141,118,71)},
    {"minecraft:clay", RGB(162,166,182)},
    {"minecraft:coal_ore", RGB(111,111,111)},
    {"minecraft:cobblestone", RGB(111,111,111)},
    {"minecraft:dirt", RGB(149,108,76)},
    {"minecraft:brown_mushroom", RGB(0,123,0)},
};

static set<string> transparentBlocks = {
    "minecraft:air",
    "minecraft:cave_air",
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }
    auto const input = string(argv[1]);
    auto const output = string(argv[2]);

    int const minX = -1024;
    int const minZ = -512;
    int const maxX = 1535;
    int const maxZ = 1023;

    int const width = maxX - minX + 1;
    int const height = maxZ - minZ + 1;
    vector<uint32_t> img(width * height, RGB(0, 0, 0, 0));

    World world(input);

    set<string> unknownBlockNames;

    int const minRegionX = Coordinate::RegionFromBlock(minX);
    int const maxRegionX = Coordinate::RegionFromBlock(maxX);
    int const minRegionZ = Coordinate::RegionFromBlock(minZ);
    int const maxRegionZ = Coordinate::RegionFromBlock(maxZ);
    for (int regionZ = minRegionZ; regionZ <= maxRegionZ; regionZ++) {
        for (int regionX = minRegionX; regionX <= maxRegionX; regionX++) {
            auto region = world.region(regionX, regionZ);
            if (!region) {
                continue;
            }
            cout << "[" << regionX << ", " << regionZ << "]" << endl;
            region->loadChunkDataSources([&](ChunkDataSource data, StreamReader& stream) {
                return data.load(stream, [&](Chunk const& chunk) {
                    for (int z = chunk.minZ(); z <= chunk.maxZ(); z++) {
                        for (int x = chunk.minX(); x <= chunk.maxX(); x++) {
                            for (int y = 255; y >= 0; y--) {
                                auto block = chunk.blockAt(x, y, z);
                                if (!block) {
                                    continue;
                                }
                                auto t = transparentBlocks.find(block->fName);
                                if (t != transparentBlocks.end()) {
                                    continue;
                                }
                                auto it = blockToColor.find(block->fName);
                                if (it == blockToColor.end()) {
                                    unknownBlockNames.insert(block->fName);
                                } else {
                                    int const index = (z - minZ) * width + (x - minX);
                                    img[index] = it->second;
                                    break;
                                }
                            }
                        }
                    }
                    return true;
                });
            });
        }
    }

    for (auto it : unknownBlockNames) {
        cerr << it << endl;
    }

    FILE *out = fopen(output.c_str(), "wb");
    if (!out) {
        return 1;
    }
    svpng(out, width, height, (unsigned char *)img.data(), 1);
    fclose(out);

    return 0;
}
