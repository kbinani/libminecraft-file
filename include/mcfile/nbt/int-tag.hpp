#pragma once

namespace mcfile::nbt {

class IntTag : public detail::ScalarTag<int32_t, Tag::Type::Int> {
public:
    IntTag()
        : ScalarTag() {}
    explicit IntTag(int32_t v)
        : ScalarTag(v) {}

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<IntTag> copy() const {
        return std::make_shared<IntTag>(fValue);
    }

    void toSnbt(std::ostream &out, SnbtOptions const &) const override {
        out << fValue;
    }
};

} // namespace mcfile::nbt
