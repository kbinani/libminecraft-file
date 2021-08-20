#pragma once

namespace mcfile {
namespace nbt {

class LongTag : public detail::ScalarTag<int64_t, Tag::Type::Long> {
public:
    LongTag() : ScalarTag() {}
    explicit LongTag(int64_t v) : ScalarTag(v) {}
};

} // namespace nbt
} // namespace mcfile
