#pragma once

namespace mcfile::blocks::data::type {

class Stairs : public BlockData, public Bisected, public Directional {
public:
    explicit Stairs(mcfile::je::Block const &j)
        : BlockData(j) {}

    explicit Stairs(mcfile::be::Block const &b)
        : BlockData(b) {}

    BlockFace facing() const override {
        if (fJava) {
            auto f = fJava->property(u8"facing");
            if (f == u8"north") {
                return BlockFace::North;
            } else if (f == u8"east") {
                return BlockFace::East;
            } else if (f == u8"west") {
                return BlockFace::West;
            } else {
                return BlockFace::South;
            }
        } else {
            auto d = fBedrock->fStates->int32(u8"weirdo_direction", 0);
            switch (d) {
            case 3: // north
                return BlockFace::North;
            case 1: // west
                return BlockFace::West;
            case 0: // east
                return BlockFace::East;
            case 2: // south
            default:
                return BlockFace::South;
            }
        }
    }

    Half half() const override {
        if (fJava) {
            return fJava->property(u8"half") == u8"top" ? Half::Top : Half::Bottom;
        } else {
            return fBedrock->fStates->boolean(u8"upside_down_bit", false) ? Half::Top : Half::Bottom;
        }
    }
};

} // namespace mcfile::blocks::data::type
