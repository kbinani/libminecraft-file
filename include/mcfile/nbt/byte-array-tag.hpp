#pragma once

namespace mcfile {
namespace nbt {

class ByteArrayTag : public detail::VectorTag<uint8_t, Tag::TAG_Byte_Array> {
public:
    ByteArrayTag() : VectorTag() {}

    explicit ByteArrayTag(std::vector<uint8_t>& input) : VectorTag(input) {}

private:
    uint8_t convert(uint8_t v, bool littleEndian) const override { return v; }
};

} // namespace nbt
} // namespace mcfile
