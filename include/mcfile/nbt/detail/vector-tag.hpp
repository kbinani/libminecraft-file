#pragma once

namespace mcfile {
namespace nbt {
namespace detail {

template<typename T, Tag::Type ID>
class VectorTag : public Tag {
public:
    VectorTag()
        : Tag()
        , fPrepared(false)
        , fLittleEndian(false) {
    }

    explicit VectorTag(std::vector<T> &input)
        : Tag()
        , fPrepared(true)
        , fLittleEndian(false) {
        fValue.swap(input);
    }

    explicit VectorTag(size_t size)
        : Tag()
        , fPrepared(true)
        , fLittleEndian(false) {
        fValue.resize(size);
    }

    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        uint32_t length;
        if (!r.read(&length)) {
            return false;
        }
        fValue.resize(length);
        if (!r.copy(fValue)) {
            return false;
        }
        fLittleEndian = r.isLittleEndian();
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        std::vector<T> const &b = value();
        if (!w.write((uint32_t)fValue.size())) {
            return false;
        }
        for (size_t i = 0; i < b.size(); i++) {
            if (!w.write(b[i])) {
                return false;
            }
        }
        return true;
    }

    Tag::Type type() const override { return ID; }

    std::vector<T> const &value() const {
        if (!fPrepared) {
            for (size_t i = 0; i < fValue.size(); i++) {
                fValue[i] = convert(fValue[i], fLittleEndian);
            }
            fPrepared = true;
        }
        return fValue;
    }

protected:
    virtual T convert(T v, bool littleEndian) const = 0;

private:
    std::vector<T> mutable fValue;
    bool mutable fPrepared = false;
    bool fLittleEndian;
};

} // namespace detail
} // namespace nbt
} // namespace mcfile
