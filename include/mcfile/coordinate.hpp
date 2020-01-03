#pragma once

namespace mcfile {

class Coordinate {
private:
    Coordinate() = delete;

public:
    static int ChunkFromBlock(int block) {
        return block >> 4;
    }

    static int RegionFromChunk(int region) {
        return region >> 5;
    }

    static int RegionFromBlock(int block) {
        return block >> 9;
    }
};

} // namespace mcfile
