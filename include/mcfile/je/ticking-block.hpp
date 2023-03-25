#pragma once

namespace mcfile::je {

struct TickingBlock {
    std::u8string fI;
    int32_t fP;
    int32_t fT;
    int32_t fX;
    int32_t fY;
    int32_t fZ;

    static std::optional<TickingBlock> FromCompound(nbt::CompoundTag const &tag) {
        auto i = tag.string(u8"i");
        auto p = tag.int32(u8"p");
        auto t = tag.int32(u8"t");
        auto x = tag.int32(u8"x");
        auto y = tag.int32(u8"y");
        auto z = tag.int32(u8"z");
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

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto t = make_shared<nbt::CompoundTag>();
        t->set(u8"i", make_shared<nbt::StringTag>(fI));
        t->set(u8"p", make_shared<nbt::IntTag>(fP));
        t->set(u8"t", make_shared<nbt::IntTag>(fT));
        t->set(u8"x", make_shared<nbt::IntTag>(fX));
        t->set(u8"y", make_shared<nbt::IntTag>(fY));
        t->set(u8"z", make_shared<nbt::IntTag>(fZ));
        return t;
    }
};

} // namespace mcfile::je
