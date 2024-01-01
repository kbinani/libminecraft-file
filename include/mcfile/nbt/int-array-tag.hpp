#pragma once

namespace mcfile ::nbt {

class IntArrayTag : public detail::VectorTag<int32_t, Tag::Type::IntArray> {
public:
    IntArrayTag()
        : VectorTag() {}

    explicit IntArrayTag(std::vector<int32_t> &input)
        : VectorTag(input) {}

    explicit IntArrayTag(size_t size)
        : VectorTag(size) {}

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<IntArrayTag> copy() const {
        std::vector<int32_t> c = fValue;
        return std::make_shared<IntArrayTag>(c);
    }
};

} // namespace mcfile::nbt
