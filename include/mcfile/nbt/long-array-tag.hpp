#pragma once

namespace mcfile::nbt {

class LongArrayTag : public detail::VectorTag<int64_t, Tag::Type::LongArray> {
public:
    LongArrayTag()
        : VectorTag() {}

    explicit LongArrayTag(std::vector<int64_t> &input)
        : VectorTag(input) {}

    explicit LongArrayTag(size_t size)
        : VectorTag(size) {}

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<LongArrayTag> copy() const {
        std::vector<int64_t> copy = value();
        return std::shared_ptr<LongArrayTag>(new LongArrayTag(copy));
    }
};

} // namespace mcfile::nbt
