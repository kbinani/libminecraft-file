#pragma once

namespace mcfile::be {

class Block {
public:
    Block(std::string const &name, std::shared_ptr<nbt::CompoundTag> const &states, int32_t version)
        : fName(name)
        , fStates(states)
        , fVersion(version) {
    }

    static std::shared_ptr<Block> FromCompound(nbt::CompoundTag const &tag) {
        auto name = tag.string("name");
        auto states = tag.compoundTag("states");
        auto version = tag.int32("version");
        if (!name || !version) {
            return nullptr;
        }
        return std::make_shared<Block>(*name, states, *version);
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto tag = make_shared<nbt::CompoundTag>();
        tag->set("name", make_shared<nbt::StringTag>(fName));
        if (fStates) {
            tag->set("states", fStates->clone());
        }
        tag->set("version", make_shared<nbt::IntTag>(fVersion));
        return tag;
    }

public:
    std::string const fName;
    std::shared_ptr<nbt::CompoundTag> fStates;
    int32_t const fVersion;
};

} // namespace mcfile::be
