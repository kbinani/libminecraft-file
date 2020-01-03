#pragma once

namespace mcfile {
namespace nbt {

class ShortTag : public detail::ScalarTag<int16_t, Tag::TAG_Short> {};

} // namespace nbt
} // namespace mcfile
