#pragma once

namespace mcfile::nbt {

class ByteTag : public detail::ScalarTag<uint8_t, Tag::Type::Byte> {
public:
    ByteTag()
        : ScalarTag() {}
    explicit ByteTag(uint8_t v)
        : ScalarTag(v) {}

    std::shared_ptr<ByteTag> copy() const {
        return std::make_shared<ByteTag>(fValue);
    }
};

} // namespace mcfile::nbt
