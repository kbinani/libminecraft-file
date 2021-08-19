#pragma once

namespace mcfile {
namespace nbt {

class ListTag : public Tag {
    friend class TagFactory;

private:
    ListTag() : fType(Tag::TAG_End) {}

public:
    explicit ListTag(uint8_t type) : fType(type) {}

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

    decltype(auto) begin() { return fValue.begin(); }
    decltype(auto) end() { return fValue.end(); }

    decltype(auto) begin() const { return fValue.begin(); }
    decltype(auto) end() const { return fValue.end(); }

    void push_back(std::shared_ptr<Tag> const& item) {
        if (!item) {
            return;
        }
        uint8_t id = item->id();
        if (fValue.empty() && fType == Tag::TAG_End) {
            fType = id;
        }
        assert(fType == id);
        if (fType != id) {
            return;
        }
        fValue.push_back(item);
    }

    size_t size() const { return fValue.size(); }
    bool empty() const { return fValue.empty(); }
    std::shared_ptr<Tag> at(size_t idx) const { return fValue[idx]; }
    std::shared_ptr<Tag>& at(size_t idx) { return fValue[idx]; }

    std::shared_ptr<Tag> clone() const override {
        auto ret = std::make_shared<ListTag>(fType);
        for (auto const& value : fValue) {
            ret->fValue.push_back(value->clone());
        }
        return ret;
    }

public:
    uint8_t fType;
    std::vector<std::shared_ptr<Tag>> fValue;
};

} // namespace nbt
} // namespace mcfile
