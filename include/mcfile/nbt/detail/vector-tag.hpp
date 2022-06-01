#pragma once

namespace mcfile::nbt::detail {

template<typename T, Tag::Type ID>
class VectorTag : public Tag {
public:
    VectorTag()
        : Tag() {
    }

    explicit VectorTag(std::vector<T> &input)
        : Tag() {
        fValue.swap(input);
    }

    explicit VectorTag(size_t size)
        : Tag() {
        fValue.resize(size);
    }

    Tag::Type type() const override { return ID; }

    std::vector<T> const &value() const {
        return fValue;
    }

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        uint32_t length;
        if (!r.read(&length)) {
            return false;
        }
        fValue.resize(length);
        if (!r.read(fValue)) {
            return false;
        }
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        uint32_t size = fValue.size();
        if (!w.write(size)) {
            return false;
        }
        for (size_t i = 0; i < size; i++) {
            if (!w.write(fValue[i])) {
                return false;
            }
        }
        return true;
    }

public:
    std::vector<T> fValue;
};

} // namespace mcfile::nbt::detail
