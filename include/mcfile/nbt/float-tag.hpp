#pragma once

namespace mcfile {
namespace nbt {

class FloatTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        uint32_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(float *)&v;
        return true;
    }

    uint8_t id() const override { return Tag::TAG_Float; }

public:
    float fValue;
};

} // namespace nbt
} // namespace mcfile
