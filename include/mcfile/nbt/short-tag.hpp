#pragma once

namespace mcfile::nbt {

class ShortTag : public detail::ScalarTag<int16_t, Tag::Type::Short> {
public:
    ShortTag()
        : ScalarTag() {}
    explicit ShortTag(int16_t v)
        : ScalarTag(v) {}

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<ShortTag> copy() const {
        return std::make_shared<ShortTag>(fValue);
    }

    void toSnbt(std::ostream &out, SnbtOptions const &) const override {
        out << fValue << "s";
    }
};

} // namespace mcfile::nbt
