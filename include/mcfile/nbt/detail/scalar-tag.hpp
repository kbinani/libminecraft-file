#pragma once

namespace mcfile {
namespace nbt {
namespace detail {

template< typename T, uint8_t ID>
class ScalarTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        T v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = v;
        return true;
    }

    uint8_t id() const override { return ID; }

public:
    T fValue;
};

} // namespace detail
} // namespace nbt
} // namespace mcfile
