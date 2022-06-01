#pragma once

namespace mcfile::nbt {

class IntTag : public detail::ScalarTag<int32_t, Tag::Type::Int> {
public:
    IntTag()
        : ScalarTag() {}
    explicit IntTag(int32_t v)
        : ScalarTag(v) {}

    std::shared_ptr<IntTag> copy() const {
        return std::make_shared<IntTag>(fValue);
    }
};

} // namespace mcfile::nbt
