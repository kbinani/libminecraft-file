#include <iostream>
#include "minecraft-file.hpp"
#include <math.h>
#include <set>
#include "svpng.inc"
#include "hwm/task/task_queue.hpp"

using namespace std;
using namespace mcfile;

class Color {
public:
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : fR(r / 255.)
        , fG(g / 255.)
        , fB(b / 255.)
        , fA(a / 255.)
    {
    }

    uint32_t color() const {
        return ((uint32_t)0xFF << 24) | ((uint32_t)ToU8(fB * fA) << 16) | ((uint32_t)ToU8(fG * fA) << 8) | (uint32_t)ToU8(fR * fA);
    }

    Color diffuse(double diffusion, double distance) const {
        double intensity = pow(10., -diffusion * distance);
        return FromDouble(fR, fG, fB, fA * intensity);
    }

    Color withAlphaComponent(double a) const {
        return FromDouble(fR, fG, fB, a);
    }

    static Color Add(Color a, Color b) {
        return Color(a.fR * a.fA + b.fR * b.fA,
                     a.fG * a.fA + b.fG * b.fA,
                     a.fB * a.fA + b.fB * b.fA,
                     1., true);
    }

    static Color FromDouble(double r, double g, double b, double a) {
        return Color(r, g, b, a, true);
    }

    std::string toString() const {
        std::ostringstream ss;
        ss << "{R: " << fR << ", G: " << fG << ", B: " << fB << ", A: " << fA << "}";
        return ss.str();
    }

private:
    static uint8_t ToU8(double v) {
        double vv = v * 255;
        if (vv < 0) {
            return 0;
        } else if (255 < vv) {
            return 255;
        } else {
            return (uint8_t)vv;
        }
    }

    Color(double r, double g, double b, double a, bool)
        : fR(r)
        , fG(g)
        , fB(b)
        , fA(a)
    {
    }

public:
    double fR;
    double fG;
    double fB;
    double fA;
};

static map<string, Color> const blockToColor {
    {"minecraft:stone", Color(111, 111, 111)},
    {"minecraft:granite", Color(149, 108, 76)},
    {"minecraft:diorite", Color(252, 249, 242)},
    {"minecraft:andesite", Color(111, 111, 111)},
    {"minecraft:chest", Color(141, 118, 71)},
    {"minecraft:clay", Color(162, 166, 182)},
    {"minecraft:coal_ore", Color(111, 111, 111)},
    {"minecraft:cobblestone", Color(111, 111, 111)},
    {"minecraft:dirt", Color(149, 108, 76)},
    {"minecraft:brown_mushroom", Color(0, 123, 0)},
    {"minecraft:grass_block", Color(125, 176, 55)},
    {"minecraft:iron_ore", Color(111, 111, 111)},
    {"minecraft:sand", Color(149, 108, 76)},
    {"minecraft:oak_leaves", Color(0, 123, 0)},
    {"minecraft:jungle_leaves", Color(0, 123, 0)},
    {"minecraft:birch_leaves", Color(67, 124, 37)},
    {"minecraft:grass", Color(125, 176, 55)},
    {"minecraft:vine", Color(0, 123, 0)},
    {"minecraft:red_mushroom", Color(0, 123, 0)},
    {"minecraft:mossy_cobblestone", Color(111, 111, 111)},
    {"minecraft:oak_stairs", Color(127, 85, 48)},
    {"minecraft:gravel", Color(111, 111, 1110)},
    {"minecraft:oak_log", Color(141, 118, 71)},
    {"minecraft:oak_planks", Color(127, 85, 48)},
    {"minecraft:wall_torch", Color(255, 255, 2550)},
    {"minecraft:farmland", Color(149, 108, 76)},
    {"minecraft:oak_fence", Color(127, 85, 48)},
    {"minecraft:cobblestone_stairs", Color(111, 111, 111)},
    {"minecraft:black_wool", Color(25, 25, 25)},
    {"minecraft:ladder", Color(255, 255, 255)},
    {"minecraft:grass_path", Color(149, 108, 76)},
    {"minecraft:birch_fence", Color(244, 230, 161)},
    {"minecraft:birch_planks", Color(244, 230, 161)},
    {"minecraft:birch_stairs", Color(244, 230, 161)},
    {"minecraft:dark_oak_fence", Color(101, 75, 50)},
    {"minecraft:dark_oak_log", Color(101, 75, 50)},
    {"minecraft:dark_oak_planks", Color(101, 75, 50)},
    {"minecraft:dark_oak_slab", Color(101, 75, 50)},
    {"minecraft:dark_oak_stairs", Color(101, 75, 50)},
    {"minecraft:dark_oak_trapdoor", Color(141, 118, 71)},
    {"minecraft:diamond_ore", Color(111, 111, 111)},
    {"minecraft:gold_ore", Color(111, 111, 111)},
    {"minecraft:ice", Color(158, 158, 252)},
    {"minecraft:jungle_fence", Color(149, 108, 76)},
    {"minecraft:jungle_log", Color(149, 108, 76)},
    {"minecraft:jungle_planks", Color(149, 108, 76)},
    {"minecraft:jungle_slab", Color(149, 108, 76)},
    {"minecraft:jungle_stairs", Color(149, 108, 76)},
    {"minecraft:jungle_trapdoor", Color(141, 118, 71)},
    {"minecraft:lapis_ore", Color(111, 111, 111)},
    {"minecraft:lava", Color(252, 0, 0)},
    {"minecraft:oak_door", Color(141, 118, 71)},
    {"minecraft:oak_slab", Color(127, 85, 48)},
    {"minecraft:oak_trapdoor", Color(141, 118, 71)},
    {"minecraft:obsidian", Color(25, 25, 25)},
    {"minecraft:packed_ice", Color(158, 158, 252)},
    {"minecraft:polished_granite", Color(149, 108, 76)},
    {"minecraft:prismarine", Color(75, 125, 151)},
    {"minecraft:prismarine_bricks", Color(91, 216, 210)},
    {"minecraft:rail", Color(255, 255, 255)},
    {"minecraft:redstone_ore", Color(111, 111, 111)},
    {"minecraft:sandstone", Color(244, 230, 161)},
    {"minecraft:sea_lantern", Color(252, 249, 242)},
    {"minecraft:snow", Color(252, 252, 252)},
    {"minecraft:snow_block", Color(252, 252, 252)},
    {"minecraft:spruce_door", Color(141, 118, 71)},
    {"minecraft:spruce_fence", Color(141, 118, 71)},
    {"minecraft:spruce_leaves", Color(0, 123, 0)},
    {"minecraft:stone_brick_stairs", Color(111, 111, 111)},
    {"minecraft:stone_bricks", Color(111, 111, 111)},
    {"minecraft:stone_slab", Color(111, 111, 111)},
    {"minecraft:spruce_log", Color(141, 118, 71)},
    {"minecraft:spruce_planks", Color(127, 85, 48)},
    {"minecraft:spruce_slab", Color(127, 85, 48)},
    {"minecraft:spruce_stairs", Color(141, 118, 71)},
    {"minecraft:spruce_trapdoor", Color(141, 118, 71)},
    {"minecraft:mossy_stone_bricks", Color(111, 111, 111)},
    {"minecraft:chiseled_stone_bricks", Color(111, 111, 111)},
    {"minecraft:cracked_stone_bricks", Color(111, 111, 111)},
    {"minecraft:infested_stone", Color(111, 111, 111)},
    {"minecraft:cobweb", Color(255, 255, 255)},
    {"minecraft:blue_ice", Color(102, 151, 246)},
    {"minecraft:magma_block", Color(181, 64, 9)},

    // plants
    {"minecraft:lily_pad", Color(0, 123, 0)},
    {"minecraft:wheat", Color(0, 123, 0)},
};

static set<string> plantBlocks = {
    "minecraft:tall_grass",
    "minecraft:beetroots",
    "minecraft:carrots",
    "minecraft:dandelion",
    "minecraft:lilac",
    "minecraft:melon",
    "minecraft:orange_tulip",
    "minecraft:oxeye_daisy",
    "minecraft:peony",
    "minecraft:pink_tulip",
    "minecraft:poppy",
    "minecraft:potatoes",
    "minecraft:pumpkin",
    "minecraft:rose_bush",
    "minecraft:seagrass",
    "minecraft:sugar_cane",
    "minecraft:sunflower",
    "minecraft:tall_seagrass",
    "minecraft:fern",
    "minecraft:azure_bluet",
    "minecraft:allium",
    "minecraft:blue_orchid",
    "minecraft:kelp",
    "minecraft:large_fern",
    "minecraft:red_tulip",
    "minecraft:white_tulip",
    "minecraft:kelp_plant",
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

    World world(input);

    int const minRegionX = Coordinate::RegionFromBlock(minX);
    int const maxRegionX = Coordinate::RegionFromBlock(maxX);
    int const minRegionZ = Coordinate::RegionFromBlock(minZ);
    int const maxRegionZ = Coordinate::RegionFromBlock(maxZ);

    hwm::task_queue q(thread::hardware_concurrency());
    struct QueueResult {
        int x;
        int z;
        int width;
        int height;
        vector<uint32_t> data;
    };
    vector<future<QueueResult>> futures;


    for (int regionZ = minRegionZ; regionZ <= maxRegionZ; regionZ++) {
        for (int regionX = minRegionX; regionX <= maxRegionX; regionX++) {
            auto region = world.region(regionX, regionZ);
            if (!region) {
                continue;
            }
            futures.emplace_back(q.enqueue([minX, maxX, minZ, maxZ](shared_ptr<Region> region) {
                int const x0 = std::min(std::max(region->minX(), minX), maxX);
                int const z0 = std::min(std::max(region->minZ(), minZ), maxZ);
                int const x1 = std::min(std::max(region->maxX(), minX), maxX);
                int const z1 = std::min(std::max(region->maxZ(), minZ), maxZ);
                QueueResult result;
                result.x = x0;
                result.z = z0;
                int const w = x1 - x0 + 1;
                int const h = z1 - z0 + 1;
                result.width = w;
                result.height = h;
                result.data.resize(w * h, Color(0, 0, 0, 0).color());

                region->loadChunkDataSources([&result, x0, z0, w](ChunkDataSource data, StreamReader& stream) {
                    return data.load(stream, [&result, x0, z0, w](Chunk const& chunk) {
                        Color const waterColor(63, 63, 252);
                        double const waterDiffusion = 0.02;

                        for (int z = chunk.minZ(); z <= chunk.maxZ(); z++) {
                            for (int x = chunk.minX(); x <= chunk.maxX(); x++) {
                                int waterDepth = 0;
                                int airDepth = 0;
                                for (int y = 255; y >= 0; y--) {
                                    auto block = chunk.blockAt(x, y, z);
                                    if (!block) {
                                        airDepth++;
                                        continue;
                                    }
                                    if (block->fName == "minecraft:water" || block->fName == "minecraft:bubble_column") {
                                        waterDepth++;
                                        continue;
                                    }
                                    if (transparentBlocks.find(block->fName) != transparentBlocks.end()) {
                                        airDepth++;
                                        continue;
                                    }
                                    if (plantBlocks.find(block->fName) != plantBlocks.end()) {
                                        airDepth++;
                                        continue;
                                    }
                                    auto it = blockToColor.find(block->fName);
                                    if (it == blockToColor.end()) {
                                        cerr << "Unknown block: " << block->fName << endl;
                                    } else {
                                        Color const opaqeBlockColor = it->second;
                                        uint32_t color;
                                        if (waterDepth > 0) {
                                            auto const w = waterColor.diffuse(waterDiffusion, waterDepth);
                                            color = w.color();
                                        } else {
                                            color = opaqeBlockColor.color();
                                        }
                                        int const index = (z - z0) * w + (x - x0);
                                        result.data[index] = color;
                                        break;
                                    }
                                }
                            }
                        }
                    });
                });
                return result;
            }, region));
        }
    }

    vector<uint32_t> img(width * height, Color(0, 0, 0, 0).color());

    for (auto& f : futures) {
        QueueResult result = f.get();
        for (int i = 0; i < result.width; i++) {
            for (int j = 0; j < result.height; j++) {
                int const x = result.x + i;
                int const z = result.z + j;
                int const idx = (z - minZ) * width + (x - minX);
                img[idx] = result.data[j * result.width + i];
            }
        }
    }

    FILE *out = fopen(output.c_str(), "wb");
    if (!out) {
        return 1;
    }
    svpng(out, width, height, (unsigned char *)img.data(), 1);
    fclose(out);

    return 0;
}
