#pragma once

namespace mcfile {
namespace nbt {

class IntArrayTag : public detail::VectorTag<int32_t, Tag::TAG_Int_Array> {
public:
    IntArrayTag() : VectorTag() {}

    explicit IntArrayTag(std::vector<int32_t>& input) : VectorTag(input) {}

private:
    int32_t convert(int32_t v, bool littleEndian) const override {
        uint32_t t = *(uint32_t *)&v;
        if (littleEndian) {
            t = ::mcfile::detail::Int32FromLE(t);
        } else {
            t = ::mcfile::detail::Int32FromBE(t);
        }
        return *(int32_t *)&t;
    }
};

} // namespace nbt
} // namespace mcfile
