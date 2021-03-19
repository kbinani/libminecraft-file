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

    bool setBlockAt(int offsetX, int offsetY, int offsetZ, std::shared_ptr<Block const> const& block) override {
        using namespace std;
        if (!block) {
            return false;
        }
        int index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return false;
        }
        int idx = -1;
        string s = block->toString();
        for (int i = 0; i < fPalette.size(); i++) {
            if (s == fPalette[i]->toString()) {
                idx = i;
                break;
            }
        }
        if (idx < 0) {
            idx = fPalette.size();
            fPalette.push_back(block);
            fBlockIdPalette.push_back(blocks::FromName(block->fName));
        }
        fPaletteIndices[index] = idx;
        return true;
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

    int rawY() const override {
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

    std::shared_ptr<mcfile::nbt::CompoundTag> toCompoundTag() const override {
        using namespace std;
        using namespace mcfile::nbt;
        auto root = make_shared<CompoundTag>();
        
        root->set("Y", make_shared<ByteTag>(fY));
        
        if (!fBlockLight.empty()) {
            vector<uint8_t> buf;
            copy(fBlockLight.begin(), fBlockLight.end(), back_inserter(buf));
            root->set("BlockLight", make_shared<ByteArrayTag>(buf));
        }
        
        if (!fSkyLight.empty()) {
            vector<uint8_t> buf;
            copy(fSkyLight.begin(), fSkyLight.end(), back_inserter(buf));
            root->set("SkyLight", make_shared<ByteArrayTag>(buf));
        }
        
        if (!fPaletteIndices.empty()) {
            vector<int64_t> blockStates;
            BlockStatesFromPaletteIndices(fPalette.size(), fPaletteIndices, blockStates);
            root->set("BlockStates", make_shared<LongArrayTag>(blockStates));
        }
    
        if (!fPalette.empty()) {
            auto palette = make_shared<ListTag>();
            palette->fType = Tag::TAG_Compound;
            for (auto p : fPalette) {
                palette->push_back(p->toCompoundTag());
            }
            root->set("Palette", palette);
        }
    
        return root;
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
        std::vector<uint16_t> paletteIndices;
        PaletteIndicesFromBlockStates(blockStatesTag->value(), paletteIndices);
        
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
                                                                   paletteIndices,
                                                                   blockLight,
                                                                   skyLight));
    }

private:
    ChunkSection_1_13(int y,
                      std::vector<std::shared_ptr<Block const>> const& palette,
                      std::vector<blocks::BlockId> const& blockIdPalette,
                      std::vector<uint16_t> const& paletteIndices,
                      std::vector<uint8_t> const& blockLight,
                      std::vector<uint8_t> const& skyLight)
        : fY(y)
        , fPalette(palette)
        , fBlockIdPalette(blockIdPalette)
        , fPaletteIndices(paletteIndices)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight)
    {}

    int paletteIndex(int offsetX, int offsetY, int offsetZ) const {
        int index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0 || fPaletteIndices.size() <= index) {
            return -1;
        }
        int i = fPaletteIndices[index];
        if (i < 0 || fPalette.size() <= i) {
            return -1;
        }
        return i;
    }

    bool eachPaletteIndexWithY(int offsetY, std::function<bool(int offsetX, int offsetZ, int paletteIndex)> callback) const {
        if (offsetY < 0 || 16 <= offsetY) {
            return false;
        }
        int index = BlockIndex(offsetY, 0, 0);
        for (int offsetZ = 0; offsetZ < 16; offsetZ++) {
            for (int offsetX = 0; offsetX < 16; offsetX++) {
                if (fPaletteIndices.size() <= index) {
                    return false;
                }
                int paletteIndex = fPaletteIndices[index];
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

    static void PaletteIndicesFromBlockStates(std::vector<int64_t> const& blockStates, std::vector<uint16_t> &paletteIndices) {
        paletteIndices.resize(16 * 16 * 16);
        size_t const numBits = blockStates.size() * 64;
        if (numBits % 4096 != 0) {
            return;
        }
        size_t const bitsPerIndex = numBits >> 12;
        if (64 % bitsPerIndex != 0) {
            int a =0 ;
        }
        size_t u64Index = 0;
        size_t bitIndex = 0;
        for (int i = 0; i < 4096; i++) {
            size_t bitIndex = bitsPerIndex * i;
            size_t u64Index = bitIndex / 64;
            size_t v0Bits = std::min(bitsPerIndex, (u64Index + 1) * 64 - bitIndex);
            size_t v0BitOffset = bitIndex - 64 * u64Index;
            size_t v1Bits = bitsPerIndex - v0Bits;
            uint64_t v0 = *(uint64_t*)(blockStates.data() + u64Index);
            uint64_t paletteIndex = (v0 >> v0BitOffset) & ((1 << v0Bits) - 1);
            if (v1Bits > 0) {
                uint64_t v1 = *(uint64_t*)(blockStates.data() + (u64Index + 1));
                uint64_t t = v1 & ((1 << v1Bits) - 1);
                paletteIndex = (t << v0Bits) | paletteIndex;
            }
            paletteIndices[i] = (uint16_t)(paletteIndex & 0xffff);
        }
    }

    static void BlockStatesFromPaletteIndices(size_t numPaletteEntries, std::vector<uint16_t> const& paletteIndices, std::vector<int64_t> &blockStates) {
        using namespace std;
        blockStates.clear();

        uint8_t bitsPerBlock;
        int blocksPerLong;
        if (numPaletteEntries <= 16) {
            bitsPerBlock = 4;
            blocksPerLong = 16;
        } else if (numPaletteEntries <= 32) {
            bitsPerBlock = 5;
            blocksPerLong = 12;
        } else if (numPaletteEntries <= 64) {
            bitsPerBlock = 6;
            blocksPerLong = 10;
        } else if (numPaletteEntries <= 128) {
            bitsPerBlock = 7;
            blocksPerLong = 9;
        } else if (numPaletteEntries <= 256) {
            bitsPerBlock = 8;
            blocksPerLong = 8;
        } else {
            bitsPerBlock = 16;
            blocksPerLong = 4;
        }
        vector<bool> bitset;
        bitset.reserve(bitsPerBlock * 4096);
        assert(paletteIndices.size() == 4096);
        for (int i = 0; i < 4096; i++) {
            uint16_t v = paletteIndices[i];
            for (int j = 0; j < bitsPerBlock; j++) {
                bitset.push_back((v & 1) == 1);
                v >>= 1;
            }
        }
        for (int i = 0; i < bitset.size(); i += 64) {
            uint64_t v = 0;
            for (int j = i; j < i + 64 && j < bitset.size(); j++) {
                uint64_t t = ((uint64_t)(bitset[j] ? 1 : 0)) << (j - i);
                v |= t;
            }
            blockStates.push_back(*(int64_t*)&v);
        }
    }

public:
    int const fY;
    std::vector<std::shared_ptr<Block const>> fPalette;
    std::vector<blocks::BlockId> fBlockIdPalette;
    std::vector<uint16_t> fPaletteIndices;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
};

} // namespace detail
} // namespace mcfile
