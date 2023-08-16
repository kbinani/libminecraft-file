#pragma once

namespace mcfile::be {

class Block {
public:
    Block(std::u8string const &name, std::shared_ptr<nbt::CompoundTag> const &states, std::optional<int32_t> version, std::optional<int16_t> val = std::nullopt)
        : fName(name)
        , fVersion(version)
        , fVal(val) {
        if (states) {
            fStates = states;
        } else {
            fStates = std::make_shared<nbt::CompoundTag>();
        }
    }

    static std::shared_ptr<Block> FromCompound(nbt::CompoundTag const &tag) {
        auto name = tag.string(u8"name");
        auto states = tag.compoundTag(u8"states");
        auto version = tag.int32(u8"version");
        if (!name) {
            return nullptr;
        }
        auto val = tag.int16(u8"val");
        return std::make_shared<Block>(*name, states, version, val);
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        auto tag = make_shared<nbt::CompoundTag>();
        tag->set(u8"name", make_shared<nbt::StringTag>(fName));
        if (fVal && !fVersion) {
            tag->set(u8"val", make_shared<nbt::ShortTag>(*fVal));
        } else {
            tag->set(u8"states", fStates->clone());
            tag->set(u8"version", make_shared<nbt::IntTag>(*fVersion));
        }
        return tag;
    }

public:
    std::u8string fName;
    std::shared_ptr<nbt::CompoundTag> fStates;
    std::optional<int32_t> fVersion;
    std::optional<int16_t> fVal;
};

} // namespace mcfile::be
