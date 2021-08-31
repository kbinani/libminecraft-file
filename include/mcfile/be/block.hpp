#pragma once

namespace mcfile::be {

class Block {
public:
    Block(std::string const& name, std::shared_ptr<nbt::CompoundTag> const& states, int32_t version)
        : fName(name)
        , fVersion(version)
    {
        if (states) {
            fStates.swap(states->fValue);
        }
    }

public:
    std::string const fName;
    std::map<std::string, std::shared_ptr<nbt::Tag>> fStates;
    int32_t const fVersion;
};

}
