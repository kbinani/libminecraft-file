#pragma once

namespace mcfile {

struct TickingBlock {
    std::string fI;
    int32_t fP;
    int32_t fT;
    int32_t fX;
    int32_t fY;
    int32_t fZ;

    static std::optional<TickingBlock> FromCompound(nbt::CompoundTag const& tag) {
        auto i = tag.string("i");
        auto p = tag.int32("p");
        auto t = tag.int32("t");
        auto x = tag.int32("x");
        auto y = tag.int32("y");
        auto z = tag.int32("z");
        if (!i || !p || !t || !x || !y || !z) {
            return std::nullopt;
        }
        TickingBlock tt;
        tt.fI = *i;
        tt.fP = *p;
        tt.fT = *t;
        tt.fX = *x;
        tt.fY = *y;
        tt.fZ = *z;
        return tt;
    }
};

}
