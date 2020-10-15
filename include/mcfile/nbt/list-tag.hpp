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
        fType = type;
        tmp.swap(fValue);
        return true;
    }

    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {
        w.write(fType);
        w.write((int32_t)fValue.size());
        for (auto const& v : fValue) {
            v->write(w);
        }
    }
    
    uint8_t id() const override { return Tag::TAG_List; }

public:
    uint8_t fType;
    std::vector<std::shared_ptr<Tag>> fValue;
};

} // namespace nbt
} // namespace mcfile
