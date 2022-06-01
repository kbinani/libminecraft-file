#pragma once

namespace mcfile::nbt {

class FloatTag : public Tag {
public:
    FloatTag()
        : Tag()
        , fValue(0) {}
    explicit FloatTag(float v)
        : fValue(v) {}

    Tag::Type type() const override { return Tag::Type::Float; }

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<FloatTag> copy() const {
        return std::make_shared<FloatTag>(fValue);
    }

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        uint32_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(float *)&v;
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        uint32_t v = *(uint32_t *)&fValue;
        return w.write(v);
    }

public:
    float fValue;
};

} // namespace mcfile::nbt
