#pragma once

namespace mcfile::nbt {

class StringTag : public Tag {
public:
    StringTag()
        : Tag() {}
    explicit StringTag(std::u8string v)
        : fValue(v) {
    }

    Tag::Type type() const override { return Tag::Type::String; }

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<StringTag> copy() const {
        return std::make_shared<StringTag>(fValue);
    }

    bool equals(StringTag const &o) const {
        return fValue == o.fValue;
    }

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        std::string tmp;
        if (!r.read(tmp)) {
            return false;
        }
        if (auto s = String::Utf8FromJavaUtf8(tmp); s) {
            fValue = *s;
            return true;
        } else {
            return false;
        }
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        if (auto s = String::JavaUtf8FromUtf8(fValue); s) {
            return w.write(*s);
        } else {
            return false;
        }
    }

public:
    std::u8string fValue;
};

} // namespace mcfile::nbt
