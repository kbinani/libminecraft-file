#pragma once

namespace mcfile {
namespace nbt {

class IntTag : public detail::ScalarTag<int32_t, Tag::Type::Int> {
public:
    IntTag()
        : ScalarTag() {}
    explicit IntTag(int32_t v)
        : ScalarTag(v) {}
};

} // namespace nbt
} // namespace mcfile
