#pragma once

namespace mcfile {
namespace nbt {

class LongArrayTag : public detail::VectorTag<int64_t, Tag::TAG_Long_Array> {
private:
    int64_t convert(int64_t v) const override {
        uint64_t t = *(uint64_t *)&v;
        t = ::mcfile::stream::InputStreamReader::Int64FromBE(t);
        return *(int64_t *)&t;
    }
};

} // namespace nbt
} // namespace mcfile
