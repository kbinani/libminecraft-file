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

    static std::shared_ptr<Block> FromCompound(nbt::CompoundTag const& tag) {
        auto name = tag.string("name");
        auto states = tag.compoundTag("states");
        auto version = tag.int32("version");
        if (!name || !version) {
            return nullptr;
        }
        return std::make_shared<Block>(*name, states, *version);
    }

public:
    std::string const fName;
    std::map<std::string, std::shared_ptr<nbt::Tag>> fStates;
    int32_t const fVersion;
};

}
