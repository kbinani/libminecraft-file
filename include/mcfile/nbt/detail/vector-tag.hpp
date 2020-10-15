#pragma once

namespace mcfile {
namespace nbt {
namespace detail {

template<typename T, uint8_t ID>
class VectorTag : public Tag {
public:
    VectorTag()
        : Tag()
        , fPrepared(false)
    {
    }

    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        uint32_t length;
        if (!r.read(&length)) {
            return false;
        }
        fValue.resize(length);
        if (!r.copy(fValue)) {
            return false;
        }
        return true;
    }

    uint8_t id() const override { return ID; }

    std::vector<T> const& value() const {
        if (!fPrepared) {
            for (size_t i = 0; i < fValue.size(); i++) {
                fValue[i] = convert(fValue[i]);
            }
            fPrepared = true;
        }
        return fValue;
    }

protected:
    virtual T convert(T v) const = 0;

private:
    std::vector<T> mutable fValue;
    bool mutable fPrepared = false;
};

} // namespace detail
} // namespace nbt
} // namespace mcfile
