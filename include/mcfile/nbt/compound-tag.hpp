#pragma once

namespace mcfile {
namespace nbt {

class CompoundTag : public Tag {
public:
    bool readImpl(::mcfile::stream::InputStreamReader& r) override {
        std::map<std::string, std::shared_ptr<Tag>> tmp;
        while (r.pos() < r.length()) {
            uint8_t type;
            if (!r.read(&type)) {
                break;
            }
            if (type == Tag::TAG_End) {
                break;
            }

            std::string name;
            if (!r.read(name)) {
                return false;
            }

            auto tag = TagFactory::makeTag(type, name);
            tag->read(r);
            if (!tag->valid()) {
                return false;
            }
            tmp.insert(std::make_pair(name, tag));
        }
        fValue.swap(tmp);
        return true;
    }
    
    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {
        for (auto it = fValue.begin(); it != fValue.end(); it++) {
            uint8_t type = it->second->id();
            w.write(type);
            w.write(it->first);
            it->second->write(w);
        }
        w.write((uint8_t)0);
    }

    uint8_t id() const override { return Tag::TAG_Compound; }

    Tag const* query(std::string const& path) const {
        // path: /Level/Sections
        if (path.empty()) {
            return EndTag::instance();
        }
        std::string p = path;
        Tag const* pivot = this;
        while (!p.empty()) {
            if (p[0] == '/') {
                if (fValue.size() != 1) {
                    return EndTag::instance();
                }
                auto child = fValue.begin()->second;
                if (p.size() == 1) {
                    return child.get();
                }
                if (child->id() != Tag::TAG_Compound) {
                    return EndTag::instance();
                }
                pivot = child->asCompound();
                p = p.substr(1);
            } else {
                auto pos = p.find_first_of('/');
                std::string name;
                if (pos == std::string::npos) {
                    name = p;
                } else {
                    name = p.substr(0, pos);
                }
                if (pivot->id() == Tag::TAG_List) {
                    auto list = pivot->asList();
                    if (!list) return EndTag::instance();
                    int index;
                    try {
                        index = std::stoi(name);
                        if (index < 0 || list->fValue.size() <= index) {
                            return EndTag::instance();
                        }
                        auto child = list->fValue[index];
                        if (pos == std::string::npos) {
                            return child.get();
                        }
                        auto id = child->id();
                        if (id != Tag::TAG_Compound && id != Tag::TAG_List) {
                            return EndTag::instance();
                        }
                        pivot = child.get();
                        p = p.substr(pos + 1);
                    } catch (...) {
                        return EndTag::instance();
                    }
                } else if (pivot->id() == Tag::TAG_Compound) {
                    auto comp = pivot->asCompound();
                    if (!comp) return EndTag::instance();

                    auto child = comp->fValue.find(name);
                    if (child == comp->fValue.end()) {
                        return EndTag::instance();
                    }
                    if (pos == std::string::npos) {
                        return child->second.get();
                    }
                    auto id = child->second->id();
                    if (id != Tag::TAG_Compound && id != Tag::TAG_List) {
                        return EndTag::instance();
                    }
                    pivot = child->second.get();
                    p = p.substr(pos + 1);
                }
            }
        }
        return EndTag::instance();
    }

    std::shared_ptr<Tag>& operator[](std::string const& name) {
        return fValue[name];
    }

public:
    std::map<std::string, std::shared_ptr<Tag>> fValue;
};

} // namespace nbt
} // namespace mcfile
