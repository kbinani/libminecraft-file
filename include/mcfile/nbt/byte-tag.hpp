#pragma once

namespace mcfile::nbt {

class ByteTag : public detail::ScalarTag<uint8_t, Tag::Type::Byte> {
public:
    ByteTag()
        : ScalarTag() {}
    explicit ByteTag(uint8_t v)
        : ScalarTag(v) {}

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<ByteTag> copy() const {
        return std::make_shared<ByteTag>(fValue);
    }

    void toSnbt(std::ostream &out, SnbtOptions const &) const override {
        int8_t v = *(int8_t *)&fValue;
        out << (int)v << "b";
    }
};

} // namespace mcfile::nbt
