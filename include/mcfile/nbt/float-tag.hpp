#pragma once

namespace mcfile {
namespace nbt {

class FloatTag : public Tag {
public:
    FloatTag()
        : Tag()
        , fValue(0) {}
    explicit FloatTag(float v)
        : fValue(v) {}

    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        uint32_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(float *)&v;
        return true;
    }

    void writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        uint32_t v = *(uint32_t *)&fValue;
        w.write(v);
    }

    Tag::Type type() const override { return Tag::Type::Float; }

    std::shared_ptr<Tag> clone() const override {
        return std::make_shared<FloatTag>(fValue);
    }

public:
    float fValue;
};

} // namespace nbt
} // namespace mcfile
