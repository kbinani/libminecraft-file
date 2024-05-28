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
        std::vector<uint8_t> c = fValue;
        return std::make_shared<ByteArrayTag>(c);
    }

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    void toSnbt(std::ostream &out, SnbtOptions const &) const override {
        out << "[B;";
        for (size_t i = 0; i < fValue.size(); i++) {
            if (i > 0) {
                out << ",";
            }
            out << (int)fValue[i] << "b";
        }
        out << "]";
    }
};

} // namespace mcfile::nbt
