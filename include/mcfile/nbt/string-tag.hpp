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

    void toSnbt(std::ostream &out, SnbtOptions const &) const override {
        out << (char const *)Quote(fValue).c_str();
    }

    static std::u8string Quote(std::u8string const &s) {
        if (auto v = String::ValidateUtf8(s, false); v) {
            std::basic_ostringstream<char8_t> ss;
            ss << std::quoted(s);
            auto ret = ss.str();
            return ret;
        } else {
            std::ostringstream ss;
            for (char8_t ch : s) {
                ss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)ch;
            }
            auto ret = "\"" + ss.str() + "\"";
            return std::u8string((char8_t const *)ret.c_str(), ret.size());
        }
    }

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        std::u8string tmp;
        if (!r.read(tmp)) {
            return false;
        }
        fValue.swap(tmp);
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        return w.write(fValue);
    }

public:
    std::u8string fValue;
};

} // namespace mcfile::nbt
