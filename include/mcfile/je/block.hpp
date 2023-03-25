#pragma once

namespace mcfile::je {

class Block {
public:
    explicit Block(std::u8string const &name, std::u8string const &data = u8"")
        : Block(blocks::FromName(name), name, data) {
    }

    Block(std::u8string const &name, std::map<std::u8string, std::u8string> const &props)
        : Block(blocks::FromName(name), name, PackProperties(props)) {
    }

    explicit Block(blocks::BlockId id, std::u8string const &data = u8"")
        : Block(id, blocks::Name(id), data) {
    }

    Block(blocks::BlockId id, std::map<std::u8string, std::u8string> const &props)
        : Block(id, blocks::Name(id), PackProperties(props)) {
    }

    static std::shared_ptr<Block const> FromBlockData(std::u8string const &blockData, int dataVersion) {
        using namespace std;
        size_t bra = blockData.find(u8'[');
        u8string name = blockData;
        u8string data;
        if (bra != u8string::npos) {
            name = blockData.substr(0, bra);
            data = blockData.substr(bra);
        }
        auto id = blocks::FromNameWithMigration(name, dataVersion);
        if (id == blocks::unknown) {
            return nullptr;
        }
        return shared_ptr<Block const>(new Block(id, name, data));
    }

    bool equals(Block const &other) const {
        return other.fBlockData == fBlockData;
    }

    std::u8string const &toString() const {
        return fBlockData;
    }

    std::u8string_view property(std::u8string const &name, char8_t const *fallback = u8"") const {
        using namespace std;
        if (fData.empty()) {
            return fallback ? fallback : u8"";
        }
        assert(fData.front() == u8'[' && fData.back() == u8']');
        size_t start = fData.find(u8"[" + name + u8"=");
        if (start == u8string::npos) {
            start = fData.find(u8"," + name + u8"=");
        }
        if (start == u8string::npos) {
            return fallback ? fallback : u8"";
        }
        size_t from = start + 1 + name.size() + 1;
        size_t ket = fData.size() - 1;
        size_t to = ket;
        size_t comma = fData.find(u8',', start + 1 + name.size() + 1);
        if (comma != u8string::npos) {
            to = comma;
        }
        return fData.substr(from, to - from);
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto root = make_shared<nbt::CompoundTag>();
        root->set(u8"Name", make_shared<nbt::StringTag>(name()));
        if (!fData.empty()) {
            assert(fData.front() == u8'[' && fData.back() == u8']');
            auto properties = make_shared<nbt::CompoundTag>();
            size_t off = 1;
            while (off < fData.size()) {
                size_t eq = fData.find(u8'=', off);
                if (eq == u8string::npos) {
                    break;
                }
                auto key = fData.substr(off, eq - off);
                size_t to = fData.find(',', eq + 1);
                if (to == u8string::npos) {
                    to = fData.size() - 1;
                }
                auto value = fData.substr(eq + 1, to - (eq + 1));
                properties->set(u8string(key.data(), key.size()), make_shared<nbt::StringTag>(u8string(value)));
                off = to + 1;
            }
            root->set(u8"Properties", properties);
        }
        return root;
    }

    std::shared_ptr<Block const> applying(std::initializer_list<std::pair<std::u8string const, std::optional<std::u8string>>> properties) const {
        return applying(std::map<std::u8string, std::optional<std::u8string>>(properties));
    }

    std::shared_ptr<Block const> applying(std::map<std::u8string, std::optional<std::u8string>> const &properties) const {
        using namespace std;
        map<u8string_view, u8string_view> props;
        ExtractProperties(fData, props);
        for (auto const &it : properties) {
            if (it.second) {
                props[it.first] = *it.second;
            } else {
                props.erase(it.first);
            }
        }
        return shared_ptr<Block const>(new Block(fId, u8string(fName.data(), fName.size()), PackProperties(props)));
    }

    std::shared_ptr<Block const> renamed(std::u8string_view const &name) const {
        auto id = blocks::FromName(name);
        return std::shared_ptr<Block const>(new Block(id, std::u8string(name), std::u8string(fData)));
    }

    std::shared_ptr<Block const> copy() const {
        return std::shared_ptr<Block const>(new Block(fId, std::u8string(fName), std::u8string(fData)));
    }

    static std::shared_ptr<Block const> FromCompoundTag(nbt::CompoundTag const &tag) {
        using namespace std;
        auto name = tag.string(u8"Name");
        if (!name) {
            return nullptr;
        }
        auto id = blocks::FromName(*name);
        auto properties = tag.compoundTag(u8"Properties");
        u8string data;
        if (properties) {
            for (auto p : properties->fValue) {
                u8string key = p.first;
                nbt::StringTag const *s = p.second->asString();
                if (s == nullptr) {
                    continue;
                }
                if (data.empty()) {
                    data += u8"[";
                } else {
                    data += u8",";
                }
                data += key + u8"=" + s->fValue;
            }
            if (!data.empty()) {
                data += u8"]";
            }
        }
        return std::shared_ptr<Block const>(new Block(id, *name, data));
    }

    std::u8string name() const {
        return std::u8string(fName);
    }

private:
    Block(blocks::BlockId id, std::u8string const &name, std::u8string const &data)
        : fId(id)
        , fBlockData(name + data)
        , fName(fBlockData.data(), name.size())
        , fData(fBlockData.data() + name.size(), fBlockData.size() - name.size()) {
    }

    static std::u8string ToString(std::u8string const &name, std::map<std::u8string, std::u8string> const &props) {
        using namespace std;
        if (props.empty()) {
            return name;
        }
        return name + PackProperties(props);
    }

    template<class String>
    static std::u8string PackProperties(std::map<String, String> const &props) {
        std::u8string data;
        for (auto const &it : props) {
            if (data.empty()) {
                data += u8"[";
            } else {
                data += u8",";
            }
            data.append(it.first);
            data.append(u8"=");
            data.append(it.second);
        }
        if (!data.empty()) {
            data += u8"]";
        }
        return data;
    }

    static void ExtractProperties(std::u8string_view const &data, std::map<std::u8string_view, std::u8string_view> &props) {
        using namespace std;
        if (data.empty()) {
            return;
        }
        assert(data.front() == u8'[' && data.back() == u8']');
        size_t off = 1;
        while (off < data.size()) {
            size_t eq = data.find(u8'=', off);
            if (eq == u8string::npos) {
                break;
            }
            size_t to = data.size() - 1;
            size_t comma = data.find(u8',', eq + 1);
            if (comma != u8string::npos) {
                to = comma;
            }
            auto key = data.substr(off, eq - off);
            auto value = data.substr(eq + 1, to - eq - 1);
            props[key] = value;
            off = to + 1;
        }
    }

public:
    blocks::BlockId const fId;
    std::u8string const fBlockData; // "minecraft:grass_block[snowy=true]"
    std::u8string_view const fName; // "minecraft:grass_block"
    std::u8string_view const fData; // "[snowy=true]"
};

} // namespace mcfile::je
