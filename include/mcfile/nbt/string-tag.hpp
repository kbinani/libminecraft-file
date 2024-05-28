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
        out << '"' << (char const *)Escape(fValue).c_str() << '"';
    }

    static std::u8string Escape(std::u8string const &s) {
        static std::regex sReg("\"");
        auto ret = std::regex_replace((char const *)s.c_str(), sReg, "\\\"");
        std::u8string r;
        r.assign((char8_t const *)ret.c_str(), ret.size());
        return r;
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
