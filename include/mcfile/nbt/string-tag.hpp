#pragma once

namespace mcfile {
namespace nbt {

class StringTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        std::string tmp;
        if (!r.read(tmp)) {
            return false;
        }
        fValue = tmp;
        return true;
    }

    uint8_t id() const override { return Tag::TAG_String; }

public:
    std::string fValue;
};

} // namespace nbt
} // namespace mcfile
