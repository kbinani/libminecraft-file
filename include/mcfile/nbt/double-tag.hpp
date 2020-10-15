#pragma once

namespace mcfile {
namespace nbt {

class DoubleTag : public Tag {
public:
    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        uint64_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(double *)&v;
        return true;
    }

    uint8_t id() const override { return Tag::TAG_Double; }

public:
    double fValue;
};

} // namespace nbt
} // namespace mcfile
