#pragma once

namespace mcfile::nbt {

class ByteArrayTag : public detail::VectorTag<uint8_t, Tag::Type::ByteArray> {
public:
    ByteArrayTag()
        : VectorTag() {}

    explicit ByteArrayTag(std::vector<uint8_t> &input)
        : VectorTag(input) {}

    explicit ByteArrayTag(size_t size)
        : VectorTag(size) {}

    std::shared_ptr<ByteArrayTag> copy() const {
        std::vector<uint8_t> copy = value();
        return std::shared_ptr<ByteArrayTag>(new ByteArrayTag(copy));
    }

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }
};

} // namespace mcfile::nbt
