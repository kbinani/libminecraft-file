#pragma once

namespace mcfile {
namespace nbt {
namespace detail {

template< typename T, uint8_t ID>
class ScalarTag : public Tag {
public:
    ScalarTag() : Tag() {}

    explicit ScalarTag(T v) {
        fValue = v;
        fValid = true;
    }

    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        T v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = v;
        return true;
    }

    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {
        w.write(fValue);
    }

    uint8_t id() const override { return ID; }

    std::shared_ptr<Tag> clone() const override {
        return std::make_shared<ScalarTag>(fValue);
    }

public:
    T fValue;
};

} // namespace detail
} // namespace nbt
} // namespace mcfile
