#pragma once

namespace mcfile::be {

class Block {
public:
    Block(std::u8string const &name, std::shared_ptr<nbt::CompoundTag> const &states, int32_t version)
        : fName(name)
        , fStates(states)
        , fVersion(version) {
    }

    static std::shared_ptr<Block> FromCompound(nbt::CompoundTag const &tag) {
        auto name = tag.string(u8"name");
        auto states = tag.compoundTag(u8"states");
        auto version = tag.int32(u8"version");
        if (!name || !version) {
            return nullptr;
        }
        return std::make_shared<Block>(*name, states, *version);
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto tag = make_shared<nbt::CompoundTag>();
        tag->set(u8"name", make_shared<nbt::StringTag>(fName));
        if (fStates) {
            tag->set(u8"states", fStates->clone());
        }
        tag->set(u8"version", make_shared<nbt::IntTag>(fVersion));
        return tag;
    }

public:
    std::u8string const fName;
    std::shared_ptr<nbt::CompoundTag> fStates;
    int32_t const fVersion;
};

} // namespace mcfile::be
