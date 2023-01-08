#pragma once

namespace mcfile::nbt {

class LongTag : public detail::ScalarTag<int64_t, Tag::Type::Long> {
public:
    LongTag()
        : ScalarTag() {}
    explicit LongTag(int64_t v)
        : ScalarTag(v) {}

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<LongTag> copy() const {
        return std::make_shared<LongTag>(fValue);
    }
};

} // namespace mcfile::nbt
