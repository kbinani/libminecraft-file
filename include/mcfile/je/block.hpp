#pragma once

namespace mcfile::je {

class Block {
public:
    explicit Block(std::string const &name, std::string const &data = "")
        : Block(blocks::FromName(name), name, data) {
    }

    Block(std::string const &name, std::map<std::string, std::string> const &props)
        : Block(blocks::FromName(name), name, PackProperties(props)) {
    }

    explicit Block(blocks::BlockId id, std::string const &data = "")
        : Block(id, blocks::Name(id), data) {
    }

    Block(blocks::BlockId id, std::map<std::string, std::string> const &props)
        : Block(id, blocks::Name(id), PackProperties(props)) {
    }

    static std::shared_ptr<Block const> FromBlockData(std::string const &blockData, int dataVersion) {
        using namespace std;
        size_t bra = blockData.find('[');
        string name = blockData;
        string data;
        if (bra != string::npos) {
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

    std::string const &toString() const {
        return fBlockData;
    }

    std::string_view property(std::string const &name, char const *fallback = "") const {
        using namespace std;
        if (fData.empty()) {
            return fallback ? fallback : "";
        }
        assert(fData.front() == '[' && fData.back() == ']');
        size_t start = fData.find("[" + name + "=");
        if (start == string::npos) {
            start = fData.find("," + name + "=");
        }
        if (start == string::npos) {
            return fallback ? fallback : "";
        }
        size_t from = start + 1 + name.size() + 1;
        size_t ket = fData.size() - 1;
        size_t to = ket;
        size_t comma = fData.find(',', start + 1 + name.size() + 1);
        if (comma != string::npos) {
            to = comma;
        }
        return fData.substr(from, to - from);
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto root = make_shared<nbt::CompoundTag>();
        root->set("Name", make_shared<nbt::StringTag>(name()));
        if (!fData.empty()) {
            assert(fData.front() == '[' && fData.back() == ']');
            auto properties = make_shared<nbt::CompoundTag>();
            size_t off = 1;
            while (off < fData.size()) {
                size_t eq = fData.find('=', off);
                if (eq == string::npos) {
                    break;
                }
                auto key = fData.substr(off, eq - off);
                size_t to = fData.find(',', eq + 1);
                if (to == string::npos) {
                    to = fData.size() - 1;
                }
                auto value = fData.substr(eq + 1, to - (eq + 1));
                properties->set(string(key.data(), key.size()), make_shared<nbt::StringTag>(string(value)));
                off = to + 1;
            }
            root->set("Properties", properties);
        }
        return root;
    }

    std::shared_ptr<Block const> applying(std::initializer_list<std::pair<std::string const, std::optional<std::string>>> properties) const {
        return applying(std::map<std::string, std::optional<std::string>>(properties));
    }

    std::shared_ptr<Block const> applying(std::map<std::string, std::optional<std::string>> const &properties) const {
        using namespace std;
        map<string_view, string_view> props;
        ExtractProperties(fData, props);
        for (auto const &it : properties) {
            if (it.second) {
                props[it.first] = *it.second;
            } else {
                props.erase(it.first);
            }
        }
        return shared_ptr<Block const>(new Block(fId, string(fName.data(), fName.size()), PackProperties(props)));
    }

    std::shared_ptr<Block const> renamed(std::string_view const &name) const {
        auto id = blocks::FromName(name);
        return std::shared_ptr<Block const>(new Block(id, std::string(name), std::string(fData)));
    }

    std::shared_ptr<Block const> copy() const {
        return std::shared_ptr<Block const>(new Block(fId, std::string(fName), std::string(fData)));
    }

    static std::shared_ptr<Block const> FromCompoundTag(nbt::CompoundTag const &tag) {
        using namespace std;
        auto name = tag.string("Name");
        if (!name) {
            return nullptr;
        }
        auto id = blocks::FromName(*name);
        auto properties = tag.compoundTag("Properties");
        string data;
        if (properties) {
            for (auto p : properties->fValue) {
                string key = p.first;
                nbt::StringTag const *s = p.second->asString();
                if (s == nullptr) {
                    continue;
                }
                if (data.empty()) {
                    data += "[";
                } else {
                    data += ",";
                }
                data += key + "=" + s->fValue;
            }
            if (!data.empty()) {
                data += "]";
            }
        }
        return std::shared_ptr<Block const>(new Block(id, *name, data));
    }

    std::string name() const {
        return std::string(fName);
    }

private:
    Block(blocks::BlockId id, std::string const &name, std::string const &data)
        : fId(id)
        , fBlockData(name + data)
        , fName(fBlockData.data(), name.size())
        , fData(fBlockData.data() + name.size(), fBlockData.size() - name.size()) {
    }

    static std::string ToString(std::string const &name, std::map<std::string, std::string> const &props) {
        using namespace std;
        if (props.empty()) {
            return name;
        }
        return name + PackProperties(props);
    }

    template<class String>
    static std::string PackProperties(std::map<String, String> const &props) {
        std::string data;
        for (auto const &it : props) {
            if (data.empty()) {
                data += "[";
            } else {
                data += ",";
            }
            data.append(it.first);
            data.append("=");
            data.append(it.second);
        }
        if (!data.empty()) {
            data += "]";
        }
        return data;
    }

    static void ExtractProperties(std::string_view const &data, std::map<std::string_view, std::string_view> &props) {
        using namespace std;
        if (data.empty()) {
            return;
        }
        assert(data.front() == '[' && data.back() == ']');
        size_t off = 1;
        while (off < data.size()) {
            size_t eq = data.find('=', off);
            if (eq == string::npos) {
                break;
            }
            size_t to = data.size() - 1;
            size_t comma = data.find(',', eq + 1);
            if (comma != string::npos) {
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
    std::string const fBlockData; // "minecraft:grass_block[snowy=true]"
    std::string_view const fName; // "minecraft:grass_block"
    std::string_view const fData; // "[snowy=true]"
};

} // namespace mcfile::je
