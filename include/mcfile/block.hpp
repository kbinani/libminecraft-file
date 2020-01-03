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

public:
    std::string const fName;
    std::map<std::string, std::string> const fProperties;
};

} // namespace mcfile
