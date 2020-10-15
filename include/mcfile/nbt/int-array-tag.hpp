#pragma once

namespace mcfile {
namespace nbt {

class IntArrayTag : public detail::VectorTag<int32_t, Tag::TAG_Int_Array> {
private:
    int32_t convert(int32_t v) const override {
        uint32_t t = *(uint32_t *)&v;
        t = ::mcfile::stream::InputStreamReader::Int32FromBE(t);
        return *(int32_t *)&t;
    }
};

} // namespace nbt
} // namespace mcfile
