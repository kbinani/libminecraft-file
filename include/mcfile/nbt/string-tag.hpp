#pragma once

namespace mcfile {
namespace nbt {

class StringTag : public Tag {
public:
    StringTag()
        : Tag() {}
    explicit StringTag(std::string v)
        : fValue(v) {
    }

    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        std::string tmp;
        if (!r.read(tmp)) {
            return false;
        }
        fValue = tmp;
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        return w.write(fValue);
    }

    Tag::Type type() const override { return Tag::Type::String; }

    std::shared_ptr<Tag> clone() const override {
        return std::make_shared<StringTag>(fValue);
    }

public:
    std::string fValue;
};

} // namespace nbt
} // namespace mcfile
