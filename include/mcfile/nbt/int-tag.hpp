#pragma once

namespace mcfile {
namespace nbt {

class IntTag : public detail::ScalarTag<int32_t, Tag::TAG_Int> {};

} // namespace nbt
} // namespace mcfile
