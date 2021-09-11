#pragma once

namespace mcfile {
namespace nbt {

class ListTag : public Tag {
    friend class TagFactory;

private:
    ListTag()
        : fType(Tag::Type::End) {}

public:
    explicit ListTag(Tag::Type type)
        : fType(type) {}

    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        uint8_t type;
        if (!r.read(&type)) {
            return false;
        }
        if (type < static_cast<uint8_t>(TypeLimits::Min) || static_cast<uint8_t>(TypeLimits::Max) < type) {
            return false;
        }
        int32_t size;
        if (!r.read(&size)) {
            return false;
        }
        std::vector<std::shared_ptr<Tag>> tmp;
        for (int32_t i = 0; i < size; i++) {
            auto tag = TagFactory::makeTag(type, "");
            if (!tag->read(r)) {
                return false;
            }
            tmp.push_back(tag);
        }
        fType = static_cast<Tag::Type>(type);
        tmp.swap(fValue);
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        if (!w.write(static_cast<uint8_t>(fType))) {
            return false;
        }
        if (!w.write((int32_t)fValue.size())) {
            return false;
        }
        for (auto const &v : fValue) {
            if (!v->write(w)) {
                return false;
            }
        }
        return true;
    }

    Tag::Type type() const override { return Tag::Type::List; }

    decltype(auto) begin() { return fValue.begin(); }
    decltype(auto) end() { return fValue.end(); }

    decltype(auto) begin() const { return fValue.begin(); }
    decltype(auto) end() const { return fValue.end(); }

    void push_back(std::shared_ptr<Tag> const &item) {
        if (!item) {
            return;
        }
        Tag::Type id = item->type();
        if (fValue.empty() && fType == Tag::Type::End) {
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
    std::shared_ptr<Tag> &at(size_t idx) { return fValue[idx]; }

    std::shared_ptr<Tag> clone() const override {
        auto ret = std::make_shared<ListTag>(fType);
        for (auto const &value : fValue) {
            ret->fValue.push_back(value->clone());
        }
        return ret;
    }

public:
    Tag::Type fType;
    std::vector<std::shared_ptr<Tag>> fValue;
};

} // namespace nbt
} // namespace mcfile
