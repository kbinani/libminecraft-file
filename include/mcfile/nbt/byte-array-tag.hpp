#pragma once

namespace mcfile {
namespace nbt {

class ByteArrayTag : public detail::VectorTag<uint8_t, Tag::TAG_Byte_Array> {
private:
    uint8_t convert(uint8_t v) const override { return v; }
};

} // namespace nbt
} // namespace mcfile
