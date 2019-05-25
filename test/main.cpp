#include <iostream>
#include "minecraft-file.hpp"

using namespace std;
using namespace mcfile;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }
    auto const input = string(argv[1]);

    Region r(input, 0, 0);
    r.loadChunkDataSource([](ChunkDataSource const& data) {
        cout << data.file_path << "; timestamp=" << data.timestamp << "; offset=" << data.offset << "; length=" << data.length << endl;
        data.open([](std::vector<uint8_t> const& d) {
            auto root = std::make_shared<nbt::CompoundTag>();
            auto bs = std::make_shared<ByteStream>(d);
            auto sr = std::make_shared<StreamReader>(bs);
            root->read(*sr);
            std::cout << std::boolalpha << root->valid() << std::endl;
        });
    });
    return 0;
}
