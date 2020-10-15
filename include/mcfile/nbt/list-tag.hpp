#pragma once

namespace mcfile {
namespace nbt {

class ListTag : public Tag {
public:
    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        uint8_t type;
        if (!r.read(&type)) {
            return false;
        }
        int32_t size;
        if (!r.read(&size)) {
            return false;
        }
        std::vector<std::shared_ptr<Tag>> tmp;
        for (int32_t i = 0; i < size; i++) {
            auto tag = TagFactory::makeTag(type, "");
            tag->read(r);
            if (!tag->valid()) {
                return false;
            }
            tmp.push_back(tag);
        }
        tmp.swap(fValue);
        return true;
    }

    uint8_t id() const override { return Tag::TAG_List; }

public:
    std::vector<std::shared_ptr<Tag>> fValue;
};

} // namespace nbt
} // namespace mcfile
