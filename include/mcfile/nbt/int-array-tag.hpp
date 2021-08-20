#pragma once

namespace mcfile {
namespace nbt {

class IntArrayTag : public detail::VectorTag<int32_t, Tag::Type::IntArray> {
public:
    IntArrayTag() : VectorTag() {}

    explicit IntArrayTag(std::vector<int32_t>& input) : VectorTag(input) {}

    explicit IntArrayTag(size_t size) : VectorTag(size) {}

    std::shared_ptr<Tag> clone() const override {
        std::vector<int32_t> copy = value();
        return std::shared_ptr<IntArrayTag>(new IntArrayTag(copy));
    }

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
