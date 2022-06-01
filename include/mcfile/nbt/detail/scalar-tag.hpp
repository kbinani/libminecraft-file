#pragma once

namespace mcfile::nbt::detail {

template<typename T, Tag::Type ID>
class ScalarTag : public Tag {
public:
    ScalarTag()
        : Tag() {}

    explicit ScalarTag(T v) {
        fValue = v;
    }

    Tag::Type type() const override { return ID; }

    std::shared_ptr<Tag> clone() const override {
        return std::make_shared<ScalarTag>(fValue);
    }

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        T v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = v;
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        return w.write(fValue);
    }

public:
    T fValue;
};

} // namespace mcfile::nbt::detail
