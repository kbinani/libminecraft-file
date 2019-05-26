#include <iostream>
#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }
    auto const input = string(argv[1]);

    auto region = Region::MakeRegion(input);
    region->loadChunkDataSources([](int chunkX, int chunkZ, ChunkDataSource data, StreamReader& reader) {
        cout
            << "x=" << chunkX
            << "; z=" << chunkZ
            << "; timestamp=" << data.fTimestamp
            << "; offset=" << data.fOffset
            << "; length=" << data.fLength
            << endl;
        
        data.load(reader, [](nbt::CompoundTag const& root) {
            cout
                << "name=" << root.name()
                << "; valid=" << boolalpha << root.valid()
                << endl;
            auto sections = root.query("/Level/Sections");
        });

        return false;
    });
    return 0;
}
