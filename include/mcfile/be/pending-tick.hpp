#pragma once

namespace mcfile::be {

class PendingTick {
public:
    static std::optional<PendingTick> FromCompound(nbt::CompoundTag const &tag) {
        auto blockState = tag.compoundTag(u8"blockState");
        auto time = tag.int64(u8"time");
        auto x = tag.int32(u8"x");
        auto y = tag.int32(u8"y");
        auto z = tag.int32(u8"z");
        if (!blockState || !time || !x || !y || !z) {
            return std::nullopt;
        }
        auto block = Block::FromCompound(*blockState);
        if (!block) {
            return std::nullopt;
        }
        PendingTick pt;
        pt.fBlockState = block;
        pt.fTime = *time;
        pt.fX = *x;
        pt.fY = *y;
        pt.fZ = *z;

        return pt;
    }

public:
    std::shared_ptr<Block const> fBlockState;
    int64_t fTime;
    int fX;
    int fY;
    int fZ;
};

} // namespace mcfile::be
