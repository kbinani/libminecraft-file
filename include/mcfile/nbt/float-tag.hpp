#pragma once

namespace mcfile {
namespace nbt {

class FloatTag : public Tag {
public:
    FloatTag() : Tag() {}
    explicit FloatTag(float v) : fValue(v) {}

    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        uint32_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(float *)&v;
        return true;
    }

    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {
        uint32_t v = *(uint32_t *)&fValue;
        w.write(v);
    }
    
    uint8_t id() const override { return Tag::TAG_Float; }

public:
    float fValue;
};

} // namespace nbt
} // namespace mcfile
