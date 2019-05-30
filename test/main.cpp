#include <iostream>
#include "minecraft-file.hpp"
#include <math.h>
#include <set>
#include "svpng.inc"
#include "hwm/task/task_queue.hpp"
#include "colormap/colormap.h"

using namespace std;
using namespace mcfile;

struct HSV {
    double fH;
    double fS;
    double fV;
};


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

    HSV toHSV() const {
        double r = fR;
        double g = fG;
        double b = fB;
        double max = r > g ? r : g;
        max = max > b ? max : b;
        double min = r < g ? r : g;
        min = min < b ? min : b;
        double h = max - min;
        if (h > 0.0) {
            if (max == r) {
                h = (g - b) / h;
                if (h < 0.0) {
                    h += 6.0;
                }
            } else if (max == g) {
                h = 2.0 + (b - r) / h;
            } else {
                h = 4.0 + (r - g) / h;
            }
        }
        h /= 6.0;
        double s = (max - min);
        if (max != 0.0)
            s /= max;
        double v = max;
        HSV ret;
        ret.fH = h;
        ret.fS = s;
        ret.fV = v;
        return ret;
    }

    static Color FromHSV(HSV c) {
        double v = c.fV;
        double s = c.fS;
        double h = c.fH;
        double r = v;
        double g = v;
        double b = v;
        if (s > 0.0) {
            h *= 6.0;
            int i = (int) h;
            double f = h - (float) i;
            switch (i) {
                default:
                case 0:
                    g *= 1 - s * (1 - f);
                    b *= 1 - s;
                    break;
                case 1:
                    r *= 1 - s * f;
                    b *= 1 - s;
                    break;
                case 2:
                    r *= 1 - s;
                    b *= 1 - s * (1 - f);
                    break;
                case 3:
                    r *= 1 - s;
                    g *= 1 - s * f;
                    break;
                case 4:
                    r *= 1 - s * (1 - f);
                    g *= 1 - s;
                    break;
                case 5:
                    g *= 1 - s;
                    b *= 1 - s * f;
                    break;
            }
        }
        return FromDouble(r, g, b, 1);
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
    {"minecraft:grass_block", Color(130, 148, 58)},
    {"minecraft:iron_ore", Color(111, 111, 111)},
    {"minecraft:sand", Color(201,192,154)}, //
    {"minecraft:oak_leaves", Color(56, 95, 31)}, //
    {"minecraft:jungle_leaves", Color(56, 95, 31)}, //
    {"minecraft:birch_leaves", Color(67, 124, 37)},
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
    {"minecraft:grass_path", Color(204, 204, 204)}, //
    {"minecraft:birch_fence", Color(244, 230, 161)},
    {"minecraft:birch_planks", Color(244, 230, 161)},
    {"minecraft:birch_stairs", Color(244, 230, 161)},
    {"minecraft:dark_oak_fence", Color(101, 75, 50)},
    {"minecraft:dark_oak_log", Color(101, 75, 50)},
    {"minecraft:dark_oak_planks", Color(191,152,63)}, //
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
    {"minecraft:lava", Color(179, 71, 3)},
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
    {"minecraft:snow", Color(229, 229, 229)}, //
    {"minecraft:snow_block", Color(252, 252, 252)},
    {"minecraft:spruce_door", Color(141, 118, 71)},
    {"minecraft:spruce_fence", Color(141, 118, 71)},
    {"minecraft:spruce_leaves", Color(56, 95, 31)}, //
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
    {"minecraft:melon", Color(125, 202, 25)},
    {"minecraft:pumpkin", Color(213, 125, 50)},
    {"minecraft:grass", Color(109, 141, 35)},
    {"minecraft:tall_grass", Color(109, 141, 35)},
    {"minecraft:dandelion", Color(245, 238, 50)},
    {"minecraft:poppy", Color(229, 31, 29)},
    {"minecraft:peony", Color(232, 143, 213)},
    {"minecraft:pink_tulip", Color(234, 182, 209)},
    {"minecraft:orange_tulip", Color(242, 118, 33)},
    {"minecraft:lilac", Color(212, 119, 197)},
    {"minecraft:sunflower", Color(245, 238, 50)},
    {"minecraft:allium", Color(200, 109, 241)},
    {"minecraft:red_tulip", Color(229, 31, 29)},
    {"minecraft:white_tulip", Color(255, 255, 255)},
    {"minecraft:rose_bush", Color(136, 40, 27)},
    {"minecraft:blue_orchid", Color(47, 181, 199)},
    {"minecraft:oxeye_daisy", Color(236, 246, 247)},
    {"minecraft:sugar_cane", Color(165, 214, 90)},
};

static set<string> plantBlocks = {
    "minecraft:beetroots",
    "minecraft:carrots",
    "minecraft:potatoes",
    "minecraft:seagrass",
    "minecraft:tall_seagrass",
    "minecraft:fern",
    "minecraft:azure_bluet",
    "minecraft:kelp",
    "minecraft:large_fern",
    "minecraft:kelp_plant",
};

static set<string> transparentBlocks = {
    "minecraft:air",
    "minecraft:cave_air",
    "minecraft:vine", // Color(56, 95, 31)}, //
    "minecraft:ladder", // Color(255, 255, 255)},
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
        int fX;
        int fZ;
        int fWidth;
        int fHeight;
        vector<Color> fPixels;
        vector<uint8_t> fHeightMap;
    };
    vector<future<QueueResult>> futures;

    for (int regionZ = minRegionZ; regionZ <= maxRegionZ; regionZ++) {
        for (int regionX = minRegionX; regionX <= maxRegionX; regionX++) {
            auto region = world.region(regionX, regionZ);
            if (!region) {
                continue;
            }
            futures.emplace_back(q.enqueue([minX, maxX, minZ, maxZ](shared_ptr<Region> region) {
                int const x0 = std::min(std::max(region->minBlockX(), minX), maxX);
                int const z0 = std::min(std::max(region->minBlockZ(), minZ), maxZ);
                int const x1 = std::min(std::max(region->maxBlockX(), minX), maxX);
                int const z1 = std::min(std::max(region->maxBlockZ(), minZ), maxZ);
                QueueResult result;
                result.fX = x0;
                result.fZ = z0;
                int const w = x1 - x0 + 1;
                int const h = z1 - z0 + 1;
                result.fWidth = w;
                result.fHeight = h;
                result.fPixels.resize(w * h, Color(0, 0, 0, 0));
                result.fHeightMap.resize(w * h, 0);

                region->loadAllChunks([&result, x0, z0, w, minX, maxX, minZ, maxZ](Chunk const& chunk) {
                    Color const waterColor(69, 91, 211);
                    double const waterDiffusion = 0.02;
                    colormap::kbinani::Altitude altitude;
                    int const sZ = std::min(std::max(chunk.minBlockZ(), minZ), maxZ);
                    int const eZ = std::min(std::max(chunk.maxBlockZ(), minZ), maxZ);
                    int const sX = std::min(std::max(chunk.minBlockX(), minX), maxX);
                    int const eX = std::min(std::max(chunk.maxBlockX(), minX), maxX);
                    for (int z = sZ; z <= eZ; z++) {
                        for (int x = sX; x <= eX; x++) {
                            int waterDepth = 0;
                            int airDepth = 0;
                            Color translucentBlock(0, 0, 0, 0);
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
                                    int const index = (z - z0) * w + (x - x0);
                                    Color const opaqeBlockColor = it->second;
                                    Color color(0, 0, 0, 0);
                                    if (waterDepth > 0) {
                                        color = waterColor.diffuse(waterDiffusion, waterDepth);
                                        translucentBlock = Color(0, 0, 0, 0);
                                    } else if (block->fName == "minecraft:grass_block") {
                                        double const v = std::min(std::max((y - 63.0) / 193.0, 0.0), 1.0);
                                        auto c = altitude.getColor(v);
                                        color = Color::FromDouble(c.r, c.g, c.b, 1);
                                        result.fHeightMap[index] = y;
                                    } else {
                                        color = opaqeBlockColor;
                                        result.fHeightMap[index] = y;
                                    }
                                    result.fPixels[index] = Color::Add(color, translucentBlock.withAlphaComponent(0.2));
                                    break;
                                }
                            }
                        }
                    }
                    return true;
                });
                return result;
            }, region));
        }
    }

    vector<Color> img(width * height, Color(0, 0, 0, 0));
    vector<uint8_t> heightMap(width * height, 0);
    vector<uint32_t> pixels(width * height);

    for (auto& f : futures) {
        QueueResult result = f.get();
        for (int i = 0; i < result.fWidth; i++) {
            for (int j = 0; j < result.fHeight; j++) {
                int const x = result.fX + i;
                int const z = result.fZ + j;
                int const idx = (z - minZ) * width + (x - minX);
                img[idx] = result.fPixels[j * result.fWidth + i];
                heightMap[idx] = result.fHeightMap[j * result.fWidth + i];
            }
        }
    }
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            int const idx = z * width + x;
            uint8_t const h = heightMap[idx];
            Color c = img[idx];
            Color color = c;
            if (h == 0) {
                Color cNorth = c;
                Color cEast = c;
                Color cSouth = c;
                Color cWest = c;
                if (z > 1) {
                    uint8_t hh = heightMap[(z - 1) * width + x];
                    if (hh == 0) {
                        cNorth = img[(z - 1) * width + x];
                    }
                }
                if (z + 1 < height) {
                    uint8_t hh = heightMap[(z + 1) * width + x];
                    if (hh == 0) {
                        cSouth = img[(z + 1) * width + x];
                    }
                }
                if (x > 1) {
                    auto hh = heightMap[z * width + x - 1];
                    if (hh == 0) {
                        cWest = img[z * width + x - 1];
                    }
                }
                if (x + 1 < width) {
                    auto hh = heightMap[z * width + x + 1];
                    if (hh > 0) {
                        cEast = img[z * width + x + 1];
                    }
                }
                double r = (c.fR * c.fA + cNorth.fR * cNorth.fA / 4 + cEast.fR * cEast.fA / 4 + cSouth.fR * cSouth.fA / 4 + cWest.fR * cWest.fA / 4) / 2;
                double g = (c.fG * c.fA + cNorth.fG * cNorth.fA / 4 + cEast.fG * cEast.fA / 4 + cSouth.fG * cSouth.fA / 4 + cWest.fG * cWest.fA / 4) / 2;
                double b = (c.fB * c.fA + cNorth.fB * cNorth.fA / 4 + cEast.fB * cEast.fA / 4 + cSouth.fB * cSouth.fA / 4 + cWest.fB * cWest.fA / 4) / 2;
                color = Color::FromDouble(r, g, b, 1);
            }

            uint8_t hNorth = h;
            uint8_t hEast = h;
            uint8_t hSouth = h;
            uint8_t hWest = h;
            if (z > 1) {
                uint8_t hh = heightMap[(z - 1) * width + x];
                if (hh > 0) {
                    hNorth = hh;
                }
            }
            if (z + 1 < height) {
                uint8_t hh = heightMap[(z + 1) * width + x];
                if (hh > 0) {
                    hSouth = hh;
                }
            }
            if (x > 1) {
                auto hh = heightMap[z * width + x - 1];
                if (hh > 0) {
                    hWest = hh;
                }
            }
            if (x + 1 < width) {
                auto hh = heightMap[z * width + x + 1];
                if (hh > 0) {
                    hEast = hh;
                }
            }
            int score = 0; // +: bright, -: dark
            if (hNorth > h) score--;
            if (hNorth < h) score++;
            if (hWest > h) score--;
            if (hWest < h) score++;

            if (score > 0) {
                double coeff = 1.2;
                HSV hsv = color.toHSV();
                hsv.fV = hsv.fV * coeff;
                Color cc = Color::FromHSV(hsv);
                pixels[idx] = cc.color();
            } else if (score < 0) {
                double coeff = 0.8;
                HSV hsv = color.toHSV();
                hsv.fV = hsv.fV * coeff;
                Color cc = Color::FromHSV(hsv);
                pixels[idx] = cc.color();
            } else {
                pixels[idx] = color.color();
            }
        }
    }

    FILE *out = fopen(output.c_str(), "wb");
    if (!out) {
        return 1;
    }
    svpng(out, width, height, (unsigned char *)pixels.data(), 1);
    fclose(out);

    return 0;
}
