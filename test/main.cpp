#include <iostream>
#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;

static std::string nameToDisplay(std::shared_ptr<Block> const& block) {
    if (!block) {
        return "x";
    }
    auto const name = block->fName;
    if (name == "minecraft:air" || name == "minecraft:cave_air") {
        return " ";
    } else if (name == "minecraft:grass") {
        return "w";
    } else if (name == "minecraft:tall_grass") {
        return "W";
    } else if (name == "minecraft:grass_block" || name == "minecraft:dirt") {
        return "d";
    } else if (name == "minecraft:lilac") {
        return "f";
    } else if (name == "minecraft:oak_log") {
        return "L";
    } else if (name == "minecraft:sand") {
        return "s";
    } else if (name == "minecraft:stone") {
        return "S";
    } else if (name == "minecraft:oak_leaves") {
        return "l";
    } else {
        return "?";
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }
    auto const input = string(argv[1]);

    auto region = Region::MakeRegion(input);
    
    cout
        << "regionX=" << region->fX
        << "; regionZ=" << region->fZ
        << endl;
    
    region->loadChunkDataSources([](ChunkDataSource data, StreamReader& reader) {
        data.load(reader, [data](Chunk const& chunk) {
            for (int y = 0; y <= 90; y++) {
                for (int z = chunk.minZ(); z <= chunk.maxZ(); z++) {
                    for (int x = chunk.minX(); x <= chunk.maxX(); x++) {
                        auto block = chunk.blockAt(x, y, z);
                        cout << nameToDisplay(block);
                    }
                    cout << endl;
                }
                cout << "-----------------" << endl;
            }
            cout
                << "chunkX=" << chunk.fChunkX
                << "; chunkZ=" << chunk.fChunkZ
                << "; timestamp=" << data.fTimestamp
                << "; chunk.fSections.size()=" << chunk.fSections.size()
                << endl;
        });

        return false;
    });

    return 0;
}
