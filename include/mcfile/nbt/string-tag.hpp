#pragma once

namespace mcfile {
namespace nbt {

class StringTag : public Tag {
public:
    StringTag() : Tag() {}
    explicit StringTag(std::string v) : fValue(v) {
        fValid = true;
    }

    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        std::string tmp;
        if (!r.read(tmp)) {
            return false;
        }
        fValue = tmp;
        return true;
    }

    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {
        w.write(fValue);
    }
    
    uint8_t id() const override { return Tag::TAG_String; }

    std::shared_ptr<Tag> clone() const override {
        return std::make_shared<StringTag>(fValue);
    }
    
public:
    std::string fValue;
};

} // namespace nbt
} // namespace mcfile
