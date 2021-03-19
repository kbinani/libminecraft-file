#pragma once

namespace mcfile::chunksection {

template <class BlockStatesParser>
class ChunkSection113Base : public ChunkSection {
public:
    std::shared_ptr<Block const> blockAt(int offsetX, int offsetY, int offsetZ) const override {
        auto const index = paletteIndexAt(offsetX, offsetY, offsetZ);
        if (!index) {
            return nullptr;
        }
        return fPalette[*index];
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
        auto const index = paletteIndexAt(offsetX, offsetY, offsetZ);
        if (!index) {
            return blocks::unknown;
        }
        return fBlockIdPalette[*index];
    }
    
    bool blockIdWithYOffset(int offsetY, std::function<bool(int offsetX, int offsetZ, blocks::BlockId blockId)> callback) const override {
        if (offsetY < 0 || 16 <= offsetY) {
            return false;
        }
        for (int offsetZ = 0; offsetZ < 16; offsetZ++) {
            for (int offsetX = 0; offsetX < 16; offsetX++) {
                auto index = paletteIndexAt(offsetX, offsetY, offsetZ);
                blocks::BlockId id = blocks::unknown;
                if (index) {
                    id = fBlockIdPalette[*index];
                }
                if (!callback(offsetX, offsetZ, id)) {
                    return false;
                }
            }
        }
        return true;
    }
    
    int y() const override {
        return fY > 251 ? fY - 256 : fY;
    }
    
    int rawY() const override {
        return fY;
    }

    std::vector<std::shared_ptr<Block const>> const& palette() const override {
        return fPalette;
    }
    
    std::optional<size_t> paletteIndexAt(int offsetX, int offsetY, int offsetZ) const override {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return std::nullopt;
        }
        auto index = BlockIndex(offsetX, offsetY, offsetZ);
        if (index < 0 || fPaletteIndices.size() <= index) {
            return std::nullopt;
        }

        int paletteIndex = fPaletteIndices[index];
        if (paletteIndex < 0 || fPalette.size() <= paletteIndex) {
            return std::nullopt;
        }
        
        return (size_t)paletteIndex;
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
            BlockStatesParser::BlockStatesFromPaletteIndices(fPalette.size(), fPaletteIndices, blockStates);
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
        std::vector<std::shared_ptr<Block const>> palette;
        if (paletteTag) {
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
        }
        
        std::vector<blocks::BlockId> blockIdPalette(palette.size());
        for (int i = 0; i < palette.size(); i++) {
            auto block = palette[i];
            blockIdPalette[i] = blocks::FromName(block->fName);
        }
        
        auto blockStatesTag = section->query("BlockStates")->asLongArray();
        std::vector<uint16_t> paletteIndices;
        if (blockStatesTag) {
            BlockStatesParser::PaletteIndicesFromBlockStates(blockStatesTag->value(), paletteIndices);
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
        
        return std::shared_ptr<ChunkSection>(new ChunkSection113Base((int)yTag->fValue,
                                                                        palette,
                                                                        paletteIndices,
                                                                        blockIdPalette,
                                                                        blockLight,
                                                                        skyLight));
    }
    
private:
    ChunkSection113Base(int y,
                        std::vector<std::shared_ptr<Block const>> const& palette,
                        std::vector<uint16_t> const& paletteIndices,
                        std::vector<blocks::BlockId> const& blockIdPalette,
                        std::vector<uint8_t> const& blockLight,
                        std::vector<uint8_t> const& skyLight)
        : fY(y)
        , fPalette(palette)
        , fPaletteIndices(paletteIndices)
        , fBlockIdPalette(blockIdPalette)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight)
    {
    }
    
    static int BlockIndex(int offsetX, int offsetY, int offsetZ) {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        return offsetY * 16 * 16 + offsetZ * 16 + offsetX;
    }

public:
    int const fY;
    std::vector<std::shared_ptr<Block const>> fPalette;
    std::vector<blocks::BlockId> fBlockIdPalette;
    std::vector<uint16_t> fPaletteIndices;
    std::vector<uint8_t> fBlockLight;
    std::vector<uint8_t> fSkyLight;
};

} // namespace mcfile::detail
