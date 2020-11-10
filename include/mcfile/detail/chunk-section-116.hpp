#pragma once

namespace mcfile {
namespace detail {

class ChunkSection_1_16 : public ChunkSection {
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
            auto nameTag = tag->query("Name")->asString();
            if (!nameTag) {
                return nullptr;
            }
            std::map<std::string, std::string> properties;
            auto propertiesTag = tag->query("Properties")->asCompound();
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

        return std::shared_ptr<ChunkSection>(new ChunkSection_1_16((int)yTag->fValue,
                                                                   palette,
                                                                   blockIdPalette,
                                                                   blockStatesTag->value(),
                                                                   blockLight,
                                                                   skyLight));
    }

private:
    ChunkSection_1_16(int y, std::vector<std::shared_ptr<Block const>> const& palette, std::vector<blocks::BlockId> const& blockIdPalette, std::vector<int64_t> const& blockStates, std::vector<uint8_t> const& blockLight, std::vector<uint8_t> const& skyLight)
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
        size_t const bitsPerIndex = std::floor((double)numBits / 4096.0);
        size_t const palettesPerLong = std::floor(64.0 / (double)bitsPerIndex);
        
        size_t const index = (size_t)offsetY * 16 * 16 + (size_t)offsetZ * 16 + (size_t)offsetX;
        size_t const uint64Index = index / palettesPerLong;
        
        if (fBlockStates.size() <= uint64Index) {
            return -1;
        }

        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        uint64_t const v = *(uint64_t *)(fBlockStates.data() + uint64Index);
        int const offset = (index % palettesPerLong) * bitsPerIndex;
        
        uint64_t const paletteIndex = (v >> offset) & mask;
        
        if (fPalette.size() <= paletteIndex) {
            return -1;
        }
        
        return (int)paletteIndex;
    }

    bool eachPaletteIndexWithY(int offsetY, std::function<bool(int offsetX, int offsetZ, int paletteIndex)> callback) const {
        if (offsetY < 0 || 16 <= offsetY) {
            return false;
        }

        size_t const numBits = fBlockStates.size() * 64;
        size_t const bitsPerIndex = std::floor((double)numBits / 4096.0);
        size_t const palettesPerLong = std::floor(64.0 / (double)bitsPerIndex);
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        size_t index = 0;
        size_t uint64Index = 0;
        uint64_t v = *(uint64_t *)fBlockStates.data();

        for (int offsetZ = 0; offsetZ < 16; offsetZ++) {
            for (int offsetX = 0; offsetX < 16; offsetX++) {
                uint64_t const paletteIndex = v & mask;
                if (!callback(offsetX, offsetZ, paletteIndex)) {
                    goto last;
                }
                
                index++;
                v = v >> bitsPerIndex;

                if (index % palettesPerLong == 0) {
                    uint64Index++;
                    if (fBlockStates.size() <= uint64Index) {
                        goto last;
                    }
                    v = *(uint64_t *)(fBlockStates.data() + uint64Index);
                }
            }
        }
    last:
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
