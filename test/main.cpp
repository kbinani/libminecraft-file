#include <iostream>
#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }
    auto const input = string(argv[1]);

    auto region = Region::MakeRegion(input, 0, 0);
    region->loadChunkDataSource([](int x, int z, ChunkDataSource data, StreamReader& reader) {
        cout << "x=" << x << "; z=" << z << "; timestamp=" << data.timestamp << "; offset=" << data.offset << "; length=" << data.length << endl;
        
        data.open(reader, [](std::vector<uint8_t>& d) {
            auto root = std::make_shared<nbt::CompoundTag>();
            auto bs = std::make_shared<ByteStream>(d);
            auto sr = std::make_shared<StreamReader>(bs);
            root->read(*sr);
            std::cout << std::boolalpha << root->valid() << std::endl;
        });
    });
    return 0;
}
