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

                    auto root = tag->compoundTag("");
                    if (!root) {
                        continue;
                    }
                    auto level = root->compoundTag("Level");
                    if (!level) {
                        continue;
                    }
                    auto sections = level->listTag("Sections");
                    if (!sections) {
                        continue;
                    }
                    for (auto const &section : *sections) {
                        auto c = section->asCompound();
                        if (!c) {
                            continue;
                        }
                        auto blocks = c->byteArrayTag("Blocks");
                        auto data = c->byteArrayTag("Data");
                        //TODO:
                    }
                }
            }
        }
    }

    fs::remove_all(*tempDir);
}
