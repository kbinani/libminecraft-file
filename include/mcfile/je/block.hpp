#pragma once

namespace mcfile::je {

class Block {
public:
    explicit Block(std::string const &name, std::map<std::string, std::string> const &properties = std::map<std::string, std::string>())
        : fId(blocks::FromName(name))
        , fName(name)
        , fProperties(properties)
        , fBlockData(ToString(name, properties)) {
    }

    explicit Block(blocks::BlockId id, std::map<std::string, std::string> const &properties = std::map<std::string, std::string>())
        : fId(id)
        , fName(blocks::Name(id))
        , fProperties(properties)
        , fBlockData(ToString(blocks::Name(id), properties)) {
    }

    bool equals(Block const &other) const {
        if (other.fName != fName) {
            return false;
        }
        if (other.fProperties.size() != fProperties.size()) {
            return false;
        }
        for (auto it : other.fProperties) {
            auto found = fProperties.find(it.first);
            if (found == fProperties.end()) {
                return false;
            }
            if (found->second != it.second) {
                return false;
            }
        }
        return true;
    }

    std::string toString() const {
        return fBlockData;
    }

    std::string property(std::string const &name, std::string const &fallback = "") const {
        auto value = fProperties.find(name);
        if (value == fProperties.end()) {
            return fallback;
        }
        return value->second;
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto root = make_shared<nbt::CompoundTag>();
        root->set("Name", make_shared<nbt::StringTag>(fName));
        if (!fProperties.empty()) {
            auto properties = make_shared<nbt::CompoundTag>();
            for (auto it : fProperties) {
                properties->set(it.first, make_shared<nbt::StringTag>(it.second));
            }
            root->set("Properties", properties);
        }
        return root;
    }

private:
    static std::string ToString(std::string const &name, std::map<std::string, std::string> const &props) {
        using namespace std;
        string s = name;
        if (props.empty()) {
            return s;
        }
        bool first = true;
        for (auto const &it : props) {
            if (first) {
                s += "[";
            } else {
                s += ",";
            }
            s += it.first + "=" + it.second;
            first = false;
        }
        s += "]";
        return s;
    }

public:
    blocks::BlockId const fId;
    std::string const fName;
    std::map<std::string, std::string> const fProperties;
    std::string const fBlockData;
};

} // namespace mcfile::je
