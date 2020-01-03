#pragma once

namespace mcfile {
namespace nbt {

class LongTag : public detail::ScalarTag<int64_t, Tag::TAG_Long> {};

} // namespace nbt
} // namespace mcfile
