#pragma once

namespace mcfile {
namespace detail {

class ChunkSection_1_13 : public ChunkSection {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = paletteIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return nullptr;
        }
        return fPalette[index];
    }

    uint8_t blockLightAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return 0;
        }
        int const bitIndex = index * 4;
        int const byteIndex = bitIndex / 8;
        if (fBlockLight.size() <= byteIndex) {
            return 0;
        }
        int const bitOffset = bitIndex - 8 * byteIndex;
        uint8_t const v = fBlockLight[byteIndex];
        return (v >> bitOffset) & 0xF;
    }

    uint8_t skyLightAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return 0;
        }
        int const bitIndex = index * 4;
        int const byteIndex = bitIndex / 8;
        if (fSkyLight.size() <= byteIndex) {
            return 0;
        }
        int const bitOffset = bitIndex - 8 * byteIndex;
        uint8_t const v = fSkyLight[byteIndex];
        return (v >> bitOffset) & 0xF;
    }

    blocks::BlockId blockIdAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = paletteIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return blocks::unknown;
        }
        return fBlockIdPalette[index];
    }

    bool blockIdWithYOffset(int offsetY, std::function<bool(int offsetX, int offsetZ, blocks::BlockId blockId)> callback) const override {
        if (offsetY < 0 || 16 <= offsetY) {
            return false;
        }
        return eachPaletteIndexWithY(offsetY, [this, callback](int offsetX, int offsetZ, int index) {
            if (index < 0 || fBlockIdPalette.size() <= index) {
                if (!callback(offsetX, offsetZ, blocks::unknown)) {
                    return false;
                }
            }
            return callback(offsetX, offsetZ, fBlockIdPalette[index]);
        });
    }

    int y() const override {
        return fY;
    }

    std::vector<std::shared_ptr<Block const>> const& palette() const override {
        return fPalette;
    }

    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        int const index = paletteIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return std::nullopt;
        }
        return index;
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const* section) {
        if (!section) {
            return nullptr;
        }
        auto yTag = section->query("Y")->asByte();
        if (!yTag) {
            return nullptr;
        }

        auto paletteTag = section->query("Palette")->asList();
        if (!paletteTag) {
            return nullptr;
        }
        std::vector<std::shared_ptr<Block const>> palette;
        for (auto entry : *paletteTag) {
            auto tag = entry->asCompound();
            if (!tag) {
                return nullptr;
            }
            auto nameTag = tag->stringTag("Name");
            if (!nameTag) {
                return nullptr;
            }
            std::map<std::string, std::string> properties;
            auto propertiesTag = tag->compoundTag("Properties");
            if (propertiesTag) {
                for (auto p : *propertiesTag) {
                    std::string n = p.first;
                    if (n.empty()) {
                        continue;
                    }
                    auto v = p.second->asString();
                    if (!v) {
                        continue;
                    }
                    properties.insert(std::make_pair(n, v->fValue));
                }
            }
            palette.push_back(std::make_shared<Block const>(nameTag->fValue, properties));
        }

        std::vector<blocks::BlockId> blockIdPalette(palette.size());
        for (int i = 0; i < palette.size(); i++) {
            auto block = palette[i];
            blockIdPalette[i] = blocks::FromName(block->fName);
        }

        auto blockStatesTag = section->query("BlockStates")->asLongArray();
        if (!blockStatesTag) {
            return nullptr;
        }

        std::vector<uint8_t> blockLight;
        auto blockLightTag = section->query("BlockLight")->asByteArray();
        if (blockLightTag) {
            blockLight = blockLightTag->value();
        }

        std::vector<uint8_t> skyLight;
        auto skyLightTag = section->query("SkyLight")->asByteArray();
        if (skyLightTag) {
            skyLight = skyLightTag->value();
        }

        return std::shared_ptr<ChunkSection>(new ChunkSection_1_13((int)yTag->fValue,
                                                                   palette,
                                                                   blockIdPalette,
                                                                   blockStatesTag->value(),
                                                                   blockLight,
                                                                   skyLight));
    }

private:
    ChunkSection_1_13(int y, std::vector<std::shared_ptr<Block const>> const& palette, std::vector<blocks::BlockId> const& blockIdPalette, std::vector<int64_t> const& blockStates, std::vector<uint8_t> const& blockLight, std::vector<uint8_t> const& skyLight)
        : fY(y)
        , fPalette(palette)
        , fBlockIdPalette(blockIdPalette)
        , fBlockStates(blockStates)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight)
    {
    }

    int paletteIndex(int offsetX, int offsetY, int offsetZ) const {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        size_t const numBits = fBlockStates.size() * 64;
        if (numBits % 4096 != 0) {
            return -1;
        }
        size_t const bitsPerIndex = numBits >> 12;
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        size_t const index = (size_t)offsetY * 16 * 16 + (size_t)offsetZ * 16 + (size_t)offsetX;
        size_t const bitIndex = index * bitsPerIndex;
        size_t const uint64Index = bitIndex >> 6;

        if (fBlockStates.size() <= uint64Index) {
            return -1;
        }
        uint64_t const v0 = *(uint64_t *)(fBlockStates.data() + uint64Index);
        uint64_t const v1 = (uint64Index + 1) < fBlockStates.size() ? *(uint64_t *)(fBlockStates.data() + uint64Index + 1) : 0;

        int const v0Offset = (int)(bitIndex - uint64Index * 64);
        int const v0Bits = (int)std::min((uint64Index + 1) * 64 - bitIndex, bitsPerIndex);

        uint64_t const paletteIndex = ((v0 >> v0Offset) & mask) | ((v1 << v0Bits) & mask);

        /*
                   uint64Index                         uint64Index + 1
        |-----------------------------------||-----------------------------------|
        |63                                0||63                                0|
        |-------------------^====^----------||-----------------------------------|
                                 |   <--   0
                                 v0Offset


                   uint64Index                         uint64Index + 1
        |-----------------------------------||-----------------------------------|
        |63                                0||63                                0|
        |===^-------------------------------||---------------------------------^=|
            |    <--        <--       <--   0                                    0
            v0Offset
        */

        if (fPalette.size() <= paletteIndex) {
            return -1;
        }

        return (int)paletteIndex;
    }

    bool eachPaletteIndexWithY(int offsetY, std::function<bool(int offsetX, int offsetZ, int paletteIndex)> callback) const {
        size_t const numBits = fBlockStates.size() * 64;
        if (numBits % 4096 != 0) {
            return false;
        }
        size_t const bitsPerIndex = numBits >> 12;
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        size_t index = (size_t)offsetY * 16 * 16;
        size_t bitIndex = index * index * bitsPerIndex;
        size_t uint64Index = bitIndex / 64;
        uint64_t v0 = uint64Index < fBlockStates.size() ? *(uint64_t *)(fBlockStates.data() + uint64Index) : 0;
        uint64_t v1 = (uint64Index + 1) < fBlockStates.size() ? *(uint64_t *)(fBlockStates.data() + uint64Index + 1) : 0;
        size_t v0Remaining = 64;
        size_t v1Remaining = 64;

        for (int offsetZ = 0; offsetZ < 16; offsetZ++) {
            for (int offsetX = 0; offsetX < 16; offsetX++) {
                assert(v0Remaining > 0 && v1Remaining > 0);
                uint64_t paletteIndex = v0 & mask;
                int const v0Bits = (int)std::min(v0Remaining, bitsPerIndex);
                int const v1Bits = (int)(bitsPerIndex - v0Bits);
                v0Remaining -= v0Bits;
                if (v1Bits == 0) {
                    v0 >>= v0Bits;
                } else {
                    assert(v0Remaining == 0);
                    paletteIndex |= (v1 << v0Bits) & mask;
                    v1 >>= v1Bits;
                    v1Remaining -= v1Bits;
                    v0 = v1;
                    v0Remaining = v1Remaining;
                    uint64Index++;
                    v1 = (uint64Index + 1) < fBlockStates.size() ? *(uint64_t *)(fBlockStates.data() + uint64Index + 1) : 0;
                    v1Remaining = 64;
                }
                if (!callback(offsetX, offsetZ, paletteIndex)) {
                    return false;
                }
                index++;
            }
        }

        return true;
    }

    static int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return offsetY * 16 * 16 + offsetZ * 16 + offsetX;
    }

public:
    int const fY;
    std::vector<std::shared_ptr<Block const>> const fPalette;
    std::vector<blocks::BlockId> const fBlockIdPalette;
    std::vector<int64_t> const fBlockStates;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
};

} // namespace detail
} // namespace mcfile
