#pragma once

namespace mcfile {
namespace nbt {

class ByteTag : public detail::ScalarTag<uint8_t, Tag::TAG_Byte> {};

} // namespace nbt
} // namespace mcfile
