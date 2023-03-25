#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;
using namespace mcfile::je;
using namespace mcfile::nbt;
using namespace mcfile::stream;
namespace fs = std::filesystem;

TEST_CASE("flatten") {
    auto const file = fs::path(__FILE__);
    auto const dir = file.parent_path();

    auto tempDir = File::CreateTempDir(fs::temp_directory_path());
    CHECK(tempDir);
    auto chunkFile = *tempDir / "chunk.nbt";

    vector<uint8_t> add(2048);

    for (int rx = -1; rx <= 0; rx++) {
        for (int rz = -1; rz <= 0; rz++) {
            string name = Region::GetDefaultRegionFileName(rx, rz);
            auto region = Region::MakeRegion(dir / "data" / "1.12" / "world-type=debug-mode" / "region" / name);
            for (int cx = region->minChunkX(); cx <= region->maxChunkX(); cx++) {
                for (int cz = region->minChunkZ(); cz <= region->maxChunkZ(); cz++) {
                    CHECK(region->exportToNbt(cx, cz, chunkFile));
                    auto tag = make_shared<CompoundTag>();
                    auto s = make_shared<FileInputStream>(chunkFile);
                    InputStreamReader ios(s);
                    CHECK(tag->read(ios));

                    auto root = tag->compoundTag(u8"");
                    if (!root) {
                        continue;
                    }
                    auto level = root->compoundTag(u8"Level");
                    if (!level) {
                        continue;
                    }
                    auto sections = level->listTag(u8"Sections");
                    if (!sections) {
                        continue;
                    }
                    for (auto const &section : *sections) {
                        auto c = section->asCompound();
                        if (!c) {
                            continue;
                        }
                        auto blocksTag = c->byteArrayTag(u8"Blocks");
                        auto dataTag = c->byteArrayTag(u8"Data");
                        if (!blocksTag || !dataTag) {
                            continue;
                        }
                        vector<uint8_t> const &blocks = blocksTag->value();
                        vector<uint8_t> const &data = dataTag->value();
                        for (int y = 0; y < 16; y++) {
                            for (int z = 0; z < 16; z++) {
                                for (int x = 0; x < 16; x++) {
                                    int index = y * 16 * 16 + z * 16 + x;
                                    uint8_t const idLo = blocks[index];
                                    uint8_t const idHi = Flatten::Nibble4(add, index);
                                    uint16_t const id = (uint16_t)idLo + ((uint16_t)idHi << 8);
                                    uint8_t const blockData = Flatten::Nibble4(data, index);
                                    auto block = Flatten::Block(id, blockData);
                                    CHECK(block != nullptr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fs::remove_all(*tempDir);
}
