#pragma once

namespace mcfile::blocks::data {

class Directional {
public:
    enum class BlockFace : uint32_t {
        North = 0,
        NorthNorthEast = 1 << 1,
        NorthEast = 1 << 2,
        EastNorthEast = 1 << 3,
        East = 1 << 4,
        EastSouthEast = 1 << 5,
        SouthEast = 1 << 6,
        SouthSouthEast = 1 << 7,
        South = 1 << 8,
        SouthSouthWest = 1 << 9,
        SouthWest = 1 << 10,
        WestSouthWest = 1 << 11,
        West = 1 << 12,
        WestNorthWest = 1 << 13,
        NorthWest = 1 << 14,
        NorthNorthWest = 1 << 15,
        Up = 1 << 16,
        Self = 1 << 17,
        Down = 1 << 18,
    };

    virtual BlockFace facing() const = 0;
};

} // namespace mcfile::blocks::data
