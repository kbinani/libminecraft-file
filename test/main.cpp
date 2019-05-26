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

            auto sections = root.query("/Level/Sections")->asList();
            if (!sections) {
                return;
            }
            cout
                << "sections->name()=" << sections->name()
                << "; sections->fValue.size()=" << sections->fValue.size()
                << endl;
            for (auto s : sections->fValue) {
                nbt::CompoundTag const* section = s->asCompound();
                if (!section) {
                    continue;
                }
                nbt::ByteTag const* y = section->query("Y")->asByte();
                if (!y) {
                    continue;
                }
                nbt::LongArrayTag const* blockStates = section->query("BlockStates")->asLongArray();
                if (!blockStates) {
                    continue;
                }
                cout
                    << "y=" << (int)y->fValue
                    << "; blockStates->fValue.size()=" << blockStates->fValue.size()
                    << endl;
            }
        });

        return false;
    });
    return 0;
}
