#pragma once

namespace mcfile::je {

class Chunk {
public:
    enum : int {
        kDataVersion = 2865,
    };

    std::shared_ptr<Block const> blockAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        auto const &section = unsafeSectionAtBlock(y);
        if (!section) {
            return nullptr;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        return section->blockAt(offsetX, offsetY, offsetZ);
    }

    std::shared_ptr<Block const> blockAt(Pos3i pos) const {
        return blockAt(pos.fX, pos.fY, pos.fZ);
    }

    blocks::BlockId blockIdAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return blocks::unknown;
        }
        auto const &section = unsafeSectionAtBlock(y);
        if (!section) {
            return blocks::unknown;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        auto block = section->blockAt(offsetX, offsetY, offsetZ);
        if (block) {
            return block->fId;
        } else {
            return blocks::minecraft::air;
        }
    }

    blocks::BlockId blockIdAt(Pos3i pos) const {
        return blockIdAt(pos.fX, pos.fY, pos.fZ);
    }

    bool setBlockAt(int x, int y, int z, std::shared_ptr<Block const> const &block, SetBlockOptions options = SetBlockOptions()) {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return false;
        }
        auto const &section = sectionAtBlock(y);
        if (!section) {
            return false;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        if (!section->setBlockAt(offsetX, offsetY, offsetZ, block)) {
            return false;
        }
        if (options.fRemoveTileEntity) {
            return removeTileEntityAt(x, y, z);
        } else {
            return true;
        }
    }

    bool setBlockAt(Pos3i pos, std::shared_ptr<Block const> const &block, SetBlockOptions options = SetBlockOptions()) {
        return setBlockAt(pos.fX, pos.fY, pos.fZ, block, options);
    }

    std::shared_ptr<nbt::CompoundTag const> tileEntityAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        Pos3i pos(x, y, z);
        auto found = fTileEntities.find(pos);
        if (found == fTileEntities.end()) {
            return nullptr;
        }
        return found->second;
    }

    std::shared_ptr<nbt::CompoundTag> tileEntityAt(int x, int y, int z) {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        Pos3i pos(x, y, z);
        auto found = fTileEntities.find(pos);
        if (found == fTileEntities.end()) {
            return nullptr;
        }
        return found->second;
    }

    bool removeTileEntityAt(int x, int y, int z) {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return false;
        }
        Pos3i pos(x, y, z);
        fTileEntities.erase(pos);
        return true;
    }

    bool removeTileEntityAt(Pos3i pos) {
        return removeTileEntityAt(pos.fX, pos.fY, pos.fZ);
    }

    biomes::BiomeId biomeAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return biomes::unknown;
        }
        if (fLegacyBiomes.size() == 256) {
            int lx = x - fChunkX * 16;
            int lz = z - fChunkZ * 16;
            return fLegacyBiomes[lz * 16 + lx];
        } else if (fLegacyBiomes.size() == 1024 || fLegacyBiomes.size() == 1536) {
            int lx = (x - fChunkX * 16) / 4;
            int ly = (y - fChunkY * 16) / 4;
            int lz = (z - fChunkZ * 16) / 4;
            int index = (ly * 4 + lz) * 4 + lx;
            if (index < fLegacyBiomes.size()) {
                return fLegacyBiomes[index];
            } else {
                return biomes::unknown;
            }
        } else {
            auto const &section = unsafeSectionAtBlock(y);
            if (!section) {
                return biomes::unknown;
            }
            int const offsetX = x - chunkX * 16;
            int const offsetZ = z - chunkZ * 16;
            int const offsetY = y - section->y() * 16;
            auto ret = section->biomeAt(offsetX, offsetY, offsetZ);
            if (ret) {
                return *ret;
            } else {
                return biomes::unknown;
            }
        }
    }

    bool setBiomeAt(int x, int y, int z, biomes::BiomeId biome) {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return false;
        }
        if (fLegacyBiomes.size() == 256) {
            int lx = x - fChunkX * 16;
            int lz = z - fChunkZ * 16;
            fLegacyBiomes[lz * 16 + lx] = biome;
            return true;
        } else if (fLegacyBiomes.size() == 1024 || fLegacyBiomes.size() == 1536) {
            int lx = (x - fChunkX * 16) / 4;
            int ly = (y - fChunkY * 16) / 4;
            int lz = (z - fChunkZ * 16) / 4;
            int index = (ly * 4 + lz) * 4 + lx;
            if (index < fLegacyBiomes.size()) {
                fLegacyBiomes[index] = biome;
                return true;
            } else {
                return false;
            }
        } else {
            auto const &section = sectionAtBlock(y);
            if (!section) {
                return false;
            }
            int const offsetX = x - chunkX * 16;
            int const offsetZ = z - chunkZ * 16;
            int const offsetY = y - section->y() * 16;
            return section->setBiomeAt(offsetX, offsetY, offsetZ, biome);
        }
    }

    biomes::BiomeId biomeAt(Pos3i pos) const {
        return biomeAt(pos.fX, pos.fY, pos.fZ);
    }

    biomes::BiomeId biomeAt(int x, int z) const {
        return biomeAt(x, 0, z);
    }

    int chunkY() const { return fChunkY; }

    int maxChunkY() const { return fChunkY < 0 ? 19 : 15; }

    int minBlockX() const { return fChunkX * 16; }
    int maxBlockX() const { return fChunkX * 16 + 15; }

    int minBlockY() const { return fChunkY * 16; }
    int maxBlockY() const { return fChunkY < 0 ? 319 : 255; }

    int minBlockZ() const { return fChunkZ * 16; }
    int maxBlockZ() const { return fChunkZ * 16 + 15; }

    enum class Status {
        UNKNOWN,
        FULL,
    };

    Status status() const {
        // Java Edition 1.15.2 2230 "full"
        // Java Edition 1.14.1 1957 "full"
        // Java Edition 19w02a 1921 "full"
        // Java Edition 18w47a 1912 "full"
        // Java Edition 18w46a 1910 "fullchunk"
        // Java Edition 18w45a 1908 "fullchunk"
        // Java Edition 18w44a 1907 "fullchunk"
        // Java Edition 18w43a 1901 "fullchunk"
        // Java Edition 1.13.2 1631 "postprocessed" | "fullchunk"
        // Java Edition 1.13.2-pre2 1630 "postprocessed"
        // Java Edition 1.13.2-pre1 1629 "postprocessed"
        // Java Edition 1.13.1-pre1 1626 "postprocessed"
        // ...
        // Java Edition 18w30a 1620 "postprocessed"
        // ...
        // Java Edition 1.13-pre1 1501 "postprocessed"
        // ...
        // Java Edition 18w15a 1482 "finalized"
        // Java Edition 18w14b 1481 "postprocessed"
        // Java Edition 18w14a 1479 "postprocessed"
        // Java Edition 18w11a 1478 "postprocessed"
        // Java Edition 18w08a 1470 "postprocessed"
        // ...
        // Java Edition 18w07a 1467 "postprocessed"
        // Java Edition 18w06a 1466 "postprocessed"
        // Java Edition 18w05a 1464 TerrainPopulated 1
        // ...
        // Java Edition 18w01a 1459 TerrainPopulated 1
        // ...
        // Java Edition 17w47a 1451 TerrainPopulated 1
        if (fDataVersion >= 1912) {
            if (fStatus == "full") {
                return Status::FULL;
            } else {
                return Status::UNKNOWN;
            }
        } else if (fDataVersion >= 1466) {
            if (fStatus == "postprocessed" || fStatus == "finalized" || fStatus == "fullchunk") {
                return Status::FULL;
            } else {
                return Status::UNKNOWN;
            }
        } else {
            if (fTerrainPopulated && *fTerrainPopulated) {
                return Status::FULL;
            } else {
                return Status::UNKNOWN;
            }
        }
    }

    static std::shared_ptr<Chunk> MakeChunk(int chunkX, int chunkZ, std::shared_ptr<nbt::CompoundTag> const &root) {
        if (!root) {
            return nullptr;
        }

        int dataVersion = 0;
        auto dataVersionTag = root->int32("DataVersion");
        if (dataVersionTag) {
            // *.mca created by Minecraft 1.2.1 does not have /DataVersion tag
            dataVersion = *dataVersionTag;
        }

        if (dataVersion <= 2840) {
            // 2835 (21w38a)
            // 2839 (21w41a)
            // 2840 (21w42a)
            return MakeChunk112(chunkX, chunkZ, *root, dataVersion);
        } else {
            // 2844 (21w43a)
            // 2847 (1.18pre1)
            // 2850 (1.18pre4)
            // 2857 (1.19rc2)
            // 2858 (1.18rc3)
            return MakeChunk118(chunkX, chunkZ, *root, dataVersion);
        }
    }

    static std::shared_ptr<Chunk> LoadFromCompressedChunkNbtFile(std::string const &filePath, int chunkX, int chunkZ) = delete;

    static std::shared_ptr<Chunk> LoadFromCompressedChunkNbtFile(std::filesystem::path const &filePath, int chunkX, int chunkZ) {
        std::error_code ec;
        auto size = std::filesystem::file_size(filePath, ec);
        if (ec) {
            return nullptr;
        }
        FILE *in = File::Open(filePath, File::Mode::Read);
        if (!in) {
            return nullptr;
        }
        auto result = LoadFromCompressedChunkNbtFile(in, size, chunkX, chunkZ);
        fclose(in);
        return result;
    }

    static std::shared_ptr<Chunk> LoadFromCompressedChunkNbtFile(FILE *in, uint64_t size, int chunkX, int chunkZ) {
        if (!in) {
            return nullptr;
        }
        std::vector<uint8_t> buffer(size);
        if (!File::Fread(buffer.data(), size, 1, in)) {
            return nullptr;
        }
        auto root = nbt::CompoundTag::ReadCompressed(buffer, Endian::Big);
        if (!root) {
            return nullptr;
        }
        return MakeChunk(chunkX, chunkZ, root);
    }

    Chunk(int cx, int cy, int cz, int dataVersion = kDataVersion)
        : fChunkX(cx)
        , fChunkY(cy)
        , fChunkZ(cz)
        , fDataVersion(dataVersion)
        , fStatus("full")
        , fCreateEmptySection(chunksection::ChunkSectionGenerator::GetEmptySectionCreatorFromDataVersion(dataVersion)) {
    }

protected:
    explicit Chunk(Chunk &s)
        : fChunkX(s.fChunkX)
        , fChunkY(s.fChunkY)
        , fChunkZ(s.fChunkZ)
        , fDataVersion(s.fDataVersion)
        , fLastUpdate(s.fLastUpdate)
        , fStatus(s.fStatus)
        , fTerrainPopulated(s.fTerrainPopulated)
        , fCreateEmptySection(s.fCreateEmptySection) {
        fSections.swap(s.fSections);
        fEntities.swap(s.fEntities);
        fTileEntities.swap(s.fTileEntities);
        fStructures.swap(s.fStructures);
        fTileTicks.swap(s.fTileTicks);
        fLiquidTicks.swap(s.fLiquidTicks);
    }

private:
    Chunk(int chunkX, int chunkY, int chunkZ,
          std::vector<std::shared_ptr<ChunkSection>> const &sections,
          int dataVersion,
          std::vector<std::shared_ptr<nbt::CompoundTag>> &entities,
          std::vector<std::shared_ptr<nbt::CompoundTag>> &tileEntities,
          std::shared_ptr<nbt::CompoundTag> const &structures,
          int64_t lastUpdate,
          std::vector<TickingBlock> tileTicks,
          std::vector<TickingBlock> liquidTicks,
          std::string const &status,
          std::optional<bool> terrainPopulated,
          std::function<std::shared_ptr<ChunkSection>(int sectionY)> createEmptySection)
        : fChunkX(chunkX)
        , fChunkY(chunkY)
        , fChunkZ(chunkZ)
        , fDataVersion(dataVersion)
        , fStructures(structures)
        , fLastUpdate(lastUpdate)
        , fTileTicks(tileTicks)
        , fLiquidTicks(liquidTicks)
        , fStatus(status)
        , fTerrainPopulated(terrainPopulated)
        , fCreateEmptySection(createEmptySection) {
        int maxChunkSectionY = fChunkY;
        for (auto const &section : sections) {
            int const y = section->y();
            maxChunkSectionY = std::max(maxChunkSectionY, y);
        }
        fSections.resize(maxChunkSectionY - fChunkY + 1);
        for (auto const &section : sections) {
            int const y = section->y();
            if (y < fChunkY) {
                continue;
            }
            int const idx = y - fChunkY;
            fSections[idx] = section;
        }
        fEntities.swap(entities);
        for (auto const &item : tileEntities) {
            assert(item);
            auto x = item->int32("x");
            auto y = item->int32("y");
            auto z = item->int32("z");
            if (!x || !y || !z) {
                continue;
            }
            Pos3i pos(*x, *y, *z);
            fTileEntities.insert(std::make_pair(pos, item));
        }
    }

private:
    static std::shared_ptr<Chunk> MakeChunk118(int chunkX, int chunkZ, nbt::CompoundTag const &tag, int dataVersion) {
        using namespace std;

        auto sectionsTag = tag.listTag("sections");
        if (!sectionsTag) {
            return nullptr;
        }

        auto chunkY = tag.int32("yPos");
        if (!chunkY) {
            return nullptr;
        }

        vector<shared_ptr<ChunkSection>> sections;
        //NOTE: blockEntities argument for MakeChunkSections is only used for ChunkSection112.
        vector<shared_ptr<nbt::CompoundTag>> emptyBlockEntities;
        auto createEmptySection = chunksection::ChunkSectionGenerator::MakeChunkSections(sectionsTag, dataVersion, chunkX, chunkZ, emptyBlockEntities, sections);

        //NOTE: Always empty
        vector<shared_ptr<nbt::CompoundTag>> entities;

        auto structures = tag.compoundTag("structures");

        auto lastUpdateTag = tag.int64("LastUpdate");
        int64_t lastUpdate = 0;
        if (lastUpdateTag) {
            lastUpdate = *lastUpdateTag;
        }

        vector<shared_ptr<nbt::CompoundTag>> blockEntities;
        auto blockEntitiesList = tag.listTag("block_entities");
        if (blockEntitiesList && blockEntitiesList->fType == nbt::Tag::Type::Compound) {
            for (auto it : *blockEntitiesList) {
                auto comp = it->asCompound();
                if (!comp) {
                    continue;
                }
                auto copy = make_shared<nbt::CompoundTag>();
                copy->fValue = comp->fValue;
                blockEntities.push_back(copy);
            }
        }

        auto blockTicksTag = tag.listTag("block_ticks");
        vector<TickingBlock> blockTicks;
        if (blockTicksTag) {
            for (auto it : *blockTicksTag) {
                auto item = it->asCompound();
                if (!item) {
                    continue;
                }
                auto tb = TickingBlock::FromCompound(*item);
                if (!tb) {
                    continue;
                }
                blockTicks.push_back(*tb);
            }
        }

        auto fluidTicksTag = tag.listTag("fluid_ticks");
        vector<TickingBlock> fluidTicks;
        if (fluidTicksTag) {
            for (auto it : *fluidTicksTag) {
                auto item = it->asCompound();
                if (!item) {
                    continue;
                }
                auto tb = TickingBlock::FromCompound(*item);
                if (!tb) {
                    continue;
                }
                fluidTicks.push_back(*tb);
            }
        }

        string s;
        auto status = tag.stringTag("Status");
        if (status) {
            s = status->fValue;
        }

        return std::shared_ptr<Chunk>(new Chunk(chunkX, *chunkY, chunkZ, sections, dataVersion,
                                                entities, blockEntities, structures, lastUpdate,
                                                blockTicks, fluidTicks, s, nullopt, createEmptySection));
    }

    static std::shared_ptr<Chunk> MakeChunk112(int chunkX, int chunkZ, nbt::CompoundTag &tag, int dataVersion) {
        using namespace std;

        auto level = tag.compoundTag("Level");
        if (!level) {
            return nullptr;
        }

        vector<shared_ptr<nbt::CompoundTag>> tileEntities;
        auto tileEntitiesList = level->listTag("TileEntities");
        if (tileEntitiesList && tileEntitiesList->fType == nbt::Tag::Type::Compound) {
            for (auto it : *tileEntitiesList) {
                auto comp = it->asCompound();
                if (!comp) {
                    continue;
                }
                auto copy = make_shared<nbt::CompoundTag>();
                copy->fValue = comp->fValue;
                tileEntities.push_back(copy);
            }
        }

        auto sectionsTag = level->listTag("Sections");
        if (!sectionsTag) {
            return nullptr;
        }
        vector<shared_ptr<ChunkSection>> sections;
        auto createEmptySection = chunksection::ChunkSectionGenerator::MakeChunkSections(sectionsTag, dataVersion, chunkX, chunkZ, tileEntities, sections);

        auto biomesTag = level->query("Biomes");
        vector<biomes::BiomeId> biomes;
        ParseBiomes(biomesTag, dataVersion, biomes);

        vector<shared_ptr<nbt::CompoundTag>> entities;
        auto entitiesTag = level->listTag("Entities");
        if (entitiesTag && entitiesTag->fType == nbt::Tag::Type::Compound) {
            auto entitiesList = entitiesTag->asList();
            for (auto it : *entitiesList) {
                auto comp = it->asCompound();
                if (!comp) {
                    continue;
                }
                auto copy = make_shared<nbt::CompoundTag>();
                copy->fValue = comp->fValue;
                entities.push_back(copy);
            }
        }

        string s;
        auto status = level->stringTag("Status");
        if (status) {
            s = status->fValue;
        }
        std::optional<bool> terrainPopulated = level->boolean("TerrainPopulated");

        auto structures = level->compoundTag("Structures");

        auto lastUpdateTag = level->int64("LastUpdate");
        int64_t lastUpdate = 0;
        if (lastUpdateTag) {
            lastUpdate = *lastUpdateTag;
        }

        auto tileTicksTag = level->listTag("TileTicks");
        std::vector<TickingBlock> tileTicks;
        if (tileTicksTag) {
            for (auto it : *tileTicksTag) {
                auto item = it->asCompound();
                if (!item) {
                    continue;
                }
                auto tb = TickingBlock::FromCompound(*item);
                if (!tb) {
                    continue;
                }
                tileTicks.push_back(*tb);
            }
        }

        auto liquidTicksTag = level->listTag("LiquidTicks");
        std::vector<TickingBlock> liquidTicks;
        if (liquidTicksTag) {
            for (auto it : *liquidTicksTag) {
                auto item = it->asCompound();
                if (!item) {
                    continue;
                }
                auto tb = TickingBlock::FromCompound(*item);
                if (!tb) {
                    continue;
                }
                liquidTicks.push_back(*tb);
            }
        }

        auto ret = std::shared_ptr<Chunk>(new Chunk(chunkX, 0, chunkZ, sections, dataVersion,
                                                    entities, tileEntities, structures, lastUpdate,
                                                    tileTicks, liquidTicks, s, terrainPopulated, createEmptySection));
        ret->fLegacyBiomes.swap(biomes);
        return ret;
    }

    std::shared_ptr<ChunkSection> unsafeSectionAtBlock(int y) const {
        int const sectionIndex = Coordinate::ChunkFromBlock(y);
        int const idx = sectionIndex - fChunkY;
        if (idx < 0 || fSections.size() <= idx) {
            return nullptr;
        } else {
            return fSections[idx];
        }
    }

    std::shared_ptr<ChunkSection> sectionAtBlock(int y) {
        int const cy = Coordinate::ChunkFromBlock(y);
        int const idx = cy - fChunkY;
        std::shared_ptr<ChunkSection> section;
        if (0 <= idx && idx < fSections.size()) {
            section = fSections[idx];
        }
        if (!section && fChunkY <= cy && cy <= maxChunkY() && fCreateEmptySection) {
            section = fCreateEmptySection(cy);
            if (section) {
                if (fSections.size() <= idx) {
                    fSections.resize(idx + 1);
                }
                fSections[idx] = section;
            }
        }
        return section;
    }

    static void ParseBiomes(nbt::Tag const *biomesTag, int dataVersion, std::vector<biomes::BiomeId> &out) {
        using namespace std;
        if (!biomesTag) {
            return;
        }

        if (biomesTag->type() == nbt::Tag::Type::IntArray) {
            vector<int32_t> const &value = biomesTag->asIntArray()->value();
            size_t const size = value.size();
            if (size != 256 && size != 1024 && size != 1536) {
                return;
            }
            out.resize(size);
            for (int i = 0; i < size; i++) {
                out[i] = biomes::FromInt(value[i]);
            }
        } else if (biomesTag->type() == nbt::Tag::Type::ByteArray) {
            vector<uint8_t> const &value = biomesTag->asByteArray()->value();
            if (value.size() != 256) {
                return;
            }
            out.resize(256);
            for (int i = 0; i < 256; i++) {
                out[i] = biomes::FromInt(value[i]);
            }
        }
    }

public:
    int const fChunkX;
    int const fChunkY;
    int const fChunkZ;
    std::vector<std::shared_ptr<ChunkSection>> fSections;
    int const fDataVersion;
    std::vector<std::shared_ptr<nbt::CompoundTag>> fEntities;
    std::unordered_map<Pos3i, std::shared_ptr<nbt::CompoundTag>, Pos3iHasher> fTileEntities;
    std::shared_ptr<mcfile::nbt::CompoundTag> fStructures;
    int64_t fLastUpdate;
    std::vector<TickingBlock> fTileTicks;
    std::vector<TickingBlock> fLiquidTicks;

    // When empty, biomes are stored in fSections.
    // When size = 256, biomes are 2D. Biome at localX, localZ is in fLegacyBiomes[localZ * 16 + localX]
    // When size = 1024 or 1536, biomes are 3D. Biomes at localX, localY, localZ is in fLegacyBiomes[localX / 4 + (localZ / 4) * 4 + (localY / 4) * 16]
    std::vector<biomes::BiomeId> fLegacyBiomes;

protected:
    std::string const fStatus;
    std::optional<bool> fTerrainPopulated;
    std::function<std::shared_ptr<ChunkSection>(int sectionY)> const fCreateEmptySection;
};

} // namespace mcfile::je
