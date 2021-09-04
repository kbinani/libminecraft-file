#pragma once

namespace mcfile::be {

class PendingTick {
public:
    static std::optional<std::pair<Pos3i, PendingTick>> FromCompound(nbt::CompoundTag const &tag) {
        auto blockState = tag.compoundTag("blockState");
        auto time = tag.int64("time");
        auto x = tag.int32("x");
        auto y = tag.int32("y");
        auto z = tag.int32("z");
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

        Pos3i pos(*x, *y, *z);
        return std::make_pair(pos, pt);
    }

public:
    std::shared_ptr<Block const> fBlockState;
    int64_t fTime;
};

} // namespace mcfile::be
