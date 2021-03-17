#pragma once

namespace mcfile {
namespace nbt {

class DoubleTag : public Tag {
public:
    DoubleTag() : Tag(), fValue(0) {}
    explicit DoubleTag(double v) : fValue(v) {}

    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        uint64_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(double *)&v;
        return true;
    }

    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {
        uint64_t v = *(uint64_t *)&fValue;
        w.write(v);
    }
    
    uint8_t id() const override { return Tag::TAG_Double; }

    std::shared_ptr<Tag> clone() const override {
        return std::make_shared<DoubleTag>(fValue);
    }

public:
    double fValue;
};

} // namespace nbt
} // namespace mcfile
