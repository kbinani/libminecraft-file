#pragma once

namespace mcfile {

class Block {
public:
    explicit Block(std::string const& name, std::map<std::string, std::string> const& properties = std::map<std::string, std::string>())
        : fName(name)
        , fProperties(properties)
    {
    }

    explicit Block(blocks::BlockId id, std::map<std::string, std::string> const& properties = std::map<std::string, std::string>())
        : fName(blocks::Name(id))
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
        std::ostringstream ss;
        ss << fName;
        if (!fProperties.empty()) {
            ss << "[";
            auto it = fProperties.begin();
            while (true) {
                ss << it->first << "=" << it->second;
                it++;
                if (it == fProperties.end()) {
                    break;
                } else {
                    ss << ",";
                }
            }
            ss << "]";
        }
        return ss.str();
    }
    
    std::string property(std::string const& name, std::string const& fallback = "") const {
        auto value = fProperties.find(name);
        if (value == fProperties.end()) {
            return fallback;
        }
        return value->second;
    }
    
public:
    std::string const fName;
    std::map<std::string, std::string> const fProperties;
};

} // namespace mcfile
