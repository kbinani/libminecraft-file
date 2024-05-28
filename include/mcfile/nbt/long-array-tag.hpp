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
        std::vector<int64_t> c = fValue;
        return std::make_shared<LongArrayTag>(c);
    }

    void toSnbt(std::ostream &out, SnbtOptions const &) const override {
        out << "[L;";
        for (size_t i = 0; i < fValue.size(); i++) {
            if (i > 0) {
                out << ",";
            }
            out << fValue[i] << "l";
        }
        out << "]";
    }
};

} // namespace mcfile::nbt
