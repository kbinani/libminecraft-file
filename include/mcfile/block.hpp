#pragma once

namespace mcfile {

class Block {
public:
    explicit Block(std::string const& name, std::map<std::string, std::string> const& properties = std::map<std::string, std::string>())
        : fId(blocks::FromName(name))
        , fName(name)
        , fProperties(properties)
    {
    }

    explicit Block(blocks::BlockId id, std::map<std::string, std::string> const& properties = std::map<std::string, std::string>())
        : fId(id)
        , fName(blocks::Name(id))
        , fProperties(properties)
    {
    }

    bool equals(Block const& other) const {
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
        using namespace std;
        string s = fName;
        if (fProperties.empty()) {
            return s;
        }
        bool first = true;
        for (auto const& it : fProperties) {
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
    
    std::string property(std::string const& name, std::string const& fallback = "") const {
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
    
public:
    blocks::BlockId const fId;
    std::string const fName;
    std::map<std::string, std::string> const fProperties;
};

} // namespace mcfile
