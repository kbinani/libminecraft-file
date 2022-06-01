#pragma once

namespace mcfile::nbt {

class DoubleTag : public Tag {
public:
    DoubleTag()
        : Tag()
        , fValue(0) {}
    explicit DoubleTag(double v)
        : fValue(v) {}

    Tag::Type type() const override { return Tag::Type::Double; }

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<DoubleTag> copy() const {
        return std::make_shared<DoubleTag>(fValue);
    }

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        uint64_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(double *)&v;
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        uint64_t v = *(uint64_t *)&fValue;
        return w.write(v);
    }

public:
    double fValue;
};

} // namespace mcfile::nbt
